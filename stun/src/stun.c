/*
 * File name: stun.c
 *
 * Purpose:
 *
 * Created on: Nov 3, 2014
 *     Author: ddhuy
 *
 * File history:
 */

#include <network.h>
#include <stdio.h>
#include <string.h>

#include "stun.h"
#include "stun_func.h"

/***************************************************************
 *
 * File local macro
 *
 ***************************************************************/
#define TEST_LOOP (5)

#define TIMEOUT_SEC  0
#define TIMEOUT_USEC 400000

#define TRUE  (1)
#define FALSE (0)

/***************************************************************
 *
 * File local variables
 *
 ***************************************************************/
Stun_Att_Other_Address      other_addr;
Stun_Att_XOR_Mapped_Address xor_mapped_addr;

uint32_t  g_local_addr;
uint16_t  g_local_port;

uint32_t g_public_addr;
uint16_t g_public_port;

const char g_filtering_name_tbl[NAT_FILTERING_COUNT + 1][32] =
{
	"FILTERING NONE",
	"ENDPOINT INDEPENDENT",
	"ADDRESS DEPENDENT",
	"ADDRESS & PORT DEPENDENT",
	"UNKNOWN"
};

const char g_mapping_name_tbl[NAT_MAPPING_COUNT + 1][32] =
{
	"MAPPING NONE",
	"ENDPOINT INDEPENDENT",
	"ADDRESS DEPENDENT",
	"ADDRESS & PORT DEPENDENT",
	"UNKNOWN",
};

const char g_nat_type_name_tbl[NAT_TYPE_COUNT + 1][32] =
{
	"NAT_TYPE_NONE",
	"NAT_TYPE_FULL_CONE",
	"NAT_TYPE_RESTRICTED_CONE",
	"NAT_TYPE_PORT_RESTRICTED_CONE",
	"NAT_TYPE_4",
	"NAT_TYPE_5",
	"NAT_TYPE_6",
	"NAT_TYPE_7",
	"NAT_TYPE_8",
	"NAT_TYPE_SYMMETRIC",
	"NAT_TYPE_UDP_BLOCKED",
	"NAT_TYPE_SYMMETRIC_FIREWALL",
	"UNKNOWN",
};

/***************************************************************
 *
 * NAT Type table (Mapping between RFC-3489 & RFC5780)
 *
 ***************************************************************/
char g_nat_type_tbl[NAT_FILTERING_COUNT][NAT_MAPPING_COUNT] =
{
				/*		MNone				MEI								MAD					MAPD 			*/
/*    FNONE    */	{ NAT_TYPE_NONE, 	NAT_TYPE_UNKNOWN, 			    NAT_TYPE_UNKNOWN,	NAT_TYPE_UNKNOWN, 	 },
/*    FEI      */	{ NAT_TYPE_UNKNOWN,	NAT_TYPE_FULL_CONE,				NAT_TYPE_4,			NAT_TYPE_7,          },
/*    FAD      */	{ NAT_TYPE_UNKNOWN,	NAT_TYPE_RESTRICTED_CONE,		NAT_TYPE_5,			NAT_TYPE_8,          },
/*    FAPD     */	{ NAT_TYPE_UNKNOWN,	NAT_TYPE_PORT_RESTRICTED_CONE,	NAT_TYPE_6,			NAT_TYPE_SYMMETRIC,  },
};

/* =========================================================================
 * File local function
 * =========================================================================
 */

/*
 *
 */
