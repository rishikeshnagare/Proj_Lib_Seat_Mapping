# Proj_Lib_Seat_Mapping

This project is a command line based reservation / mapping system. Please follow the instructions to compile and run the project.
The whole process afterwards is self explainatory.

== Compile ==

    make clean
    make all

== Run Server ==

    ./server <port>

    For example:

    ./server 8207

    The server accepts connections from both its IPv4 and IPv6 addresses.

== Run IPv4 Client ==

    ./client4 <server_ip> <server_port>

    For examples:

    ./client4 127.0.0.1 8207
    
    If the server IPv4 address is 192.168.0.1, then run:

    ./client4 192.168.0.1 8207

== Run IPv6 Client ==

    ./client6 <server_ip> <server_port>

    For examples:

    ./client6 ::1 8207
    
    If the server IPv6 (link-local) address is fe80::20c:29ff:fefc:3f4e,
    then run:

    sudo ./client6 fe80::20c:29ff:fefc:3f4e 8207

    The IPv6 client can also take IPv4 server addresses, e.g.:

    ./client6 127.0.0.1 8207

    If the server IPv4 address is 192.168.0.1, then run:

    ./client6 192.168.0.1 8207

