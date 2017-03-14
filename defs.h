/** @file defs.h
 *  @brief Useful definitions
 *
 *  Contains the definition of several different Macros
 *  used throughout the application in order to standardize
 *  message buffer sizes and default server ips and ports.
 *
 *  @author João Borrego
 *  @author Pedro Abreu
 *  @bug No known bugs.
 */

#ifndef DEFS
#define DEFS

/* Buffer Sizes */
#define BUFFER_SIZE 2048                    /**< Length of buffer for requests */
#define LARGE_BUFFER_SIZE 204800   			/**< Lenght of a huge buffer for large server replies */
#define PROTOCOL_SIZE 256                   /**< Length of protocol macros */
#define MESSAGE_SIZE 140                    /**< Length of messages */
#define COMMAND_SIZE 128                    /**< Length of commands from the terminal */
#define NAMEIP_SIZE 256                     /**< Length for the name of servers and their ip */
#define MAX_NUMBER_MSG 10   

#define STDIN 0 /**< `stdin` Macro for handling file descriptor in a clear way */

/* The following macro's are generally identified by the protocol they are processing */
#define SSCANF_MESSAGE_PUBLISH "%s %140[^\n]"
#define SSCANF_ID "%256[^\n]%256[^;];%256[^;];%d;%d"
#define SSCANF_SGET_MESSAGES "%256s"
#define SSCANF_SMESSAGES "%256s"
#define SSCANF_SERVERS_PARSING "%256[^;];%256[^;];%d;%d" //sv_list.c
#define SSCANF_MESSAGE_CLOCK_TABLE_INSERT "%d;%140[^;]" //msg_table.c
#define SSCANF_DEBUGMESSAGE_PUBLISH "%s %s %d %140[^\n]"

#define MSG_FORMAT_CLK "%d;%s\n"
#define MSG_FORMAT "%s\n"
#define MSG_FORMAT_CLK_SIZE 2		 			
#define MSG_FORMAT_SIZE 1

#define RMB_TIMEOUT 1	/**< UDP Socket timeout value in seconds for terminal */

#define DEFAULT_SIPT "tejo.tecnico.ulisboa.pt"  /**< Default Identity Server IP */
#define DEFAULT_SIIP 59000                      /**< Default Identity Server UDP port */
#define DEFAULT_MAX_MESSAGES 200                /**< Message Server maximum message storage capacity */
#define DEFAULT_REFRESH_RATE 10                 /**< Message Server default refresh rate */

#endif