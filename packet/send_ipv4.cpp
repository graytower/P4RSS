#include <iostream>

#include "MyRawSocket_ipv4.h"
#include "MyProtocol_ipv4.h"

using namespace std;

int main()
{
    // Create a raw socket
    MyRawSocket my_raw_socket((unsigned char *)"eth0");

    // Import protocol
    MyProtocol_ipv4 my_protocol;

    // Initiate the send buffer
    int mtu = 1500;
    unsigned char send_buffer[mtu] = {0};
    int send_index = 0;

    // Set: H_ETHERNET
    unsigned char send_ethernet_dst_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    unsigned char send_ethernet_src_mac[6] = {0};
    my_raw_socket.GetMac(send_ethernet_src_mac);
    send_index = my_protocol.SetEthernet(send_buffer, send_index,
                                         send_ethernet_dst_mac, sizeof(send_ethernet_dst_mac),
                                         send_ethernet_src_mac, sizeof(send_ethernet_src_mac));

    // Set: H_TYPE
    unsigned char send_type_protocol[2] = {0x08, 0x00};
    send_index = my_protocol.SetType(send_buffer, send_index,
                                     send_type_protocol, sizeof(send_type_protocol));
    
    // Set: H_IP
    unsigned char send_ip_version_ihl[1] = {0x45};
    unsigned char send_ip_diffserv[1] = {0x00};
    unsigned char send_ip_totalLen[2] = {0x00, 0x26};
    unsigned char send_ip_identification[2] = {0x00, 0x00};
    unsigned char send_ip_flags_offset[2] = {0x00, 0x00};
    unsigned char send_ip_ttl[1] = {0x03};
    unsigned char send_ip_protocol_t[1] = {0x06};
    unsigned char send_ip_hdrChecksum[2] = {0x00, 0x00};
    unsigned char send_ip_src_addr[4] = {0x0a, 0x01, 0x01, 0x03};
    unsigned char send_ip_dst_addr[4] = {0x0a, 0x01, 0x01, 0x01};
    send_index = my_protocol.SetIp(send_buffer, send_index,
                                     send_ip_version_ihl, sizeof(send_ip_version_ihl),
                                     send_ip_diffserv, sizeof(send_ip_diffserv),
                                     send_ip_totalLen, sizeof(send_ip_totalLen),
                                     send_ip_identification, sizeof(send_ip_identification),
                                     send_ip_flags_offset, sizeof(send_ip_flags_offset),
                                     send_ip_ttl, sizeof(send_ip_ttl),
                                     send_ip_protocol_t, sizeof(send_ip_protocol_t),
                                     send_ip_hdrChecksum, sizeof(send_ip_hdrChecksum),
                                     send_ip_src_addr, sizeof(send_ip_src_addr),
                                     send_ip_dst_addr, sizeof(send_ip_dst_addr));

    //Just for test, we only change the TCP port number

    
    // Send
    int n = 1;
    while (true)
    {   
        if(n > 500){break;}
 
        // Set: H_TCP
        unsigned char send_tcp_src_port[2] = {0x00, 0x9c};
        unsigned char send_tcp_dst_port[2] = {0x00, 0x22};
        send_index = my_protocol.SetTcp(send_buffer, send_index,
                                    send_tcp_src_port, sizeof(send_tcp_src_port),
                                    send_tcp_dst_port, sizeof(send_tcp_dst_port));
        my_raw_socket.SendPacket(send_buffer, send_index);
        send_index = my_protocol.DelTCP(send_buffer, send_index, sizeof(send_tcp_src_port), sizeof(send_tcp_dst_port));
        cout<<"SendIpv4--"<<n<<" "<<int(send_tcp_src_port[1])<<" "<<int(send_tcp_dst_port[1])<<endl;
        sleep(0.1);
        n++;
    }
}
