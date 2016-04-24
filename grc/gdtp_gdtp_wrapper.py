MAIN_TMPL = """\
<?xml version="1.0"?>
<block>
  <name>GDTP</name>
  <key>gdtp_gdtp_wrapper</key>
  <category>gdtp</category>
  <import>import gdtp</import>
  <make>gdtp.gdtp_wrapper(\$debug, \$src_addr, \$dest_addr, \$reliable, \$addr_mode, \$addr_source, \$ack_timeout, \$max_retry, \$max_seq_no, \$scheduler, \$num_flows)</make>
  
  <param>
    <name>Debug</name>
    <key>debug</key>
    <value>False</value>
    <type>bool</type>

    <option>
      <name>Enable</name>
      <key>True</key>
    </option>
    <option>
      <name>Disable</name>
      <key>False</key>
    </option>
  </param>  
       
  <param>
    <name>Source Address</name>
    <key>src_addr</key>
    <value>1</value>
    <type>int</type>
  </param>
  
  <param>
    <name>Destination Address</name>
    <key>dest_addr</key>
    <value>2</value>
    <type>int</type>
  </param>
  
  <param>
    <name>Reliable</name>
    <key>reliable</key>
    <value>[0]</value>
    <type>int_vector</type>
  </param>    
  
  <param>
    <name>Addressing Mode</name>
    <key>addr_mode</key>
    <value>Explicit</value>
    <type>string</type>

    <option>
      <name>Explicit</name>
      <key>explicit</key>
    </option>
    <option>
      <name>Implicit</name>
      <key>implicit</key>
    </option>
  </param>    

  <param>
    <name>Address source</name>
    <key>addr_source</key>
    <value>tun0</value>
    <type>string</type>
  </param>  

  <param>
    <name>ACK Timeout</name>
    <key>ack_timeout</key>
    <value>100</value>
    <type>int</type>
  </param>

  <param>
    <name>Max. retransmissions</name>
    <key>max_retry</key>
    <value>7</value>
    <type>int</type>
  </param>

  <param>
    <name>Max. sequence number</name>
    <key>max_seq_no</key>
    <value>127</value>
    <type>int</type>
  </param>
  
  <param>
    <name>Scheduler</name>
    <key>scheduler</key>
    <value>FIFO</value>
    <type>string</type>

    <option>
      <name>FIFO</name>
      <key>fifo</key>
    </option>
  </param>  
       
  <param>
    <name>Flows</name>
    <key>num_flows</key>
    <value>1</value>
    <type>int</type>
  </param>
  #for $n in range($max_nflows)
  <param>
    <name>Flow$(n): Reliable</name>
    <key>reliable$(n)</key>
    <value>True</value>
    <type>bool</type>
    <hide>\#if \$num_flows() > $(n) then 'none' else 'all'#</hide>
    <option>
      <name>Yes</name>
      <key>True</key>
    </option>
    <option>
      <name>No</name>
      <key>False</key>
    </option>
  </param>
  #end for
  <sink>
    <name>flowin</name>
    <type>message</type>
    <nports>\$num_flows</nports>
    <optional>1</optional>
  </sink>

  <sink>
    <name>fromMAC</name>
    <type>message</type>
    <nports>1</nports>
    <optional>1</optional>
  </sink>

  <source>
    <name>fer</name>
    <type>message</type>
    <optional>1</optional>
  </source>

  <source>
    <name>flowout</name>
    <type>message</type>
    <nports>\$num_flows</nports>
    <optional>1</optional>
  </source>  

  <source>
    <name>toMAC</name>
    <type>message</type>
    <nports>1</nports>
    <optional>1</optional>
  </source>

</block>
"""


def parse_tmpl(_tmpl, **kwargs):
    from Cheetah import Template
    return str(Template.Template(_tmpl, kwargs))

max_num_flows = 32

import sys
open('./gdtp_gdtp_wrapper.xml', 'w').write(parse_tmpl(MAIN_TMPL,
                                                      max_nflows=max_num_flows,
                                                      ))
