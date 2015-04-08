/* -*- c++ -*- */
/*
 * Copyright 2015, Andre Puschmann <andre.puschmann@tu-ilmenau.de>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <gnuradio/block_detail.h>

#include <iostream>
#include <string>

#include "libgdtp/libgdtp.h"
#include "gdtp_wrapper_impl.h"


/**
 *  The basic skeleton of this block is based on the RIME stack block written
 *  by Christoph Leitner as part of gr-ieee802-15-4.
 */

namespace gr {
  namespace gdtp {

    gdtp_wrapper::sptr
    gdtp_wrapper::make(bool debug, uint64_t src_addr, uint64_t dest_addr, bool reliable, std::string addr_mode, std::string addr_src, int ack_timeout, int max_retry, int max_seq_no, std::string scheduler)
    {
      return gnuradio::get_initial_sptr
        (new gdtp_wrapper_impl(debug, src_addr, dest_addr, reliable, addr_mode, addr_src, ack_timeout, max_retry, max_seq_no, scheduler));
    }

    /*
     * The private constructor
     */
    gdtp_wrapper_impl::gdtp_wrapper_impl(bool debug, uint64_t src_addr, uint64_t dest_addr, bool reliable, std::string addr_mode, std::string addr_src, int ack_timeout, int max_retry, int max_seq_no, std::string scheduler)
      : gr::block("gdtp_wrapper",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
        debug_(debug),
        src_address_(src_addr),
        dest_address_(dest_addr),
        addr_mode_(addr_mode),
        addr_src_(addr_src),
        reliable_(reliable),
        ack_timeout_(ack_timeout),
        max_retry_(max_retry),
        max_seq_no_(max_seq_no),
        scheduler_(scheduler)
    {
        // register message ports
        std::string inport_name("fromMAC");
        std::string outport_name("toMAC");
        mac_in = pmt::mp(inport_name);
        mac_out = pmt::mp(outport_name);

        message_port_register_in(mac_in);
        set_msg_handler(mac_in, boost::bind(&gdtp_wrapper_impl::unpack, this, inport_name, _1));
        message_port_register_out(mac_out);

        // configure and init gdtp instance
        gdtp_.set_scheduler_type(scheduler_);
        gdtp_.set_default_local_address(src_address_);
        gdtp_.set_default_destination_address(dest_address_);
        gdtp_.initialize();

        // register flow message ports
        int num_flows = 1; // FIXME: limited to one at the moment
        if(num_flows != 1)
            throw std::invalid_argument("Only a single flow is supported through this block at the moment.");
        register_flows(num_flows, "flowin", "flowout");

        // start transmit thread
        threads_.push_back(new boost::thread(boost::bind(&gdtp_wrapper_impl::tx_handler, this)));
    }


    void gdtp_wrapper_impl::register_flows(int num_flows, const char *inport_base, const char *outport_base)
    {
        std::cout << "Registering " << num_flows << " flows." << std::endl;

        if(std::strlen(inport_base) == 0 || std::strlen(outport_base) == 0) {
            throw std::invalid_argument("No in/outport name specified.");
        }

        for (int i = 0; i < num_flows; i++) {
            std::string inport_name(inport_base);
            std::string outport_name(outport_base);
            if (num_flows != 1) {
                inport_name += std::to_string(i);
                outport_name += std::to_string(i);
            }

            pmt::pmt_t inport = pmt::mp(inport_name);
            pmt::pmt_t outport = pmt::mp(outport_name);
            message_port_register_out(outport);
            message_port_register_in(inport);

            // construct properties
            FlowProperty props((reliable_ == true ? RELIABLE : UNRELIABLE),
                           (addr_mode_ == "implicit" ? IMPLICIT : EXPLICIT),
                            max_seq_no_,
                            ack_timeout_,
                            max_retry_,
                            addr_src_);

            // allocate flow
            FlowId id = gdtp_.allocate_flow(i, props);
            gdtpPorts_[inport_name] = id;

            // setup handler for "incoming" msgs
            set_msg_handler(inport, boost::bind(&gdtp_wrapper_impl::pack, this, inport_name, id, _1));

            // create thread for outgoing msgs for this flow, i.e., when libgdtp has frames
            threads_.push_back(new boost::thread(boost::bind(&gdtp_wrapper_impl::flowout_handler, this, outport_base, id)));
        }
    }

    void gdtp_wrapper_impl::pack(std::string inport_name, FlowId id, pmt::pmt_t msg)
    {
        if (debug_) std::cout << "pack called on portname " << inport_name << std::endl;
        size_t msg_len;
        const char *sdu;

        if (pmt::is_eof_object(msg)) {
            message_port_pub(mac_out, pmt::PMT_EOF);
            detail().get()->set_done(true);
            return;
        } else if (pmt::is_symbol(msg)) {
            std::string str;
            str = pmt::symbol_to_string(msg);
            msg_len = str.length();
            sdu = str.data();
        } else if(pmt::is_pair(msg)) {
            msg_len = pmt::blob_length(pmt::cdr(msg));
            sdu = reinterpret_cast<const char *>(pmt::blob_data(pmt::cdr(msg)));
        } else {
            throw std::invalid_argument("GDTP expects PDUs or strings");
            return;
        }

        std::shared_ptr<Sdu> frame = std::make_shared<Sdu>(sdu, sdu + msg_len);
        gdtp_.handle_frame_from_above(frame, id);
    }


    void gdtp_wrapper_impl::unpack(std::string inport_name, pmt::pmt_t msg)
    {
        if (debug_) std::cout << "Unpack called on port " << inport_name << std::endl;

        if (pmt::is_pair(msg)) {
            size_t msg_len = pmt::blob_length(pmt::cdr(msg));
            const char* msdu = reinterpret_cast<const char *>(pmt::blob_data(pmt::cdr(msg)));
            Sdu frame(msdu, msdu + msg_len);
            if (debug_) std::cout << "Receiving PDU with size " << frame.size() << std::endl;
            gdtp_.handle_frame_from_below(frame, DEFAULT_LOWER_LAYER_PORT);
        } else {
            throw std::runtime_error("PMT must be blob");
        }
    }


    void gdtp_wrapper_impl::tx_handler()
    {
        std::cout << "transmit thread started." << std::endl;

        try
        {
            while (true)
            {
                boost::this_thread::interruption_point();
                while (true) {
                    Sdu pdu;
                    // this call may block if no frames are present
                    gdtp_.get_frame_for_below(pdu, DEFAULT_LOWER_LAYER_PORT);
                    if (debug_) std::cout << "Transmitting PDU with size " << pdu.size() << std::endl;

                    pmt::pmt_t msg = pmt::make_blob(pdu.data(), pdu.size());
                    message_port_pub(mac_out, pmt::cons(pmt::PMT_NIL, msg));

                    // FIXME: wait for txover event
                    gdtp_.set_frame_transmitted(DEFAULT_LOWER_LAYER_PORT);
                }
            }
        }
        catch(boost::thread_interrupted)
        {
            std::cout << "Tx thread interrupted (id: " << boost::this_thread::get_id() << ")." << std::endl;
        }
    }


    void gdtp_wrapper_impl::flowout_handler(std::string outport_name, FlowId id)
    {
        std::cout << "flowout handler thread for " << outport_name << " started." << std::endl;

        try
        {
            while (true)
            {
                boost::this_thread::interruption_point();
                while (true) {
                    std::shared_ptr<Sdu> data = gdtp_.get_frame_for_above(id);
                    if (debug_) std::cout << "received: " << data->size() << " byte." << std::endl;
                    pmt::pmt_t msg = pmt::make_blob(data->data(), data->size());
                    message_port_pub(pmt::mp(outport_name), pmt::cons(pmt::PMT_NIL, msg));
                }
            }
        }
        catch(boost::thread_interrupted)
        {
            std::cout << "flowout thread for " << outport_name << " interrupted (id: " << boost::this_thread::get_id() << ")." << std::endl;
        }
    }


    /*
     * Our virtual destructor.
     */
    gdtp_wrapper_impl::~gdtp_wrapper_impl()
    {
        // stop threads
        boost::ptr_vector<boost::thread>::iterator it;
        for (it = threads_.begin(); it != threads_.end(); ++it) {
            it->interrupt();
            it->join();
        }
    }

  } /* namespace gdtp */
} /* namespace gr */

