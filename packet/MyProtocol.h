#ifndef MYPROTOCOL_H
#define MYPROTOCOL_H

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
    unsigned char protocol[2];  //0x801 CPUU, 0x800 IPV4
};

struct H_METADATA
{
    unsigned char cpuu[1]; 
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MyProtocol
{
public:
    int SetEthernet(unsigned char *buffer, int index,
                    const unsigned char *dst_mac, int dst_mac_length,
                    const unsigned char *src_mac, int src_mac_length);
    int SetType(unsigned char *buffer, int index,
                const unsigned char *protocol, int protocol_length);
    int SetMetadata(unsigned char *buffer, int index,
                    const unsigned char *cpuu, int cpuu_length);
    int DelMetadata(unsigned char *buffer, int index, int cpuu_length); 

    struct H_ETHERNET GetEthernet(unsigned char *buffer, int index);
    struct H_TYPE GetType(unsigned char *buffer, int index);
    struct H_METADATA GetMetadata(unsigned char *buffer, int index);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MyProtocol::SetEthernet(unsigned char *buffer, int index,
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

int MyProtocol::SetType(unsigned char *buffer, int index,
                        const unsigned char *protocol, int protocol_length)
{
    for (int i = 0; i < protocol_length; i++)
    {
        buffer[index++] = protocol[i];
    }

    return index;
}

int MyProtocol::SetMetadata(unsigned char *buffer, int index,
                    const unsigned char *cpuu, int cpuu_length)
{
    for (int i = 0; i < cpuu_length; i++)
    {
        buffer[index++] = cpuu[i];
    }

    return index;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct H_ETHERNET MyProtocol::GetEthernet(unsigned char *buffer, int index)
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

struct H_TYPE MyProtocol::GetType(unsigned char *buffer, int index)
{
    struct H_TYPE type = {};

    for (int i = 0; i < sizeof(type.protocol); i++)
    {
        type.protocol[i] = buffer[index++];
    }

    return type;
}

struct H_METADATA MyProtocol::GetMetadata(unsigned char *buffer, int index)
{
    struct H_METADATA metadata = {};

    for (int i = 0; i < sizeof(metadata.cpuu); i++)
    {
        metadata.cpuu[i] = buffer[index++];
    }

    return metadata;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MyProtocol::DelMetadata(unsigned char *buffer, int index, int cpuu_length)
{
    for (int i = 0; i < cpuu_length; i++)
    {
        buffer[index--] = 0;
    }

    return index;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
