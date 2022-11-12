# P4RSS

Off-the-shelf x86 servers are widely deployed as middleboxes in edge and public clouds, e.g., cloud gateways and load balancers. They follow the ``run-to-completion'' model and achieve parallel traffic processing with packet flows distributed onto multiple CPU cores based on the RSS capability of NICs. However, RSS may cause inter-core load imbalance as it conducts stateless hashing without considering the CPU core utilization and multiple heavy-hitter flows will potentially overload a single CPU core when they are hashed onto that core. In this work, we propose P4RSS, a load-aware intra-server load balancing solution with the aid of the P4 data plane. Specifically, a P4 ASIC is placed in front of the CPU for stateful traffic load balancing among multiple CPU cores based on the real-time acquisition of the core utilization. Furthermore, flow affinity maintenance and heavy hitter throttling are also offloaded to the P4 ASIC to maximally free up valuable CPU computing resource. P4RSS can be deployed in the form of either hyper-converged server switches or P4-based SmartNICs. Evaluation shows that P4RSS reduces the variance of CPU core utilization by 38.2% compared with RSS, which not only makes the middleboxes more stable but also enables higher CPU utilization without overprovisioning.

The major contributions are summarized as follows:

1.We propose an intra-server load balancing framework based on precise CPU core utilization feedback. Specifically, we design a load-aware protocol to advertise the real-time CPU load distribution to the P4 ASIC, and conduct P4-based traffic scheduling to hash packet flows to the least-loaded CPU core while ensuring flow affinity.

2.We propose a lightweight heavy hitter detection method based on the CPU load distribution feedback and an adaptive heavy hitter throttling algorithm based on token buckets. The on-demand heavy hitter throttling ensures CPU core overload prevention in the worst case.
    
3.We build a P4RSS demo with 435 lines of P4 and 360 lines of C++ to verify the feasibility of our design. Evaluation shows that P4RSS reduces the variance of CPU core utilization by 38.2% compared with RSS and prevents CPU from being overloaded by heavy hitters even in extreme cases.


# Framework demo
The demo includes three modules: `p4_code`, `packet`, `topology`.

## p4_code
The P4 file which defines the packet processing logic of the switches, mainly includes load balancing and traffic rate limiting. You can build .p4 into .json through the run.sh file in the terminal.

## packet
Construct a special protocol header through rawsocket to send the cpu utilization to the ASIC. We also provide code samples for sending and receiving ipv4 packets. You can generate load-aware packet sender and ipv4 receiver via run.sh, the sender and receiver will be automatically bound to the core when the topology is generated.

## topology
We use mininet to build a virtual network test platform. We set up five hosts and a switch, isolating a CPU for each host and assigning it a switch port. After starting the topology, host 5 sends packets, and bmv2 distributes traffic to hosts 1 to 4.

## How to run
First, enter the root direction of the system.
```
cd P4RSS
```
Generate P4 switch configuration file.
```
cd p4_code
sh run.sh
```
Generate send and receive package file.
```
cd packet
sh run.sh
```
Start the topology and enter the CLI of Mininet.
```
sudo python topo.py
```
Start the host terminal.
```
xterm [host]
```
Start sending load-aware packets on the cpu side
```
cd packet && ./h1_send_cpuu
```
Start sending ipv4 packets
```
cd packet && ./send_ipv4
```
Then you can see the real-time utilization of each core and the receipt of packets on each core.
