#ifndef MYPROTOCOL_IPV4_H
#define MYPROTOCOL_IPV4_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Header define

struct H_ETHERNET
{
    unsigned char dst_mac[6];
    unsigned char src_mac[6];
};

struct H_TYPE
{
    unsigned char protocol[2];  //0x800 ipv4
};

struct H_IP
{
    unsigned char version_ihl[1];
    unsigned char diffserv[1];
    unsigned char totalLen[2];
    unsigned char identification[2];
    unsigned char flags_offset[2];
    unsigned char ttl[1];
    unsigned char protocol_t[1];
    unsigned char hdrChecksum[2];
    unsigned char src_addr[4];
    unsigned char dst_addr[4];
};

struct H_TCP
{
    unsigned char src_port[2];
    unsigned char dst_port[2];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MyProtocol_ipv4
{
public:
    int SetEthernet(unsigned char *buffer, int index,
                    const unsigned char *dst_mac, int dst_mac_length,
                    const unsigned char *src_mac, int src_mac_length);
    int SetType(unsigned char *buffer, int index,
                const unsigned char *protocol, int protocol_length);
    int SetIp(unsigned char *buffer, int index,
              const unsigned char *version_ihl, int version_ihl_length,
              const unsigned char *diffserv, int diffserv_length,
              const unsigned char *totalLen, int totalLen_length,
              const unsigned char *identification, int identification_length,
              const unsigned char *flags_offset, int flags_offset_length,
              const unsigned char *ttl, int ttl_length,
              const unsigned char *protocol_t, int protocol_t_length,
              const unsigned char *hdrChecksum, int hdrChecksum_length,
              const unsigned char *src_addr, int src_addr_length,
              const unsigned char *dst_addr, int dst_addr_length);
    int SetTcp(unsigned char *buffer, int index,
               const unsigned char *src_port, int src_port_length,
               const unsigned char *dst_port, int dst_port_length);
    int DelTCP(unsigned char *buffer, int index,
               int src_port_length, int dst_port_length);

    struct H_ETHERNET GetEthernet(unsigned char *buffer, int index);
    struct H_TYPE GetType(unsigned char *buffer, int index);
    struct H_IP GetIp(unsigned char *buffer, int index);
    struct H_TCP GetTcp(unsigned char *buffer, int index);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MyProtocol_ipv4::SetEthernet(unsigned char *buffer, int index,
                                 const unsigned char *dst_mac, int dst_mac_length,
                                 const unsigned char *src_mac, int src_mac_length)
{
    for (int i = 0; i < dst_mac_length; i++)
    {
        buffer[index++] = dst_mac[i];
    }

    for (int i = 0; i < src_mac_length; i++)
    {
        buffer[index++] = src_mac[i];
    }

    return index;
}

int MyProtocol_ipv4::SetType(unsigned char *buffer, int index,
                             const unsigned char *protocol, int protocol_length)
{
    for (int i = 0; i < protocol_length; i++)
    {
        buffer[index++] = protocol[i];
    }

    return index;
}

int MyProtocol_ipv4::SetIp(unsigned char *buffer, int index,
                      const unsigned char *version_ihl, int version_ihl_length,
                      const unsigned char *diffserv, int diffserv_length,
                      const unsigned char *totalLen, int totalLen_length,
                      const unsigned char *identification, int identification_length,
                      const unsigned char *flags_offset, int flags_offset_length,
                      const unsigned char *ttl, int ttl_length,
                      const unsigned char *protocol_t, int protocol_t_length,
                      const unsigned char *hdrChecksum, int hdrChecksum_length,
                      const unsigned char *src_addr, int src_addr_length,
                      const unsigned char *dst_addr, int dst_addr_length)
{
    for (int i = 0; i < version_ihl_length; i++)
    {
        buffer[index++] = version_ihl[i];
    }

    for (int i = 0; i < diffserv_length; i++)
    {
        buffer[index++] = diffserv[i];
    }

    for (int i = 0; i < totalLen_length; i++)
    {
        buffer[index++] = totalLen[i];
    }

    for (int i = 0; i < identification_length; i++)
    {
        buffer[index++] = identification[i];
    }

    for (int i = 0; i < flags_offset_length; i++)
    {
        buffer[index++] = flags_offset[i];
    }

    for (int i = 0; i < ttl_length; i++)
    {
        buffer[index++] = ttl[i];
    }

    for (int i = 0; i < protocol_t_length; i++)
    {
        buffer[index++] = protocol_t[i];
    }

    for (int i = 0; i < hdrChecksum_length; i++)
    {
        buffer[index++] = hdrChecksum[i];
    }

    for (int i = 0; i < src_addr_length; i++)
    {
        buffer[index++] = src_addr[i];
    }

    for (int i = 0; i < dst_addr_length; i++)
    {
        buffer[index++] = dst_addr[i];
    }

    return index;
}

int MyProtocol_ipv4::SetTcp(unsigned char *buffer, int index,
               const unsigned char *src_port, int src_port_length,
               const unsigned char *dst_port, int dst_port_length)
{
    for (int i = 0; i < src_port_length; i++)
    {
        buffer[index++] = src_port[i];
    }

    for (int i = 0; i < dst_port_length; i++)
    {
        buffer[index++] = dst_port[i];
    }

    return index;
}

int MyProtocol_ipv4::DelTCP(unsigned char *buffer, int index,
                            int src_port_length,
                            int dst_port_length)
{
    for (int i = 0; i < src_port_length; i++)
    {
        buffer[index--] = 0;
    }
    
    for (int i = 0; i < dst_port_length; i++)
    {
        buffer[index--] = 0;
    }

    return index;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct H_ETHERNET MyProtocol_ipv4::GetEthernet(unsigned char *buffer, int index)
{
    struct H_ETHERNET ethernet = {};

    for (int i = 0; i < sizeof(ethernet.dst_mac); i++)
    {
        ethernet.dst_mac[i] = buffer[index++];
    }

    for (int i = 0; i < sizeof(ethernet.src_mac); i++)
    {
        ethernet.src_mac[i] = buffer[index++];
    }

    return ethernet;
}

struct H_TYPE MyProtocol_ipv4::GetType(unsigned char *buffer, int index)
{
    struct H_TYPE type = {};

    for (int i = 0; i < sizeof(type.protocol); i++)
    {
        type.protocol[i] = buffer[index++];
    }

    return type;
}
struct H_IP MyProtocol_ipv4::GetIp(unsigned char *buffer, int index)
{
    struct H_IP ip = {};

    for (int i = 0; i < sizeof(ip.version_ihl); i++)
    {
        ip.version_ihl[i] = buffer[index++];
    }

    for (int i = 0; i < sizeof(ip.diffserv); i++)
    {
        ip.diffserv[i] = buffer[index++];
    }

    for (int i = 0; i < sizeof(ip.totalLen); i++)
    {
        ip.totalLen[i] = buffer[index++];
    }

    for (int i = 0; i < sizeof(ip.identification); i++)
    {
        ip.identification[i] = buffer[index++];
    }

    for (int i = 0; i < sizeof(ip.flags_offset); i++)
    {
        ip.flags_offset[i] = buffer[index++];
    }

    for (int i = 0; i < sizeof(ip.ttl); i++)
    {
        ip.ttl[i] = buffer[index++];
    }

    for (int i = 0; i < sizeof(ip.protocol_t); i++)
    {
        ip.protocol_t[i] = buffer[index++];
    }

    for (int i = 0; i < sizeof(ip.hdrChecksum); i++)
    {
        ip.hdrChecksum[i] = buffer[index++];
    }

    for (int i = 0; i < sizeof(ip.src_addr); i++)
    {
        ip.src_addr[i] = buffer[index++];
    }

    for (int i = 0; i < sizeof(ip.dst_addr); i++)
    {
        ip.dst_addr[i] = buffer[index++];
    }

    return ip;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
