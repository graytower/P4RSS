/* -*- P4_16 -*- */
#include <core.p4>
#include <v1model.p4>

/*************************************************************************
*********************** H E A D E R S  ***********************************
*************************************************************************/

header ethernet_t {
    bit<48>   dstMac;
    bit<48>   srcMac;
}

header type_t {
    bit<16>   protocol;    // 0x801:cpuu; 0x800:ipv4
}     

header cpuu_t {  
    bit<1>    isAvailable;
    bit<7>    utilization;
}

header ipv4_t {
    bit<4>    version;
    bit<4>    ihl;
    bit<8>    diffserv;
    bit<16>   totalLen;
    bit<16>   identification;
    bit<3>    flags;
    bit<13>   fragOffset;
    bit<8>    ttl;
    bit<8>    protocol;
    bit<16>   hdrChecksum;
    bit<32>   srcAddr;
    bit<32>   dstAddr;
}

header tcp_t {
    bit<16>   srcPort;
    bit<16>   dstPort;
}

struct headers {
    ethernet_t      ethernet; 
    type_t          type;
    cpuu_t          cpuu;
    ipv4_t          ipv4;
    tcp_t           tcp;
}

struct metadata {
    bit<16>   hash_index;
    bit<9>    read_cpuu_port;
    bit<7>    read_cpuu_utilization;
    bit<9>    read_fc_port;
    bit<48>   read_fc_time;
    bit<2>    read_islimited;
    bit<32>   read_packet_counter;
    bit<32>   read_max_counter_flow_hash;
    bit<32>   read_max_counter;
    bit<32>   read_token;
    bit<32>   read_last_max_flow;
    bit<8>    read_deny_limit;
    bit<48>   read_record_time;
    bit<32>   read_record_etime;
}

# define CPU_NUMBER 4
# define AGE_TIME 5000000            //5s
# define TOKEN 12500                 //12500 bytes/sec (~100kbps)
# define LIMIT_THRESHOLD 80   
# define DENY_LIMIT 5 
# define CPUU_INDEX 1
# define REGISTER_CPUU_SIZE 2
# define REGISTER_FC_SIZE 1025
# define REGISTER_MaxF_SIZE 5

register<bit<9>>(REGISTER_CPUU_SIZE) CPUU_Port;
register<bit<7>>(REGISTER_CPUU_SIZE) CPUU_Utilization;

register<bit<9>>(REGISTER_FC_SIZE)  FC_Port;
register<bit<48>>(REGISTER_FC_SIZE) FC_Time;
register<bit<2>>(REGISTER_FC_SIZE)  IsLimited;
register<bit<32>>(REGISTER_FC_SIZE) Packet_Counter;

register<bit<32>>(REGISTER_MaxF_SIZE) Max_Counter_Flow_Hash;
register<bit<32>>(REGISTER_MaxF_SIZE) Max_Counter;
register<bit<32>>(REGISTER_MaxF_SIZE) Token;
register<bit<32>>(REGISTER_MaxF_SIZE) Last_Max_Flow;
register<bit<8>>(REGISTER_MaxF_SIZE)  Deny_Limit; //when Deny_limit = 0, can limit another flow.

register<bit<48>>(2) Record_Time;
register<bit<32>>(2) Record_etime;

/*************************************************************************
*********************** P A R S E R  ***********************************
*************************************************************************/

parser MyParser(packet_in packet,
                out headers hdr,
                inout metadata meta,
                inout standard_metadata_t standard_metadata) {

    state start {
        transition parse_ethernet;
    }

    state parse_ethernet {
        packet.extract(hdr.ethernet);
        transition parse_type;
    }

    state parse_type {
        packet.extract(hdr.type);
        transition select(hdr.type.protocol) {
            0x800: parse_ipv4;
            0x801: parse_cpuu;
            default: accept;
        }
    }

    state parse_ipv4 {
        packet.extract(hdr.ipv4);
        transition select(hdr.ipv4.protocol) {
            6: parse_tcp;
            default: accept;
        }
    }
    
    state parse_tcp {
        packet.extract(hdr.tcp);
        transition accept;
    }

    state parse_cpuu {
        packet.extract(hdr.cpuu);
        transition accept;
    }
}

