mkdir recv_1
mkdir recv_2
mkdir recv_3
mkdir recv_4

g++ recv.cpp -o ./recv_1/recv11
g++ recv.cpp -o ./recv_1/recv12
g++ recv.cpp -o ./recv_1/recv13
g++ recv.cpp -o ./recv_1/recv14
g++ recv.cpp -o ./recv_2/recv21
g++ recv.cpp -o ./recv_2/recv22
g++ recv.cpp -o ./recv_2/recv23
g++ recv.cpp -o ./recv_2/recv24
g++ recv.cpp -o ./recv_3/recv31
g++ recv.cpp -o ./recv_3/recv32
g++ recv.cpp -o ./recv_3/recv33
g++ recv.cpp -o ./recv_3/recv34
g++ recv.cpp -o ./recv_4/recv41
g++ recv.cpp -o ./recv_4/recv42
g++ recv.cpp -o ./recv_4/recv43
g++ recv.cpp -o ./recv_4/recv44

g++ h1_send_cpuu.cpp -o h1_send_cpuu
g++ h2_send_cpuu.cpp -o h2_send_cpuu
g++ h3_send_cpuu.cpp -o h3_send_cpuu
g++ h4_send_cpuu.cpp -o h4_send_cpuu

g++ send_ipv4.cpp -o send_ipv4
