/*
 * File name: stun.h
 *
 * Purpose:
 *
 * Created on: Nov 3, 2014
 *     Author: ddhuy
 *
 * File history:
 */


#ifndef STUN_H_
#define STUN_H_

#include "stun_attribute.h"
#include "stun_const.h"
#include "stun_message.h"
#include "stun_func.h"

/***************************************************************
 *
 * NAT constants
 *
 ***************************************************************/
#define STUN_PRIMARY_PORT     (3478)
#define STUN_ALTERNATIVE_PORT (3479)

/***************************************************************
 *
 * NAT type (RFC-3489)
 *
 ***************************************************************/
typedef enum
{
	NAT_TYPE_NONE                 = 0,
	NAT_TYPE_FULL_CONE            = 1,
	NAT_TYPE_RESTRICTED_CONE      = 2,
	NAT_TYPE_PORT_RESTRICTED_CONE = 3,
	NAT_TYPE_4                    = 4,
	NAT_TYPE_5                    = 5,
	NAT_TYPE_6                    = 6,
	NAT_TYPE_7                    = 7,
	NAT_TYPE_8                    = 8,
	NAT_TYPE_SYMMETRIC            = 9,
	NAT_TYPE_UDP_BLOCKED          = 10,
	NAT_TYPE_SYMMETRIC_FIREWALL   = 11,
	NAT_TYPE_UNKNOWN,
} NAT_Type;

#define NAT_TYPE_COUNT 12

/***************************************************************
 *
 * NAT Mapping Behavior (RFC-5780)
 *
 ***************************************************************/
typedef enum
{
	NAT_MAPPING_NONE                    = 0,
	NAT_MAPPING_ENDPOINT_INDEPENDENT    = 1,
	NAT_MAPPING_ADDRESS_DEPENDENT       = 2,
	NAT_MAPPING_ADDRESS_PORT_DEPENDENT  = 3,
	NAT_MAPPING_UNKNOWN,
} NAT_Mapping_Behavior;

#define NAT_MAPPING_COUNT 4

/***************************************************************
 *
 * NAT Filtering Behavior (RFC-5780)
 *
 ***************************************************************/
typedef enum
{
	NAT_FILTERING_NONE                    = 0,
	NAT_FILTERING_ENDPOINT_INDEPENDENT    = 1,
	NAT_FILTERING_ADDRESS_DEPENDENT       = 2,
	NAT_FILTERING_ADDRESS_PORT_DEPENDENT  = 3,
	NAT_FILTERING_UNKNOWN,
} NAT_Filtering_Behavior;

#define NAT_FILTERING_COUNT 4

/***************************************************************
 *
 * NAT/STUN functions
 *
 ***************************************************************/
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
);

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
NAT_Mapping_Behavior stun_detect_nat_mapping
(
	uint16_t local_port,   /* [in] the local port that client want to open for incoming/outgoing stream */
	char*    host_addr,    /* [in] address of open stun server */
	uint16_t host_port    /* [in] port of open stun server */
);

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
NAT_Filtering_Behavior stun_detect_nat_filtering
(
	uint16_t local_port,   /* [in] the local port that client want to open for incoming/outgoing stream */
	char*    host_addr,    /* [in] address of open stun server */
	uint16_t host_port    /* [in] port of open stun server */
);

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
);

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
uint16_t stun_get_local_port();

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
char* stun_get_local_address();

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
uint16_t stun_get_public_port();

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
char* stun_get_public_address();

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
const char* stun_get_mapping_name(NAT_Mapping_Behavior mapping);

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
const char* stun_get_filtering_name(NAT_Filtering_Behavior filtering);

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
const char* stun_get_nat_name(NAT_Type nat_type);


#endif /* STUN_H_ */
