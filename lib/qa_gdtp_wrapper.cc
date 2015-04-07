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


#include <gnuradio/attributes.h>
#include <cppunit/TestAssert.h>
#include "qa_gdtp_wrapper.h"
#include <gdtp/gdtp_wrapper.h>

namespace gr {
  namespace gdtp {

    void
    qa_gdtp_wrapper::t1()
    {
        // Main testing is done inside libgdtp itself.
        // FIXME: add proper tests for GR wrapper nonetheless.
        uint8_t rx_bytes[] = { 0x08, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x5a, 0xb6};
        CPPUNIT_ASSERT(sizeof(rx_bytes) == 11);
    }

  } /* namespace gdtp */
} /* namespace gr */

