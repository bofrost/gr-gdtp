gr-gdtp
=======

This is a wrapper component in order to use [libgdtp](https://github.com/andrepuschmann/libgdtp) from within GNU radio.
In essence, the OOTM provides a number of features to build packet radios using GNU Radio, such as:

* Reliable and unreliable communication
* Flow control (limited at the moment)
* Flexible framing (using Google Protobufs for over-the-air encoding)
* Multiplexing (multiple incoming and outgoing flows)
* Scheduling of outgoing flows
* Tunneling of Ethernet/IP traffic

This OOTM is still work in progress and will be complemented over time. Not all features of libgdtp are supported yet.
The module suports multiple PHY standards including IEEE 802.11 and 802.15.4.

# Installation

## GNU Radio v3.7

You need at least version 3.7.3.

There are several ways to install GNU Radio. You can use

- [pybombs](http://gnuradio.org/redmine/projects/pybombs/wiki)
- [pre-combiled binaries](http://gnuradio.org/redmine/projects/gnuradio/wiki/BinaryPackages)
- [from source](http://gnuradio.org/redmine/projects/gnuradio/wiki/InstallingGRFromSource)


## libgdtp

- libgdtp


## PHY layer

- gr-ieee802-11 or gr-ieee-802-15-4