/*************************************************************************
************   C H E C K S U M    V E R I F I C A T I O N   *************
*************************************************************************/

control MyVerifyChecksum(inout headers hdr, inout metadata meta) {   
    apply {  }
}

/*************************************************************************
**************  I N G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyIngress(inout headers hdr,
                  inout metadata meta,
                  inout standard_metadata_t standard_metadata) {
    action drop() {
        mark_to_drop(standard_metadata);
    }
    
    action do_updateToken(){
        if(hdr.cpuu.utilization<LIMIT_THRESHOLD - 10){
            meta.read_token = TOKEN + (LIMIT_THRESHOLD - 10 - (bit<32>)hdr.cpuu.utilization)*1000;
        }
        else if(hdr.cpuu.utilization>LIMIT_THRESHOLD + 10){
            meta.read_token = TOKEN - ((bit<32>)hdr.cpuu.utilization - LIMIT_THRESHOLD - 10)*1000;
        }
        else{
            meta.read_token = TOKEN;
        }
        //write
        Token.write((bit<32>)standard_metadata.ingress_port, meta.read_token);
    }
    
    action do_updateCPUU(){ 
        //read
        CPUU_Port.read(meta.read_cpuu_port, CPUU_INDEX);
        CPUU_Utilization.read(meta.read_cpuu_utilization, CPUU_INDEX);
        Deny_Limit.read(meta.read_deny_limit, (bit<32>)standard_metadata.ingress_port);

        //update min CPUU
        if((meta.read_cpuu_port == 0) || (meta.read_cpuu_port == standard_metadata.ingress_port) || (hdr.cpuu.utilization < meta.read_cpuu_utilization)){
	    meta.read_cpuu_port = standard_metadata.ingress_port;
            meta.read_cpuu_utilization = hdr.cpuu.utilization;
        }

        //write
        CPUU_Port.write(CPUU_INDEX, meta.read_cpuu_port);
	CPUU_Utilization.write(CPUU_INDEX, meta.read_cpuu_utilization);
    }
    
    action do_check_limit_valid(){

        //read
        Deny_Limit.read(meta.read_deny_limit, (bit<32>)standard_metadata.ingress_port);
        Last_Max_Flow.read(meta.read_last_max_flow, (bit<32>)standard_metadata.ingress_port);
        IsLimited.read(meta.read_islimited, meta.read_last_max_flow);

        //check if limit is valid
        if(meta.read_deny_limit > 0){
            meta.read_deny_limit = meta.read_deny_limit - 1;
            if(meta.read_deny_limit > 1){
                if(hdr.cpuu.utilization < LIMIT_THRESHOLD){  //limit is valid
                    meta.read_last_max_flow = 0;
                    meta.read_deny_limit = 0;
                }
            }
            else{                                            //limit is not valid
                meta.read_islimited = 2;
            }
        }

        //write
        Deny_Limit.write((bit<32>)standard_metadata.ingress_port, meta.read_deny_limit);
        IsLimited.write(meta.read_last_max_flow, meta.read_islimited);
        Last_Max_Flow.write((bit<32>)standard_metadata.ingress_port, meta.read_last_max_flow);
    }

    action do_reset(){

        //read
        CPUU_Port.read(meta.read_cpuu_port, CPUU_INDEX);
        CPUU_Utilization.read(meta.read_cpuu_utilization, CPUU_INDEX);

        //reset
        if(standard_metadata.ingress_port == meta.read_cpuu_port){
            meta.read_cpuu_port = 0;
            meta.read_cpuu_utilization = 0;
        }

        //write
        CPUU_Port.write(CPUU_INDEX, meta.read_cpuu_port);
        CPUU_Utilization.write(CPUU_INDEX, meta.read_cpuu_utilization);
    }

    action do_mark_limit(){

        //read
        Max_Counter_Flow_Hash.read(meta.read_max_counter_flow_hash, (bit<32>)standard_metadata.ingress_port);
        Max_Counter.read(meta.read_max_counter, (bit<32>)standard_metadata.ingress_port);
        Last_Max_Flow.read(meta.read_last_max_flow, (bit<32>)standard_metadata.ingress_port);
        Deny_Limit.read(meta.read_deny_limit, (bit<32>)standard_metadata.ingress_port);
        FC_Time.read(meta.read_fc_time, (bit<32>)meta.read_max_counter_flow_hash);

        //mark
        if(meta.read_deny_limit == 0 && meta.read_max_counter_flow_hash > 0){
            if(standard_metadata.ingress_global_timestamp - meta.read_fc_time < AGE_TIME){
                meta.read_islimited = 1;
                meta.read_deny_limit = DENY_LIMIT;
                meta.read_last_max_flow = meta.read_max_counter_flow_hash;
            }
            else{
                meta.read_islimited = 0;
            }
            meta.read_max_counter_flow_hash = 0;
            meta.read_max_counter = 0;
        }
        
        //write
        IsLimited.write(meta.read_last_max_flow, meta.read_islimited);
        Max_Counter_Flow_Hash.write((bit<32>)standard_metadata.ingress_port, meta.read_max_counter_flow_hash);
        Max_Counter.write((bit<32>)standard_metadata.ingress_port, meta.read_max_counter);
        Last_Max_Flow.write((bit<32>)standard_metadata.ingress_port, meta.read_last_max_flow);
        Deny_Limit.write((bit<32>)standard_metadata.ingress_port, meta.read_deny_limit);
    }

    action do_forward(){

        //crc16 hash
        hash(meta.hash_index,
	    HashAlgorithm.crc16,
            32w0,
	    { hdr.ipv4.srcAddr,
	      hdr.ipv4.dstAddr,
              hdr.ipv4.protocol,
              hdr.tcp.srcPort,
              hdr.tcp.dstPort},
            32w0x400);

        //read
        FC_Time.read(meta.read_fc_time, (bit<32>)meta.hash_index);
        FC_Port.read(meta.read_fc_port, (bit<32>)meta.hash_index);
        Packet_Counter.read(meta.read_packet_counter, (bit<32>)meta.hash_index);
        IsLimited.read(meta.read_islimited, (bit<32>)meta.hash_index);
        CPUU_Port.read(meta.read_cpuu_port, 1);

        //set egress port
        if(meta.read_fc_port < 1 || standard_metadata.ingress_global_timestamp - meta.read_fc_time > AGE_TIME){
            if(meta.read_cpuu_port < 1){
                 meta.read_fc_port = (bit<9>)meta.hash_index % CPU_NUMBER + 1;
                 hdr.type.protocol = 769;  //mark "hash"
            }
            else{
                 meta.read_fc_port = meta.read_cpuu_port;
                 hdr.type.protocol = 770;  //mark "min-load forward"
            }
            meta.read_packet_counter = 0;
            meta.read_islimited = 0;
        }
        else{
            hdr.type.protocol = 771;   //mark "hit entry"
        }
        standard_metadata.egress_spec = meta.read_fc_port;

        //write
        FC_Port.write((bit<32>)meta.hash_index, standard_metadata.egress_spec);
        FC_Time.write((bit<32>)meta.hash_index, standard_metadata.ingress_global_timestamp);
        Packet_Counter.write((bit<32>)meta.hash_index, meta.read_packet_counter);
        IsLimited.write((bit<32>)meta.hash_index, meta.read_islimited);
    }
    
    action do_limit(){
        
        //read
        IsLimited.read(meta.read_islimited, (bit<32>)meta.hash_index);
        Packet_Counter.read(meta.read_packet_counter, (bit<32>)meta.hash_index);
        Token.read(meta.read_token, (bit<32>)standard_metadata.egress_spec);

        //limit
        meta.read_packet_counter = meta.read_packet_counter + 1;
        
        if(meta.read_islimited == 1){
            if(standard_metadata.packet_length < meta.read_token){
                meta.read_token = meta.read_token - standard_metadata.packet_length;
            }
            else{
            meta.read_packet_counter = meta.read_packet_counter - 1;
            meta.read_fc_port = 0;
            }
        }
        //write
        Token.write((bit<32>)standard_metadata.egress_spec, meta.read_token);
        Packet_Counter.write((bit<32>)meta.hash_index, meta.read_packet_counter);
    }

    action do_updateMax(){

        //read
        Packet_Counter.read(meta.read_packet_counter, (bit<32>)meta.hash_index);
        Max_Counter.read(meta.read_max_counter, (bit<32>)standard_metadata.egress_spec);
        Max_Counter_Flow_Hash.read(meta.read_max_counter_flow_hash, (bit<32>)standard_metadata.egress_spec);
        IsLimited.read(meta.read_islimited, (bit<32>)meta.hash_index);

        //update
        if(meta.read_islimited == 0){  
            if(meta.read_packet_counter > meta.read_max_counter){
                meta.read_max_counter_flow_hash = (bit<32>)meta.hash_index;
                meta.read_max_counter = meta.read_packet_counter;
            }
        }

        //write
        Max_Counter_Flow_Hash.write((bit<32>)standard_metadata.egress_spec, meta.read_max_counter_flow_hash);
        Max_Counter.write((bit<32>)standard_metadata.egress_spec, meta.read_max_counter);
    }

    apply {
        if(hdr.cpuu.isValid()) {
            do_updateToken();
            if((bool)hdr.cpuu.isAvailable){
                do_updateCPUU();
                do_check_limit_valid();
                if(hdr.cpuu.utilization > LIMIT_THRESHOLD){
                    do_mark_limit();
                }
            }
            else{
                do_reset();
            }
        }
        else if(hdr.ipv4.isValid()) {
            if(hdr.tcp.isValid()){
                do_forward();
                do_limit();
                do_updateMax();
            }
        }
    }
}

/*************************************************************************
****************  E G R E S S   P R O C E S S I N G   *******************
*************************************************************************/

