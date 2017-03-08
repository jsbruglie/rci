#ifndef DEFS
#define DEFS

#define BUFFER_SIZE 2048  // Length of buffer for requests
#define LARGE_BUFFER_SIZE BUFFER_SIZE*100
#define PROTOCOL_SIZE 256 // Length of protocol macros
#define MESSAGE_SIZE 140  // Length of messages
#define COMMAND_SIZE 128  // Length of commands from the terminal
#define NAMEIP_SIZE 256   // Length for the name of servers and their ip

#define STDIN 0             // STDIN macro for the fd's

//The following macro's are generally identified by the protocol they are processing
#define SSCANF_MESSAGE_PUBLISH "%s %140[^\n]"
#define SSCANF_ID "%256[^\n]%256[^;];%256[^;];%d;%d"
#define SSCANF_SGET_MESSAGES "%256s"
#define SSCANF_SMESSAGES "%256s"


//Identity Server default IP and port (UDP)
#define DEFAULT_SIPT "tejo.tecnico.ulisboa.pt"
#define DEFAULT_SIIP 59000
//Message Server default refresh rate and messages
#define DEFAULT_MAX_MESSAGES 200
#define DEFAULT_REFRESH_RATE 10
#endif