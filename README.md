# FIFO-Server-Client
## C implementation of a FIFO Server and Client Connection. 
### A one to many node connection including message queues and a custom designed system request structure for message communication. 

Note: Requires GCC to run application
# Terminal Command instructions below:

## Server Terminal
1. gcc DHashem-FIFO-Server.c -o fifo-server
2. ./fifo-server
Server Terminal will stall while waiting for client connections
Server will output status updates and not except direct input

## Client Terminal
[Repeat steps in n terminals for n clients]
1. gcc DHashem-FIFO-Client.c -o fifo-client-n
2. ./fifo-client-n

Repeat client setup for all client terminals before following terminal prompt
Note: System messages are of type Char and must not contain any spaces
