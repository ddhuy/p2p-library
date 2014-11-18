/*
 * File name: stun_attribute.h
 *
 * Purpose:
 *
 * Created on: Nov 3, 2014
 *     Author: ddhuy
 *
 * File history:
 */


#ifndef STUN_ATTRIBUTE_H_
#define STUN_ATTRIBUTE_H_

/* =========================================================================
 * Headers & Include
 * =========================================================================
 */
#include <stdint.h>

#include "stun_const.h"

/*****************************************************************************/
/*--------------------------  CONSTANT DEFINE -------------------------------*/
/*****************************************************************************/
#define SIZE 64


/*****************************************************************************/
/*-----------------  STUN ATTRIBUTE DEFINES  ---------------------------------*/
/*****************************************************************************/
/* Stun common header */
typedef struct
{
	Stun_Attribute_Type att_type;
	uint16_t            att_len;
	char                valid;
} Stun_Att_Header;

/* Attribute Mapped Address */
typedef struct
{
	Stun_Att_Header header;
	uint16_t   addr_family; /* address family */
	uint16_t   port_number; /* port number    */
	uint32_t   ip_address;  /* IPv4 address   */
} Stun_Att_Mapped_Address;

/* Attribute Response Address */
typedef struct
{
	Stun_Att_Header header;
	uint16_t   addr_family; /* address family */
	uint16_t   port_number; /* port number    */
	uint32_t   ip_address;  /* IP address     */
} Stun_Att_Response_Address;

/* Attribute Change Request */
typedef struct
{
	Stun_Att_Header header;
	uint32_t   change_ip_port;  /* change IP & Port flag */
} Stun_Att_Change_Request;

/* Attribute Source Address */
typedef struct
{
	Stun_Att_Header header;
	uint16_t   addr_family; /* address family */
	uint16_t   port_number; /* port number    */
	uint32_t   ip_address;  /* IP address     */
} Stun_Att_Source_Address;

/* Attribute Changed Address */
typedef struct
{
	Stun_Att_Header header;
	uint16_t   addr_family; /* address family */
	uint16_t   port_number; /* port number    */
	uint32_t   ip_address;  /* IP address     */
} Stun_Att_Changed_Address;

/* Response Origin */
typedef struct
{
	Stun_Att_Header header;
	uint16_t   addr_family; /* address family */
	uint16_t   port_number; /* port number    */
	uint32_t   ip_address;  /* IP address     */
} Stun_Att_Response_Origin;

/* Attribute Username */
typedef struct
{
	Stun_Att_Header header;
	char username[SIZE];
} Stun_Att_Username;

/* Attribute Password */
typedef struct
{
	Stun_Att_Header header;
	char password[SIZE];
} Stun_Att_Password;

/* Attribute Message Integrity */
typedef struct
{
	Stun_Att_Header header;
} Stun_Att_Message_Integrity;

/* Attribute Error Code */
typedef struct
{
	Stun_Att_Header header;
	union
	{
		struct
		{
			uint32_t res    : 21;  /* reserved for future use           */
			uint32_t class  : 3;   /* error class: from 1 to 6          */
			uint32_t number : 8;   /* error number = error_code % 100   */
		} ea;                  /* mapped to RFC3489 - Page 29       */
		Stun_Error_Code ec;     /* error code RFC3489 - Page 29      */
	} error_code;              /* Error code                        */
	char         reason[24]; /* Reason Phrase                     */
} Stun_Att_Error_Code;

/* Attribute Unknown Attribute */
typedef struct
{
	Stun_Att_Header header;
	uint16_t   unknown_att[SIZE]; /* list of unknown attribute types */
} Stun_Att_Unknown_Attribute;

/* Attribute Reflect From */
typedef struct
{
	Stun_Att_Header header;
	uint16_t   addr_family; /* address family */
	uint16_t   port_number; /* port number    */
	uint32_t   ip_address;  /* IP address     */
} Stun_Att_Reflect_From;

/* Attribute Other Address */
typedef struct
{
	Stun_Att_Header header;
	uint16_t   addr_family; /* address family */
	uint16_t   port_number; /* port number    */
	uint32_t   ip_address;  /* IP address     */
} Stun_Att_Other_Address;

/* Attribute REALM */
typedef struct
{
	Stun_Att_Header header;
} Stun_Att_Realm;

/* Attribute NONCE */
typedef struct
{
	Stun_Att_Header header;
} Stun_Att_Nonce;

/* Attribute XOR Mapped Address */
typedef struct
{
	Stun_Att_Header header;
	uint16_t   addr_family; /* address family */
	uint16_t   port_number; /* port number    */
	uint32_t   ip_address;  /* IP address     */
} Stun_Att_XOR_Mapped_Address;

/* Attribute XOR Mapped Address */
typedef struct
{
	Stun_Att_Header header;
} Stun_Att_Software;

/* Attribute XOR Mapped Address */
typedef struct
{
	Stun_Att_Header header;
} Stun_Att_Altenate_Server;

/* Attribute XOR Mapped Address */
typedef struct
{
	Stun_Att_Header header;
} Stun_Att_Fingerprint;

/* Attribute Padding */
typedef struct
{
	Stun_Att_Header header;
} Stun_Att_Padding;

/* Attribute Response Port */
typedef struct
{
	Stun_Att_Header header;
} Stun_Att_Response_Port;

/* Attribute Cache Timeout */
typedef struct
{
	Stun_Att_Header header;
} Stun_Att_Cache_Timeout;

/* Dummy Attribute */
typedef struct
{
	Stun_Att_Header header;
} Stun_Att_Dummy;

/* Common STUN attribute structure */
typedef union
{
	Stun_Att_Dummy              dummy;
	Stun_Att_Mapped_Address     mapped_addr;
	Stun_Att_Response_Address   response_addr;
	Stun_Att_Change_Request     change_req;
	Stun_Att_Source_Address     source_addr;
	Stun_Att_Changed_Address    changed_addr;
	Stun_Att_Username           username;
	Stun_Att_Password           password;
	Stun_Att_Message_Integrity  msg_integrity;
	Stun_Att_Error_Code         error_code;
	Stun_Att_Unknown_Attribute  unknown_att;
	Stun_Att_Reflect_From       reflect_from;
	Stun_Att_Realm              realm;
	Stun_Att_Nonce              nonce;
	Stun_Att_XOR_Mapped_Address xor_mapped_addr;
	Stun_Att_Padding            padding;
	Stun_Att_Response_Port      res_port;
	Stun_Att_Software           software;
	Stun_Att_Altenate_Server    alternate_server;
	Stun_Att_Cache_Timeout      cache_timeout;
	Stun_Att_Fingerprint        fingerprint;
	Stun_Att_Response_Origin    response_origin;
	Stun_Att_Other_Address      other_addr;
} Stun_Attribute;

#endif /* STUN_ATTRIBUTE_H_ */
