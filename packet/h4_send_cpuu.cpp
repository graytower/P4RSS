#include <iostream>

#include "MyRawSocket.h"
#include "MyProtocol.h"
#include <fstream>
#include <string>
#include<time.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
using namespace std;

unsigned int getPID(char* recv)
{
     FILE* fp = NULL;
     char cmd[16];
     sprintf(cmd, "pidof %s", recv);
     if((fp = popen(cmd, "r")) !=NULL)
     {
         fgets(cmd, sizeof(cmd), fp);
         pclose(fp);
         fp = NULL;
     }
     unsigned int result = 0;
     result = strtol(cmd, NULL, 10);
     return result;
}

void taskset(int cid, int PID)
{
     FILE* fp = NULL;
     char cmd[48];
     sprintf(cmd, "taskset -pc %d %d | grep -v \"current\"", cid, PID);
     if((fp = popen(cmd, "r")) !=NULL)
     {
         fgets(cmd, sizeof(cmd), fp);
         pclose(fp);
         fp = NULL;
     }
     cout<<cmd<<endl;
}

int main()
{   std::string sFilename = "cpu4.txt";   
    int cid = 4;
    char recv[7] = "recv";
    for(int i=1;i<5;i++){
        recv[4] = (char)(cid+48);
        recv[5] = (char)(i+48);
        unsigned int rPID = getPID(recv);
        if (rPID == 0)
        {
            cout<<"recv is not started"<<endl;
        }
        else
        {
            cout<<recv<<"'s PID is "<<rPID<<endl;
            taskset(cid, rPID);
        }
    }
    char bsend[13] = "h0_send_cpuu";
    bsend[1] = (char)(cid+48);
    unsigned int sPID = getPID(bsend);
    cout<<bsend<<"'s PID is "<<sPID<<endl;
    taskset(cid, sPID);
    
    // Create a raw socket
    MyRawSocket my_raw_socket((unsigned char *)"eth0");

    // Import protocol
    MyProtocol my_protocol;

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
    unsigned char send_type_protocol[2] = {0x08, 0x01};
    send_index = my_protocol.SetType(send_buffer, send_index,
                                     send_type_protocol, sizeof(send_type_protocol));
   
    int n = 1;
    
    // Send
    while (true){
     
    string buffer;
    system("mpstat -P 4 1 1|awk 'NR==4{print $12}'>cpu4.txt");
    std::ifstream fileSource(sFilename); // Creates an input file stream
    if (!fileSource) {
        cerr << "Canot open " << sFilename << endl;
    }
    else {
        // Intermediate buffer
        
        // By default, the >> operator reads word by workd (till whitespace)
        while (fileSource >> buffer)
        {
            cout << "get cpu remain="<<buffer<<"  ";
        }
    };
    unsigned char *data = (unsigned char *)buffer.c_str();
    unsigned char send_metadata_cpuu[1];  
	if(data[1]=='.'){
		send_metadata_cpuu[0]=100-((int)data[0])+48+128;
	}
	else if(data[3]=='.'){send_metadata_cpuu[0]=0+128;}
	else {send_metadata_cpuu[0]=100-(((int)data[0]-48)*10+(int)data[1]-48)+128;}

        send_index = my_protocol.SetMetadata(send_buffer, send_index,
                                             send_metadata_cpuu, sizeof(send_metadata_cpuu));
        my_raw_socket.SendPacket(send_buffer, send_index);
        send_index = my_protocol.DelMetadata(send_buffer, send_index, sizeof(send_metadata_cpuu));
        time_t now = time(0);
        char* dt = ctime(&now);
 
        cout<<dt<<"send_CU="<<dec<<((int)send_metadata_cpuu[0]-128)<<endl; 
        n++;   
    }
}
