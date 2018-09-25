# Program Specifications

You must write a client and server to support file transfer. The client should send to the server a request to initiate a transfer of a specific file. Upon recieving the request, if the file exists, it should be sent to the client. Be sure your program works with all file types, not just text files.

The client and server both must be written in C, C++, Python, or Java (note that Mininet only supports java up to version 7) using UDP datagram sockets.

- When the server starts, it should wait for a client connection (on a port number either obtained as a command line option or input by the user).
- When the client starts, it should connect to the server and send a request for a file. This request may be formatted in any way your group finds convenient.
- The server IP address, server port number, and file name may either be taken by the client program as user input, or as command line options.
- Upon recieving a request, the server should read the requested file if it exists, and send it to the client, which should write the file locally. The file transfer must meet the following requirements:
    - UDP (datagram) sockets must be used.
    - The data in each packet (not including any header information you add) must be 1024 bytes or less.
    - You must implement a sliding window of 5 packets for your file transfer. Stop-and-wait protocols will not receive full credit.      Specifically, you must not wait for all acks of a window to be recieved before moving the window and sending new data.
    - The file transfer must be reliable (recover from any packet loss, packet corruption, packet duplication, and packet reordering).
- The client and server should each print to the screen each time they send or recieve a packet.
- Do not load the entire file you will be sending into memory at once. This is not realistic for sending large files. Instead, you must only read as far as you are currently sending. So, when sending the first 5 packets, only read the first 5 packets worth of data from the file. Read more of the file when you are ready to send it. Keep all data in memory until it is acknowledged. If you are resending data, you must not re-read it from the file.
- You must account for problems occuring in any packet involved in the transfer, not just packets where you send file data. For example, what happens if your initial request gets lost? It is difficult to force a test for this condition, but testing with high percentages should cause it to happen relatively often.

# Part One
Due: September 25th, 2018

The file transfer must work, and must be implemented with a sliding window. Please see the Grading seciton below.

# Part Two
Due: October 1st, 2018

Packet corruption, loss, duplication, and reordering must be accounted for. Be sure to think about corruption, loss, duplication, and reordering of both data and acknowledgement packets. Please see the Grading section below.

Testing
To test your code, you must check if it handles loss, reordering, duplication, and corruption. However, you are unlikely to run into any of these happening at random on a small test network, and certain to not run into them at random on the simulator. Instead, we must intentionally cause the simulator to simulate these behaviors.
Simulating loss
To simulate loss, we will run commands on the virtual switch, using netem. Documentation is available at http://www.linuxfoundation.org/collaborate/workgroups/networking/netem. Specifically, to simulate loss, the command to use is tc qdisc add dev <dev> root netem loss <X.Y%>. Replace <dev> with the network interface you want to simulate loss on. Replace <X.Y%> with a percentage of packet loss to test with. 10% is reasonable for testing. If you are changing your loss percent after already adding it, you will have to replace the word add with change. Netem only affects outgoing packets, so, when testing, be sure to run any commands on both of the switch's network interfaces to simulate problems in both directions of the connection.

Simulating reordering
We will also use netem to simulate reordering. The specific command to simulate reordering is tc qdisc add dev <dev> root netem delay <Xms> reorder <Y%>. This will delay Y% of packets by X ms, causing reordering if the packets are originally sent less than Xms apart. Remember as with loss, this command works for only outgoing packets. To simulate reordering in both directions, you must run it on both switch interfaces. You may combine the loss and reordering commands to get both behaviors, with a command like tc qdisc add dev <dev> root netem delay <Xms> reorder <Y%> loss <Z%>

Simulating duplication
Duplication can also be simulated using netem. The specific command to simulate duplication is tc qdisc add dev <dev> root netem duplicate <Y%>. This will duplicate Y% of packets by X ms. Remember as with loss, this command works for only outgoing packets. To simulate reordering in both directions, you must run it on both switch interfaces. You may combine this command with the others in the obvious way to get a combination of behaviors.

Simulating corruption
To simulate packet corruption, you will need to run a custom controller that modifies the switch behavior to introduce corrpution (netem can create packet corrution, but only in a limited way not suited for this project).

pox is a modular controller included with mininet. To test packet corruption, we wil use the pox module packetcorrupt.py. This modifies standard switch behavior to cause the switch to randomly flip bits in a percentage of packets.

Copy packetcorrupt.py to /home/mininet/pox/ext in the mininet virtual machine. If you want to change the packet drop percentage, you may do so by modifying line 105 of packetcorrupt.py To run the pox openflow controller with this script, change directory to /home/mininet/pox. Then run ./pox.py packetcorrupt (you can do this from the conroller terminal after starting mininet, or from the main mininet VM terminal). To make mininet let pox control its switch behavior, add --controller=remote to the usual command line invocation of mn -x

Documentation
Documentation of your program is worth 10 points. This should be a 2-3 page document describing the design of your program. This should not be a line by line explanation of your code, but should explain the overall structure and logic of the program, as well as what major challenges you encountered and how you solved them.

Grading
There will be a total of 100 points, divided as follows:
Criteria	Points
Part 1
Transfer request	5
File transfer (on error free line) working for text files	10
File transfer (on error free line) working for other file types (pdf, jpg, etc)	10
Sliding window protocol properly implemented	15
Error checking for all inputs	5
Part 2
File transfer works with data/ack loss	10
File transfer works with ack/ack duplication	10
File transfer works with data/ack packet corruption	10
File transfer works with reordering	10
File transfer works with all above simultaneously	5
Misc
Documentation	10
