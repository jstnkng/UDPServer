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
