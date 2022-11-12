from mininet.net import Mininet
from mininet.topo import Topo
from mininet.link import Link, Intf, TCLink
from mininet.log import setLogLevel, info
from mininet.cli import CLI
from p4_mininet import P4Switch, P4Host

import argparse
import time
import os
os.system("sudo mn -c")


class MyTopo():

    def __init__(self):

        self.sw_path = "simple_switch"
        self.json_path = "../p4_code/p4rss.json"
        self.thrift_port = 9090
        self.device_id = 1
        self.pcap_dump = False

        self.sw_list = []
        self.s0 = None
        self.h_list = []

        self.net = Mininet(host=P4Host,
                           switch=P4Switch,
                           controller=None)

    def CreateNet(self):
        
	# Create switch
        for i in range(1):
            sw = self.net.addSwitch('sw%d' % self.device_id,
                                    sw_path=self.sw_path,
                                    json_path=self.json_path,
                                    thrift_port=self.thrift_port,
                                    nanolog="ipc:///tmp/bm-%d-log.ipc" % self.device_id,
                                    device_id=self.device_id,
                                    pcap_dump=self.pcap_dump)
            self.sw_list.append(sw)
            self.thrift_port += 1
            self.device_id += 1


        # Connect switch and host
        
        for k in range(1,6):
            h = self.net.addHost('host%d' % k,
                                         ip='10.0.%d.%d/32' % (k, k),
                                         mac='00:00:00:%s:%s:%s' % (hex(1)[2:], hex(1)[2:], hex(k)[2:]))
            self.net.addLink(h, self.sw_list[0])
            self.h_list.append(h)


        self.net.start()
           
        for i in [0, 1, 2, 3]:
            h = self.h_list[i]
            l = i + 1
            for j in range(1, 5):
                h.cmd('../packet/recv_%d/recv%d%d >../packet/tmp/%d%d_recv 2>/dev/null &' % (l, l, j, l, j))

        for i in [0, 1, 2, 3]:
            h = self.h_list[i]
            l = i + 1
            h.cmd('../packet/h%d_send_cpuu >../packet/tmp/%d_send_cpuu 2>/dev/null &' % (l, l))

        #for i in [4]:
            #h = self.h_list[i]
            #l = i + 1
            #h.cmd('../packet/send_ipv4 >../packet/tmp/10_1_1_%d_send 2>/dev/null &' % l)

        CLI(self.net)
        self.net.stop()
        
if __name__ == '__main__':
    setLogLevel('info')
    test = MyTopo()
    test.CreateNet()
