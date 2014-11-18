/*
 * File name: network.c
 *
 * Purpose:
 *
 * Created on: Nov 3, 2014
 *     Author: ddhuy
 *
 * File history:
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "network.h"

/* =========================================================================
 * File constant & macro
 * =========================================================================
 */

#define NIPQUAD(addr)   ((unsigned char *)&addr)[0], \
                        ((unsigned char *)&addr)[1], \
                        ((unsigned char *)&addr)[2], \
                        ((unsigned char *)&addr)[3]

/* =========================================================================
 * File functions
 * =========================================================================
 */

/*
 *
 */
EndPoint* create_EP(uint16_t local_port,
				    uint16_t conn_type,
					uint16_t protocol,
					uint32_t timeout_sec,
					uint32_t timeout_usec,
					uint8_t* error_ptr
				  )
{
	EndPoint* ep_ptr = (EndPoint*) malloc(sizeof(EndPoint));
	hostent*  he_ptr;

	if ( ep_ptr != NULL )
	{
		/* set the initial value for rval */
		*error_ptr = EID_OK;

		/* initialize end-point memory to zero */
		memset((char*)ep_ptr, 0, sizeof(EndPoint));

		/* set parameters for destination end-point */
		ep_ptr->dst_sa_in.sin_addr.s_addr = -1;
		ep_ptr->dst_sa_in.sin_family      = -1;
		ep_ptr->dst_sa_in.sin_port        = -1;

		/* get host name */
		he_ptr = gethostbyname("0.0.0.0");
		if ( he_ptr == NULL )
		{
			/* delete EP before exit */
			delete_EP(ep_ptr);

			*error_ptr = EID_CREATE_HOSTENT_FAILED;
			return NULL;
		}
		/* set parameters for source end-point */
		memcpy((char*) &ep_ptr->src_sa_in.sin_addr, he_ptr->h_addr_list[0], he_ptr->h_length);
		ep_ptr->src_sa_in.sin_family      = he_ptr->h_addrtype;
		ep_ptr->src_sa_in.sin_port        = htons(local_port);

		ep_ptr->protocol                  = protocol;
		ep_ptr->conn_type                 = conn_type;
		ep_ptr->timeout.tv_sec            = timeout_sec;
		ep_ptr->timeout.tv_usec           = timeout_usec;

		/* create source socket */
		ep_ptr->socket_fd = socket(AF_INET, conn_type, protocol);
		if ( ep_ptr->socket_fd < 0 )
		{
			/* get socket failed, delete end-point and exit */
			delete_EP(ep_ptr);

			*error_ptr = EID_CREATE_SOCKET_FAILED;
			return NULL;
		}

		/* try to bind socket */
		if ( bind(ep_ptr->socket_fd, ((sockaddr*) &ep_ptr->src_sa_in), sizeof(sockaddr_in)) < 0 )
		{
			/* set option failed. close socket before delete & exit */
			delete_EP(ep_ptr);

			*error_ptr = EID_BIND_SOCKET_FAILED;
			return NULL;
		}

		/* set time out value for socket option */
		if ( setsockopt(ep_ptr->socket_fd, SOL_SOCKET, SO_RCVTIMEO, &ep_ptr->timeout, sizeof(ep_ptr->timeout)) < 0 )
		{
			/* set option failed. close socket before delete & exit */
			delete_EP(ep_ptr);

			*error_ptr = EID_SET_OPTION_FAILED;
			return NULL;
		}
	}

	return ep_ptr;
}

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
		           char* addr,
				   uint32_t port
				   )
{
	hostent*    he_ptr;
	socklen_t   sock_len;
	sockaddr_in sa_name;

	if (ep_ptr == NULL || addr == NULL)
	{
		return EID_NULL_POINTER;
	}

	/* check the input destination address */
	he_ptr = gethostbyname(addr);
	if ( he_ptr == NULL )
	{
		return EID_INVALID_ADDRESS;
	}

	/* extract the socket address information */
	memcpy((char*) &ep_ptr->dst_sa_in.sin_addr, he_ptr->h_addr_list[0], he_ptr->h_length);
	ep_ptr->dst_sa_in.sin_family  = he_ptr->h_addrtype;
	ep_ptr->dst_sa_in.sin_port    = htons(port);


	/* connect local socket_fd to destination socket */
	if ( connect(ep_ptr->socket_fd, (sockaddr*) &ep_ptr->dst_sa_in, sizeof(ep_ptr->dst_sa_in)) < 0 )
	{
		return EID_CONNECT_FAILED;
	}

	/* get the binded local ip */
	sock_len = sizeof(sockaddr_in);
	if ( getsockname(ep_ptr->socket_fd, (sockaddr*) &sa_name, &sock_len) >= 0 )
	{
		ep_ptr->local_addr = sa_name.sin_addr.s_addr;
		ep_ptr->local_port = sa_name.sin_port;
	}

	/* connect socket successfully, copy destination host name */
	strcpy(ep_ptr->dst_addr, he_ptr->h_name);

	return EID_OK;
}