static int8_t encode_send_wait(EndPoint* ep_ptr, Stun_Message* msg_ptr, char* data_ptr)
{
	uint8_t   i = 0;
	int16_t len = 0;
	char    snd_buf[MAX_RECV] = { 0 };

	/* encode message before sending */
	stun_encode_message(msg_ptr, snd_buf);

	/* try to send and get at least TEST_LOOP times for ensuring message is sent successfully */
	for (i = 0; i < TEST_LOOP; ++i)
	{
		/* send message to host */
		if ( send_message(ep_ptr, snd_buf, msg_ptr->header.msg_len + STUN_HEADER_SIZE) == EID_OK )
		{
			/* waiting for response */
			if ( recv_message(ep_ptr, data_ptr, &len) == EID_OK )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

/***************************************************************
 *
 * NAT/STUN functions
 *
 ***************************************************************/

/*
 * Purpose:
 * - Get local port mapping.
 *
 * Precondition:
 * - Run detecting function before calling this
 *
 * Input:
 *
 * Output:
 * - return local port
 */
uint16_t stun_get_local_port()
{
	return g_local_port;
}

/*
 * Purpose:
 * - Get local address.
 *
 * Precondition:
 * - Run detecting function before calling this
 *
 * Input:
 *
 * Output:
 * - return local address
 */
char* stun_get_local_address()
{
	return inet_ntoa( *((in_addr*) &g_local_addr) );
}

/*
 * Purpose:
 * - Get public port mapping.
 *
 * Precondition:
 * - Run detecting function before calling this
 *
 * Input:
 *
 * Output:
 * - return public port
 */
uint16_t stun_get_public_port()
{
	return g_public_port;
}

/*
 * Purpose:
 * - Get public address.
 *
 * Precondition:
 * - Run detecting function before calling this
 *
 * Input:
 *
 * Output:
 * - return public address
 */
char* stun_get_public_address()
{
	return inet_ntoa( *((in_addr*) &g_public_addr) );
}

/*
 * Purpose:
 * - Create sockets and then connect to open STUN server for detecting NAT mapping behavior
 *
 * Input:
 * - [in] local_port: the local port that client want to open for incoming/outgoing stream
 * - [in] host_addr: address of open stun server
 * - [in] host_port: port of open stun server
 *
 * Output:
 * - If detect successfully return true
 * - Else return false
 */
NAT_Filtering_Behavior stun_detect_nat_filtering
(
	uint16_t local_port,   /* [in] the local port that client want to open for incoming/outgoing stream */
	char*    host_addr,    /* [in] address of open stun server */
	uint16_t host_port    /* [in] port of open stun server */
)
{
	char                     data_ptr[MAX_RECV];
	uint8_t                  rval;
	Stun_Message             stun_msg;
	in_addr                  addr;
	NAT_Filtering_Behavior   filtering_behavior = NAT_FILTERING_UNKNOWN;
	Stun_Att_Mapped_Address  mapped_addr;
	Stun_Att_Response_Origin res_origin;

	/* clear the send/receive data */
	memset(data_ptr, 0, sizeof(char) * MAX_RECV);
	memset((char*) &stun_msg, 0, sizeof(Stun_Message));

	/* create end-point */
	EndPoint* ep_ptr = create_EP(0, SOCK_DGRAM, IPPROTO_IP, TIMEOUT_SEC, TIMEOUT_USEC, &rval);

	/* end-point create successfully */
	if ( ep_ptr != NULL && rval == EID_OK)
	{
		/* connect to stun server */
		if ( connect_EP(ep_ptr, host_addr, host_port) == EID_OK )
		{
			/* Encode Binding_Request message before sending for the 1st time */
			stun_msg.header.msg_type = STUN_MT_BINDING_REQ;
			stun_msg.header.magic_cookie = STUN_MAGIC_COOKIE;

			stun_msg.data.binding_req.change_req.header.att_type = STUN_AT_CHANGE_REQ;
			stun_msg.data.binding_req.change_req.header.att_len  = 4;
			stun_msg.data.binding_req.change_req.header.valid    = TRUE;
			stun_msg.data.binding_req.change_req.change_ip_port  = 0; /* no change ip & port */

			printf("\tTEST 1: send message to %s:%d\n",
					inet_ntoa(ep_ptr->dst_sa_in.sin_addr),
					ep_ptr->dst_sa_in.sin_port);
			/* encode, send and wait for first response */
			if ( encode_send_wait(ep_ptr, &stun_msg, data_ptr) )
			{
				/* Decode the response*/
				stun_decode_message(data_ptr, &stun_msg);
				/* save mapped address for comparing with local address */
				if ( stun_msg.data.binding_res.mapped_addr.header.valid )
				{
					mapped_addr = stun_msg.data.binding_res.mapped_addr;

					g_public_port = htons(mapped_addr.port_number);
					g_public_addr = htonl(mapped_addr.ip_address);
				}
				/* save response origin for further comparing */
				if (stun_msg.data.binding_res.res_origin.header.valid)
				{
					res_origin = stun_msg.data.binding_res.res_origin;
				}

				addr.s_addr = ntohl(res_origin.ip_address);
				printf("\t\tReceived response from %s:%d\n", inet_ntoa(addr), res_origin.port_number);

				/* compare with NATed address for checking NAT_NONE */
				if (mapped_addr.ip_address == ep_ptr->local_addr &&
					mapped_addr.port_number == ep_ptr->local_port)
				{
					printf("Public_Address is identical to Local_Address\n");
					filtering_behavior = NAT_FILTERING_NONE;
				}
				else
				{
					/* clear the send/receive data */
					memset(data_ptr, 0, sizeof(char) * MAX_RECV);
					memset((char*) &stun_msg, 0, sizeof(Stun_Message));
					/* Encode Binding_Request message before sending for the 2nd time */
					stun_msg.header.msg_type = STUN_MT_BINDING_REQ;
					stun_msg.header.magic_cookie = STUN_MAGIC_COOKIE;

					stun_msg.data.binding_req.change_req.header.att_type = STUN_AT_CHANGE_REQ;
					stun_msg.data.binding_req.change_req.header.att_len  = 4;
					stun_msg.data.binding_req.change_req.header.valid    = TRUE;
					stun_msg.data.binding_req.change_req.change_ip_port  = STUN_CHANGE_ADDR | STUN_CHANGE_PORT; /* change ip & port */

					printf("\tTEST 2: Send message Change IP & Port to %s:%d\n",
							inet_ntoa(ep_ptr->dst_sa_in.sin_addr),
							ep_ptr->dst_sa_in.sin_port);
					/* encode, send and wait for first response */
					if ( encode_send_wait(ep_ptr, &stun_msg, data_ptr) )
					{
						/* Decode the response*/
						stun_decode_message(data_ptr, &stun_msg);
						/* save response origin for further comparing */
						if (stun_msg.data.binding_res.res_origin.header.valid)
						{
							res_origin = stun_msg.data.binding_res.res_origin;
						}

						addr.s_addr = ntohl(res_origin.ip_address);
						printf("\t\tReceived response from %s:%d\n", inet_ntoa(addr), res_origin.port_number);

						filtering_behavior = NAT_FILTERING_ENDPOINT_INDEPENDENT;
					}
					else
					{
						/* clear the send/receive data */
						memset(data_ptr, 0, sizeof(char) * MAX_RECV);
						memset((char*) &stun_msg, 0, sizeof(Stun_Message));
						/* Encode Binding_Request message before sending for the 2nd time */
						stun_msg.header.msg_type = STUN_MT_BINDING_REQ;
						stun_msg.header.magic_cookie = STUN_MAGIC_COOKIE;

						stun_msg.data.binding_req.change_req.header.att_type = STUN_AT_CHANGE_REQ;
						stun_msg.data.binding_req.change_req.header.att_len  = 4;
						stun_msg.data.binding_req.change_req.header.valid    = TRUE;
						stun_msg.data.binding_req.change_req.change_ip_port  = STUN_CHANGE_PORT; /* change port */

						printf("\tTEST 3: send message Change Port to %s:%d\n",
								inet_ntoa(ep_ptr->dst_sa_in.sin_addr),
								ep_ptr->dst_sa_in.sin_port);
						/* encode, send and wait for first response */
						if ( encode_send_wait(ep_ptr, &stun_msg, data_ptr) )
						{
							addr.s_addr = ntohl(res_origin.ip_address);
							printf("\t\tReceived response from %s:%d\n", inet_ntoa(addr), res_origin.port_number);
							filtering_behavior = NAT_FILTERING_ADDRESS_DEPENDENT;
						}
						else
						{
							printf("\t\tTimeout\n");
							filtering_behavior = NAT_FILTERING_ADDRESS_PORT_DEPENDENT;
						}
					} /* else send 2nd message */
				} /* else comparing mapped_address with local_address */
			} /* else send 1st message */
		} /* if connect to STUN host */
	}

	g_local_addr = ep_ptr->local_addr;
	g_local_port = ep_ptr->local_port;

	/* close all the connection */
	delete_EP(ep_ptr);

	return filtering_behavior;
}

/*
 * Purpose:
 * - Create sockets and then connect to open STUN server for detecting NAT filtering behavior
 *
 * Input:
 * - [in] local_port: the local port that client want to open for incoming/outgoing stream
 * - [in] host_addr: address of open stun server
 * - [in] host_port: port of open stun server
 *
 * Output:
 * - If detect successfully return true
 * - Else return false
 */
NAT_Mapping_Behavior stun_detect_nat_mapping
(
	uint16_t local_port,   /* [in] the local port that client want to open for incoming/outgoing stream */
	char*    host_addr,    /* [in] address of open stun server */
	uint16_t host_port    /* [in] port of open stun server */
)
{
	uint8_t               rval;
	Stun_Message          stun_msg;
	char                  data_ptr[MAX_RECV];
	in_addr               addr;
	NAT_Mapping_Behavior  mapping_behavior = NAT_MAPPING_UNKNOWN;
	in_addr               alternative_addr;
	uint16_t              primary_port, alternative_port;
	Stun_Att_Mapped_Address    mapped_addr_1, mapped_addr_2;
	Stun_Att_Response_Origin   res_origin;
	Stun_Att_Other_Address     other_addr;

	/* create end-point for test */
	EndPoint* ep_ptr = create_EP(0, SOCK_DGRAM, IPPROTO_IP, TIMEOUT_SEC, TIMEOUT_USEC, &rval);

	if ( ep_ptr == NULL )
	{
		return mapping_behavior;
		printf("%s - %d: Detect mapping error %d", __FILE__, __LINE__, rval);
	}

	/* connect to primary address & primary port for Testing 1 */
	if ( connect_EP(ep_ptr, host_addr, host_port) != EID_OK )
	{
		return mapping_behavior;
	}

	printf("\tTEST 1: Send message to Primary IP & Primary Port: %s:%d\n",
			inet_ntoa(ep_ptr->dst_sa_in.sin_addr),
			ep_ptr->dst_sa_in.sin_port);

	/* clear the send/receive data */
	memset(data_ptr, 0, sizeof(char) * MAX_RECV);
	memset((char*) &stun_msg, 0, sizeof(Stun_Message));

	/* Encode Binding_Request message before sending for the test 1 */
	stun_msg.header.msg_type = STUN_MT_BINDING_REQ;
	stun_msg.header.magic_cookie = STUN_MAGIC_COOKIE;

	stun_msg.data.binding_req.change_req.header.att_type = STUN_AT_CHANGE_REQ;
	stun_msg.data.binding_req.change_req.header.att_len  = 4;
	stun_msg.data.binding_req.change_req.header.valid    = TRUE;
	stun_msg.data.binding_req.change_req.change_ip_port  = 0; /* no change ip & port */

	/* encode, send and wait for first response */
	if ( encode_send_wait(ep_ptr, &stun_msg, data_ptr) )
	{
		/* Decode the response*/
		stun_decode_message(data_ptr, &stun_msg);
		/* save mapped address for comparing with local address */
		if ( stun_msg.data.binding_res.mapped_addr.header.valid )
		{
			mapped_addr_1 = stun_msg.data.binding_res.mapped_addr;

			g_public_port = htons(mapped_addr_1.port_number);
			g_public_addr = htonl(mapped_addr_1.ip_address);
		}
		/* save response origin for further comparing */
		if (stun_msg.data.binding_res.res_origin.header.valid)
		{
			res_origin = stun_msg.data.binding_res.res_origin;
		}
		/* save other address */
		if (stun_msg.data.binding_res.other_addr.header.valid )
		{
			other_addr = stun_msg.data.binding_res.other_addr;
		}
		/* get primary port */
		primary_port        = res_origin.port_number;
		/* get alternative address & port */
		alternative_addr.s_addr = htonl(other_addr.ip_address);
		alternative_port        = other_addr.port_number;

		addr.s_addr = ntohl(res_origin.ip_address);
		printf("\t\tReceived response from: %s:%d\n", inet_ntoa(addr), res_origin.port_number);

		/* compare with NATed address for checking NAT_NONE */
		if (mapped_addr_1.ip_address == ep_ptr->local_addr &&
			mapped_addr_1.port_number == ep_ptr->local_port)
		{
			printf("\t\tPublic_Address is identical to Local_Address\n");
			mapping_behavior = NAT_MAPPING_NONE;
		}
	}/* if TEST_1 */
	else
	{
		printf("\t\tTime out - NAT mapping detecting failed!!!\n");
	}

	/* TEST_2 */
	if ( mapping_behavior == NAT_MAPPING_UNKNOWN )
	{
		printf("\tTEST 2: Send message to Alternative IP & Primary Port: %s:%d\n",
					inet_ntoa(alternative_addr), primary_port);

		/* connect to primary address & primary port for Testing 1 */
		if ( connect_EP(ep_ptr, inet_ntoa(alternative_addr), primary_port) != EID_OK )
		{
			return mapping_behavior;
		}

		/* clear the send/receive data */
		memset(data_ptr, 0, sizeof(char) * MAX_RECV);
		memset((char*) &stun_msg, 0, sizeof(Stun_Message));

		/* Encode Binding_Request message before sending for the test 1 */
		stun_msg.header.msg_type = STUN_MT_BINDING_REQ;
		stun_msg.header.magic_cookie = STUN_MAGIC_COOKIE;

		stun_msg.data.binding_req.change_req.header.att_type = STUN_AT_CHANGE_REQ;
		stun_msg.data.binding_req.change_req.header.att_len  = 4;
		stun_msg.data.binding_req.change_req.header.valid    = TRUE;
		stun_msg.data.binding_req.change_req.change_ip_port  = 0; /* no change ip & port */

		/* encode, send and wait for first response */
		if ( encode_send_wait(ep_ptr, &stun_msg, data_ptr) )
		{
			/* Decode the response*/
			stun_decode_message(data_ptr, &stun_msg);
			/* save mapped address for comparing with local address */
			if ( stun_msg.data.binding_res.mapped_addr.header.valid )
			{
				mapped_addr_2 = stun_msg.data.binding_res.mapped_addr;
			}
			/* save response origin for further comparing */
			if (stun_msg.data.binding_res.res_origin.header.valid)
			{
				res_origin = stun_msg.data.binding_res.res_origin;
			}

			addr.s_addr = ntohl(res_origin.ip_address);
			printf("\t\tReceived response from: %s:%d\n", inet_ntoa(addr), res_origin.port_number);

			/* compare with NATed address for checking NAT_NONE */
			if (mapped_addr_1.ip_address == mapped_addr_2.ip_address &&
				mapped_addr_1.port_number == mapped_addr_2.port_number)
			{
				printf("\t\tPublic_Address is identical to Local_Address\n");
				mapping_behavior = NAT_MAPPING_ENDPOINT_INDEPENDENT;
			}
		}/* if send 2nd message */
		else
		{
			printf("\t\tTime out - NAT mapping detecting failed!!!\n");
		}
	}/* if TEST_2 */

	/* TEST_3 */
	if ( mapping_behavior == NAT_MAPPING_UNKNOWN )
	{
		printf("\tTEST 3: Send message to Alternative IP & Alternative Port: %s:%d\n",
					inet_ntoa(alternative_addr), alternative_port);

		/* connect to primary address & primary port for Testing 1 */
		if ( connect_EP(ep_ptr, inet_ntoa(alternative_addr), alternative_port) != EID_OK )
		{
			return mapping_behavior;
		}

		/* clear the send/receive data */
		memset(data_ptr, 0, sizeof(char) * MAX_RECV);
		memset((char*) &stun_msg, 0, sizeof(Stun_Message));

		/* Encode Binding_Request message before sending for the test 1 */
		stun_msg.header.msg_type = STUN_MT_BINDING_REQ;
		stun_msg.header.magic_cookie = STUN_MAGIC_COOKIE;

		stun_msg.data.binding_req.change_req.header.att_type = STUN_AT_CHANGE_REQ;
		stun_msg.data.binding_req.change_req.header.att_len  = 4;
		stun_msg.data.binding_req.change_req.header.valid    = TRUE;
		stun_msg.data.binding_req.change_req.change_ip_port  = 0; /* no change ip & port */

		/* encode, send and wait for first response */
		if ( encode_send_wait(ep_ptr, &stun_msg, data_ptr) )
		{
			/* Decode the response*/
			stun_decode_message(data_ptr, &stun_msg);
			/* save mapped address for comparing with local address */
			if ( stun_msg.data.binding_res.mapped_addr.header.valid )
			{
				mapped_addr_1 = stun_msg.data.binding_res.mapped_addr;
			}
			/* save response origin for further comparing */
			if (stun_msg.data.binding_res.res_origin.header.valid)
			{
				res_origin = stun_msg.data.binding_res.res_origin;
			}

			addr.s_addr = ntohl(res_origin.ip_address);
			printf("\t\tReceived response from: %s:%d\n", inet_ntoa(addr), res_origin.port_number);

			/* compare with NATed address for checking NAT_NONE */
			if (mapped_addr_1.ip_address == mapped_addr_2.ip_address &&
				mapped_addr_1.port_number == mapped_addr_2.port_number)
			{
				printf("\t\tPublic_Address is identical to Local_Address\n");
				mapping_behavior = NAT_MAPPING_ADDRESS_DEPENDENT;
			}
			else
			{
				printf("\t\tPublic_Address is NOT identical to Local_Address\n");
				mapping_behavior = NAT_MAPPING_ADDRESS_PORT_DEPENDENT;
			}
		}/* if send 3rd message */
		else
		{
			printf("\t\tTime out - NAT mapping detecting failed!!!\n");
		}
	}/* if TEST_3 */

	g_local_addr = ep_ptr->local_addr;
	g_local_port = ep_ptr->local_port;

	/* delete end-point before exiting */
	delete_EP(ep_ptr);

	return mapping_behavior;
}

/*
 * CAUTION:
 * - Run the stun_detect_nat_mapping_behavior & stun_detect_nat_filtering_behavior first
 * Purpose:
 * - Depend on the input NAT Mapping & Filtering Behavior (RFC-5780) to return out the NAT type (RFC-3489)
 *
 * Input:
 * - [in] (RFC-5780) mapping_behavior: mapping behavior
 * - [in] (RFC-5780) filtering_behavior: filtering behavior
 *
 * Output:
 * - the NAT type (RFC-3489)
 * - Else return false
 */
NAT_Type stun_get_nat_type
(
	NAT_Mapping_Behavior   mapping,     /* [in] mapping behavior   */
	NAT_Filtering_Behavior filtering    /* [in] filtering behavior */
)
{
	if ( (mapping < 0 || mapping >= NAT_MAPPING_COUNT) ||
		 (filtering < 0 || filtering >= NAT_FILTERING_COUNT))
	{
		return NAT_TYPE_UNKNOWN;
	}
	return (NAT_Type)g_nat_type_tbl[filtering][mapping];
}

/*
 * Purpose:
 * - Using STUN protocol to get public IP address & Port number
 *
 * Input:
 * - [in] local_port: the local port that client want to open for incoming/outgoing stream
 * - [in] host_addr: address of open stun server
 * - [in] host_port: port of open stun server
 * - [out] public_addr: the public address
 * - [out] public_port: public port
 *
 * Output:
 * - If get public address successfully return true
 * - Else return false
 */
void stun_detect_public_address
(
	uint16_t   local_port,   /* [in] the local port that client want to open for incoming/outgoing stream */
	char*      host_addr,    /* [in] address of open stun server */
	uint16_t   host_port,    /* [in] port of open stun server */
	char*      public_addr,  /* [out] public address */
	uint16_t*  public_port   /* [out] public port */
)
{}

/*
 * Purpose:
 * - Get the Mapping Behavior string depend on input mapping behavior type
 *
 * Input:
 * - [in]  mapping: the mapping behavior type
 *
 * Output:
 * - Return string of mapping behavior type
 */
const char* stun_get_get_mapping_name(NAT_Mapping_Behavior mapping)
{
	if ( 0 <= mapping && mapping <= NAT_FILTERING_COUNT )
	{
		return g_filtering_name_tbl[mapping];
	}
	return NULL;
}

/*
 * Purpose:
 * - Get the Filtering Behavior string depend on input filtering behavior type
 *
 * Input:
 * - [in]  filtering: the filtering behavior type
 *
 * Output:
 * - Return string of filtering behavior type
 */
const char* stun_get_filtering_name(NAT_Filtering_Behavior filtering)
{
	if ( 0 <= filtering && filtering <= NAT_FILTERING_COUNT )
	{
		return g_filtering_name_tbl[filtering];
	}
	return NULL;
}

/*
 * Purpose:
 * - Get the NAT type string depend on input NAT type
 *
 * Input:
 * - [in]  nat_type: the NAT type
 *
 * Output:
 * - Return string of NAT type
 */
const char* stun_get_nat_name(NAT_Type nat_type)
{
	if ( 0 <= nat_type && nat_type <= NAT_TYPE_COUNT )
	{
		return g_nat_type_name_tbl[nat_type];
	}
	return NULL;
}
