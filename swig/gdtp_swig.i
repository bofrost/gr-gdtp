/* -*- c++ -*- */

#define GDTP_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "gdtp_swig_doc.i"

%{
#include "gdtp/gdtp_wrapper.h"
%}


%include "gdtp/gdtp_wrapper.h"
GR_SWIG_BLOCK_MAGIC2(gdtp, gdtp_wrapper);