control MyEgress(inout headers hdr,
                 inout metadata meta,
                 inout standard_metadata_t standard_metadata) {  
    action drop() {
        mark_to_drop(standard_metadata);
    }  

    apply {
        Record_Time.write(0, standard_metadata.ingress_global_timestamp);
        Record_Time.read(meta.read_record_time,0);
        Record_etime.write(0, standard_metadata.enq_timestamp);
        Record_etime.read(meta.read_record_etime,0);
        Record_Time.write(1, standard_metadata.egress_global_timestamp);
        Record_Time.read(meta.read_record_time,1);
        
        if (hdr.cpuu.isValid() || meta.read_fc_port == 0) {
            drop();
        }
    }
}

/*************************************************************************
*************   C H E C K S U M    C O M P U T A T I O N   **************
*************************************************************************/

control MyComputeChecksum(inout headers hdr, inout metadata meta) {
    apply {}
}

/*************************************************************************
***********************  D E P A R S E R  *******************************
*************************************************************************/

control MyDeparser(packet_out packet, in headers hdr) {
    apply {
        packet.emit(hdr.ethernet);
        packet.emit(hdr.type);
        packet.emit(hdr.ipv4);
        packet.emit(hdr.tcp);
    }
}

/*************************************************************************
***********************  S W I T C H  *******************************
*************************************************************************/

V1Switch(
MyParser(),
MyVerifyChecksum(),
MyIngress(),
MyEgress(),
MyComputeChecksum(),
MyDeparser()
) main;