/*
 *
 */
uint8_t listen_EP(EndPoint* ep_ptr, uint32_t mode)
{
	if ( ep_ptr == NULL )
	{
		return EID_NULL_POINTER;
	}

	if ( listen(ep_ptr->socket_fd, mode) < 0 )
	{
		/* delete socket before exiting*/
		delete_EP(ep_ptr);

		return EID_LISTEN_FAILED;
	}

	return EID_OK;
}

/*
 *
 */
EndPoint* accept_EP(EndPoint* server_ep_ptr)
{
	EndPoint*  new_ep_ptr = NULL;
	socklen_t  sock_len   = sizeof(sockaddr_in);

	if ( server_ep_ptr != NULL )
	{
		new_ep_ptr = (EndPoint*) malloc(sizeof(EndPoint));
		if ( new_ep_ptr != NULL )
		{
			memset((char*) new_ep_ptr, 0, sizeof(EndPoint));

			new_ep_ptr->socket_fd = accept(server_ep_ptr->socket_fd, (sockaddr*) &new_ep_ptr->dst_sa_in, &sock_len);

			if ( new_ep_ptr->socket_fd < 0 )
			{
				delete_EP(new_ep_ptr);
			}

			/* init source information of new End-Point */
			strcpy(new_ep_ptr->dst_addr, inet_ntoa(new_ep_ptr->dst_sa_in.sin_addr));

			new_ep_ptr->protocol  = server_ep_ptr->protocol;
			new_ep_ptr->conn_type = server_ep_ptr->conn_type;
			new_ep_ptr->timeout   = server_ep_ptr->timeout;
		}
	}

	return new_ep_ptr;
}

/*
 * PURPOSE : Delete an end-point
 * INPUT   : [ep] - end-point
 * OUTPUT  : None
 * RETURN  : None
 * DESCRIPT: None
 */
void delete_EP(EndPoint* ep_ptr)
{
	if ( ep_ptr != NULL)
	{
		/* close destination socket */
		if ( ep_ptr->socket_fd >= 0)
		{
			close(ep_ptr->socket_fd);
		}

		/* delete end-point */
		free(ep_ptr);
		ep_ptr = NULL;
	}
}

/*
 *
 */
uint8_t recv_message(EndPoint* ep_ptr, char* buf_ptr, int16_t* len)
{
	socklen_t sock_len = sizeof(sockaddr_in);

	/* clear the buffer */
	memset(buf_ptr, 0, MAX_RECV);

	//*len = recv(ep_ptr->socket_fd, buf_ptr, MAX_RECV, MSG_WAITALL);
	//*len = read(ep_ptr->socket_fd, buf_ptr, MAX_RECV);
	*len = recvfrom(ep_ptr->socket_fd, buf_ptr, MAX_RECV, 0, (sockaddr*) &ep_ptr->dst_sa_in, &sock_len);

	if ( *len < 0 )
	{
		return EID_RECEIVE_FAILED;
	}
	else if ( *len == 0 )
	{
		return EID_SOCKET_CLOSED;
	}
	else
	{
		/* get the address of inbound packet */
		strcpy(ep_ptr->dst_addr, inet_ntoa(ep_ptr->dst_sa_in.sin_addr));
	}

	return EID_OK;
}

/*
 *
 */
uint8_t send_message(EndPoint* ep_ptr, char* buf_ptr, uint16_t len)
{
//	socklen_t sock_len = sizeof(ep_ptr->dst_sa_in);

	if ( send(ep_ptr->socket_fd, buf_ptr, len, 0) < 0 )
	//if ( write(ep_ptr->socket_fd, buf_ptr, len) < 0)
	//if ( sendto(ep_ptr->socket_fd, buf_ptr, len, 0, (sockaddr*) &ep_ptr->dst_sa_in, sock_len) < 0 )
	{
		return EID_SEND_FAILED;
	}

	return EID_OK;
}


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
void get_mac_address(EndPoint* ep_ptr, char* mac_addr)
{
	struct ifreq ifr;
	struct sockaddr_in* sa_ptr = NULL;
	struct if_nameindex* if_list = NULL;
	struct if_nameindex* saved_list = NULL;

	if_list = saved_list = if_nameindex();

	for (; if_list->if_name != NULL; ++if_list)
	{
		strncpy(ifr.ifr_name, if_list->if_name, IF_NAMESIZE);

		if ( !ioctl(ep_ptr->socket_fd, SIOCGIFADDR, &ifr) )
		{
			sa_ptr = (sockaddr_in *) &ifr.ifr_addr;

			if ( sa_ptr->sin_addr.s_addr == ep_ptr->local_addr )
			{
				if ( !ioctl(ep_ptr->socket_fd, SIOCGIFHWADDR, &ifr) )
				{
					memcpy(mac_addr, ifr.ifr_hwaddr.sa_data, 6);
					break;
				}
			}
		}
	}

    //free the dynamic memory kernel allocated for us
	if_freenameindex(saved_list);

}

