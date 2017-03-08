#ifndef DEFS
#define DEFS

#define BUFFER_SIZE 2048  // Length of buffer for requests
#define LARGE_BUFFER_SIZE BUFFER_SIZE*100
#define PROTOCOL_SIZE 256 // Length of protocol macros
#define MESSAGE_SIZE 140  // Length of messages
#define COMMAND_SIZE 128  // Length of commands from the terminal
#define NAMEIP_SIZE 256   // Length for the name of servers and their ip

#define STDIN 0             // STDIN macro for the fd's

#define SSCANF_MESSAGE "%s %140[^\n]"


#endif