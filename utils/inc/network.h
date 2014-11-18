/*
 * network.h
 *
 *  Created on: Oct 21, 2014
 *      Author: ddhuy
 */

#ifndef NETWORK_H_
#define NETWORK_H_

/* =========================================================================
 * Headers & Include
 * =========================================================================
 */
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>

/* =========================================================================
 * File constant & macro
 * =========================================================================
 */

#define MAX_RECV 1024

typedef enum
{
	EID_OK,
	EID_SEND_FAILED,
	EID_RECEIVE_FAILED,
	EID_CREATE_SOCKET_FAILED,
	EID_SET_OPTION_FAILED,
	EID_BIND_SOCKET_FAILED,
	EID_CONNECT_FAILED,
	EID_GET_SOCK_NAME_FAILED,
	EID_INVALID_ADDRESS,
	EID_NULL_POINTER,
	EID_LISTEN_FAILED,
	EID_CREATE_HOSTENT_FAILED,
	EID_SOCKET_CLOSED,
} ErrorID;

/* =========================================================================
 * File local types and data structures
 * =========================================================================
 */
typedef struct sockaddr     sockaddr;
typedef struct hostent      hostent;
typedef struct timeval      timeval;
typedef struct in_addr      in_addr;
typedef struct ifaddrs      ifaddrs;
typedef struct sockaddr_in  sockaddr_in;
typedef struct sockaddr_in6 sockaddr_in6;
typedef struct msghdr       msghdr;
typedef struct iovec        iovec;
/*
 * struct EndPoint
 */
typedef struct
{
	char        dst_addr[48];
	sockaddr_in dst_sa_in;

	uint32_t    socket_fd;
	sockaddr_in src_sa_in;

	uint32_t    local_addr;
	uint16_t    local_port;

	uint16_t    conn_type;
	uint16_t    protocol;
	timeval     timeout;

} EndPoint;

/* =========================================================================
 * Function prototypes
 * =========================================================================
 */

/*
 * PURPOSE : Create new end-point
 * INPUT   :
 * OUTPUT  : None
 * RETURN  : New end-point
 * DESCRIPT: None
 */
EndPoint* create_EP(uint16_t local_port,
				    uint16_t conn_type,
					uint16_t protocol,
					uint32_t timeout_sec,
					uint32_t timeout_usec,
					uint8_t* error_ptr
				  );

/*
 * PURPOSE : Connect end-point to destination
 * INPUT   : [ep]        - end-point
 *           [dest_ip]   - Destination IP
 *           [dest_port] - Destination port
 * OUTPUT  : None
 * RETURN  : 0 if success
 * DESCRIPT: None
 */
uint8_t connect_EP(EndPoint* ep_ptr,
		           char* dst_addr,
				   uint32_t dst_port
				   );

/*
 * PURPOSE : listening for a connection
 * INPUT   : [ep] - end-point
 * OUTPUT  : None
 * RETURN  : None
 * DESCRIPT: None
 */
uint8_t listen_EP(EndPoint* server_ep_ptr, uint32_t mode);

/*
 * PURPOSE : accept a connection
 * INPUT   : [ep] - end-point
 * OUTPUT  : None
 * RETURN  : None
 * DESCRIPT: None
 */
EndPoint* accept_EP(EndPoint* server_ep_ptr);

/*
 * PURPOSE : Delete an end-point
 * INPUT   : [ep] - end-point
 * OUTPUT  : None
 * RETURN  : None
 * DESCRIPT: None
 */
void delete_EP(EndPoint* ep_ptr);

/*
 *
 */
uint8_t recv_message(EndPoint* ep_ptr, char* msg_ptr, int16_t* msg_len);

/*
 *
 */
uint8_t send_message(EndPoint* dst_ep_ptr, char* msg_ptr, uint16_t msg_len);

/*
 * Purpose:
 * - Get MAC address of the current interface
 *
 * Input:
 * - [in]  ep_ptr: the socket which used to get MAC
 * - [out] mac_addr: the mac address.
 *
 * Output:
 * - void
 *
 */
void get_mac_address(EndPoint* ep_ptr, char* mac_addr);

#endif /* NETWORK_H_ */
