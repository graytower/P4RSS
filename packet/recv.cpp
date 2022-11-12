#include <iostream>

#include <vector>

#include "MyRawSocket.h"
#include "MyProtocol.h"

using namespace std;

int main()
{
    // Create a raw socket
    MyRawSocket my_raw_socket((unsigned char *)"eth0");
    unsigned char local_mac[6];
    my_raw_socket.GetMac(local_mac);

    // Import protocol
    MyProtocol my_protocol;

    // Initiate the receive buffer
    int mtu = 1500;
    unsigned char send_buffer[mtu] = {0};
    unsigned char recv_buffer[mtu] = {0};

    // Start receiving
    while (true)
    {
        int recv_packet_length = my_raw_socket.RecvPacket(recv_buffer, sizeof(recv_buffer));

        int send_index = 0;
        int recv_index = 0;

        // Filter: Length > 14 (has Ethernet)
        if (recv_packet_length < 14)
        {
            continue;
        }

        // Parser: H_ETHERNET
        struct H_ETHERNET recv_ethernet = my_protocol.GetEthernet(recv_buffer, recv_index);
        recv_index += sizeof(recv_ethernet);
        
        // Filter: Get packets which are not sent from local
        if (local_mac[0] == recv_ethernet.src_mac[0] && local_mac[1] == recv_ethernet.src_mac[1] &&
            local_mac[2] == recv_ethernet.src_mac[2] && local_mac[3] == recv_ethernet.src_mac[3] &&
            local_mac[4] == recv_ethernet.src_mac[4] && local_mac[5] == recv_ethernet.src_mac[5])
        {
            continue;
        }
        
        for(int i=0;i<100;i++)
        {
            for(int j=0;j<100;j++)
            {
                for(int k=0;k<12;k++)
                {}
            }
        }
        // Parser: H_TYPE
        struct H_TYPE recv_type = my_protocol.GetType(recv_buffer, recv_index);
        recv_index += sizeof(recv_type);

        // Action: cpuu (0x0801)
        if (recv_type.protocol[0] == (unsigned char)0x08 && recv_type.protocol[1] == (unsigned char)0x01)
        {
            cout<<"recv_cpuu"<<endl;
        }
        else if(recv_type.protocol[0] == (unsigned char)0x08 && recv_type.protocol[1] == (unsigned char)0x00)
        {
            cout<<"recv_ipv4"<<endl;
        }
        else if(recv_type.protocol[0] == (unsigned char)0x03 && recv_type.protocol[1] == (unsigned char)0x01) 
        {
            cout<<"Hash egress"<<endl;
        }
        else if(recv_type.protocol[0] == (unsigned char)0x03 && recv_type.protocol[1] == (unsigned char)0x02)
        {
            cout<<"Choose min_U"<<endl;
        }
        else if(recv_type.protocol[0] == (unsigned char)0x03 && recv_type.protocol[1] == (unsigned char)0x03)
        {
            //cout<<"Hit entry"<<endl;
        }
        else if(recv_type.protocol[0] == (unsigned char)0x03 && recv_type.protocol[1] == (unsigned char)0x04)
        {
            cout<<"Get token"<<endl;
        }
        else if(recv_type.protocol[0] == (unsigned char)0x03 && recv_type.protocol[1] == (unsigned char)0x05)
        {
            cout<<"max hash 0"<<endl;
        }
        else if(recv_type.protocol[0] == (unsigned char)0x03 && recv_type.protocol[1] == (unsigned char)0x05)
        {
            cout<<"mark"<<endl;
        }
        }
}
