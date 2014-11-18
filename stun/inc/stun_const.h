/*
 * File name: stun_const.h
 *
 * Purpose:
 *
 * Created on: Nov 3, 2014
 *     Author: ddhuy
 *
 * File history:
 */


#ifndef STUN_CONST_H_
#define STUN_CONST_H_


/***************************************************************
 *
 * STUN macro
 *
 ***************************************************************/
/* Default Magic Cookie always 0x2112A442 */
#define STUN_MAGIC_COOKIE 0x2112A442

#define STUN_MAGIC_COOKIE_OCTET1   0x21
#define STUN_MAGIC_COOKIE_OCTET2   0x12
#define STUN_MAGIC_COOKIE_OCTET3   0xA4
#define STUN_MAGIC_COOKIE_OCTET4   0x42

#define STUN_MAGIC_COOKIE_XOR_PORT 0x2112

/* Address family type */
#define STUN_IPV4_ADDRESS 0x01
#define STUN_IPV6_ADDRESS 0x02

/* Flag change IP & Port for attribute Change_Request */
#define STUN_CHANGE_PORT 0x02
#define STUN_CHANGE_ADDR 0x04

/* STUN header always has size 20 bytes */
#define STUN_HEADER_SIZE (20)


/***************************************************************
 *
 * STUN Message Types
 *
 ***************************************************************/
typedef enum
{
	/* Binding message types */
	STUN_MT_RESERVED               = 0x0000, /* Reserved not used            */
	STUN_MT_BINDING_REQ            = 0x0001, /* Binding Request              */
	STUN_MT_BINDING_RES            = 0x0101, /* Binding Response             */
	STUN_MT_BINDING_ERR_RES        = 0x0111, /* Binding Error Response       */
	/* Shared Secret message types */
	STUN_MT_SHARED_SECRET_REQ      = 0x0002, /* Shared Secret Request        */
	STUN_MT_SHARED_SECRET_RES      = 0x0102, /* Shared Secret Response       */
	STUN_MT_SHARED_SECRET_ERR_RES  = 0x0112  /* Shared Secret Error Response */
} Stun_Message_Type;

#define STUN_MAX_MESSAGE_TYPE 0x07 /* maximum NAT message type */


/***************************************************************
 *
 * STUN Attribute Types
 *
 ***************************************************************/
typedef enum
{
	/* Comprehension-required (0x0000-0x7FFF) */
	STUN_AT_RESERVED         = 0x0000, /* reserved not used  */
	STUN_AT_MAPPED_ADDR      = 0x0001, /* Mapped Address     */
	STUN_AT_RESP_ADDR        = 0x0002, /* Response Address   */
	STUN_AT_CHANGE_REQ       = 0x0003, /* Change Request     */
	STUN_AT_SOURCE_ADDR      = 0x0004, /* Source Address     */
	STUN_AT_CHANGED_ADDR     = 0x0005, /* Changed Address    */
	STUN_AT_USERNAME         = 0x0006, /* User name          */
	STUN_AT_PASSWORD         = 0x0007, /* Password           */
	STUN_AT_MSG_INTEGITY     = 0x0008, /* Message Integrity  */
	STUN_AT_ERROR_CODE       = 0x0009, /* Error Code         */
	STUN_AT_UNKNOWN_ATT      = 0x000A, /* Unknown Attribute  */
	STUN_AT_REFLECT_FROM     = 0x000B, /* Reflect From       */
	STUN_AT_REALM            = 0x0014, /* Realm              */
	STUN_AT_NONCE            = 0x0015, /* Nonce              */
	STUN_AT_XOR_MAPPED_ADDR  = 0x0020, /* XOR Mapped Address */
	STUN_AT_PADDING          = 0x0026, /* Padding            */
	STUN_AT_RESPONSE_PORT    = 0x0027, /* Response Port      */
	/* Comprehension-optional (0x8000-0xFFFF) */
	STUN_AT_SOFTWARE         = 0x8022, /* Software           */
	STUN_AT_ALTERNATE_SERVER = 0x8023, /* Alternate Server   */
	STUN_AT_CACHE_TIMEOUT    = 0x8027, /* Cache timeout      */
	STUN_AT_FINGERPRINT      = 0x8028, /* Fingerprint        */
	STUN_AT_RESPONSE_ORIGIN  = 0x802B, /* Response Origin    */
	STUN_AT_OTHER_ADDR       = 0x802C, /* Other Address      */
} Stun_Attribute_Type;

#define STUN_MAX_ATTRIBUTE_TYPE 0x16 /* Max STUN Attributes include comprehension required/optional */

/* First half range of the comprehension-required */
#define STUN_COMPREHENSION_REQUIRED_1ST_L 0x0000
#define STUN_COMPREHENSION_REQUIRED_1ST_H 0x3FFF
/* Second half range of the comprehension-required */
#define STUN_COMPREHENSION_REQUIRED_2ND_L 0x4000
#define STUN_COMPREHENSION_REQUIRED_2ND_H 0x7FFF

/* First half range of the comprehension-optional */
#define STUN_COMPREHENSION_OPTIONAL_1ST_L 0x8000
#define STUN_COMPREHENSION_OPTIONAL_1ST_H 0xBFFF
/* Second half range of the comprehension-optional */
#define STUN_COMPREHENSION_OPTIONAL_2ND_L 0xC000
#define STUN_COMPREHENSION_OPTIONAL_2ND_H 0xFFFF


/***************************************************************
 *
 * STUN Error Code
 *
 ***************************************************************/
typedef enum
{
	STUN_ERR_BAD_REQ           = 400, /* Bad Request              */
	STUN_ERR_UNAUTHORIZED      = 401, /* Unauthorized             */
	STUN_ERR_UNKNOWN_ATT       = 420, /* Unknown Attribute        */
	STUN_ERR_STALE_CREDENTIAL  = 430, /* Stale Credential         */
	STUN_ERR_INT_CHECK_FAILURE = 431, /* Integrity Check Failure  */
	STUN_ERR_MISSING_USERNAME  = 432, /* Missing Username         */
	STUN_ERR_USE_TLS           = 433, /* Use TLS                  */
	STUN_ERR_SERVER_ERROR      = 500, /* Server Error             */
	STUN_ERR_GLOBAL_FAILURE    = 600, /* Global Failure           */
} Stun_Error_Code;
#define STUN_MAX_ERROR_CODE 0x09 /* maximum NAT error code */

/***************************************************************
 *
 * Minimum value length of STUN Attribute type
 * (include Attribute Type and Attribute Length)
 *
 ***************************************************************/
typedef enum
{
	/* Comprehension-required (0x0000-0x7FFF) */
	STUN_LL_RESERVED         = 0,  /* reserved not used   */
	STUN_LL_MAPPED_ADDR      = 12, /* Mapped Address      */
	STUN_LL_RESP_ADDR        = 12, /* Response Address    */
	STUN_LL_CHANGE_REQ       = 8,  /* Change Request      */
	STUN_LL_SOURCE_ADDR      = 12, /* Source Address      */
	STUN_LL_CHANGED_ADDR     = 12, /* Changed Address     */
	STUN_LL_USERNAME         = 4,  /* User name           */
	STUN_LL_PASSWORD         = 4,  /* Password            */
	STUN_LL_MSG_INTEGITY     = 4,  /* Message Integrity   */ /* Not yet supported */
	STUN_LL_ERROR_CODE       = 4,  /* Error Code          */ /* Not yet supported */
	STUN_LL_UNKNOWN_ATT      = 4,  /* Unknown Attribute   */ /* Not yet supported */
	STUN_LL_REFLECT_FROM     = 4,  /* Reflect From        */ /* Not yet supported */
	STUN_LL_REALM            = 4,  /* Realm               */ /* Not yet supported */
	STUN_LL_NONCE            = 4,  /* Nonce               */ /* Not yet supported */
	STUN_LL_XOR_MAPPED_ADDR  = 12, /* XOR Mapped Address  */
	STUN_LL_PADDING          = 4,  /* Padding             */ /* Not yet supported */
	STUN_LL_RESPONSE_PORT    = 4,  /* Response Port       */ /* Not yet supported */
	/* Comprehension-optional (0x8000-0xFFFF) */
	STUN_LL_SOFTWARE         = 4,  /* Software            */ /* Not yet supported */
	STUN_LL_ALTERNATE_SERVER = 4,  /* Alternate Server    */ /* Not yet supported */
	STUN_LL_CACHE_TIMEOUT    = 4,  /* Cache timeout       */ /* Not yet supported */
	STUN_LL_FINGERPRINT      = 4,  /* Fingerprint         */ /* Not yet supported */
	STUN_LL_RESPONSE_ORIGIN  = 12, /* Response Origin     */
	STUN_LL_OTHER_ADDR       = 12, /* Other Address       */
} Stun_Att_Min_Length;

/***************************************************************
 *
 * Maximum value length of STUN Attribute type
 * (include Attribute Type and Attribute Length)
 *
 ***************************************************************/
typedef enum
{
	STUN_LH_RESERVED         = 0,   /* reserved not used  */
	STUN_LH_MAPPED_ADDR      = 12,  /* Mapped Address     */ /* Only support IPv4 address */
	STUN_LH_RESP_ADDR        = 12,  /* Response Address   */ /* Only support IPv4 address */
	STUN_LH_CHANGE_REQ       = 8,   /* Change Request     */ /* Only support IPv4 address */
	STUN_LH_SOURCE_ADDR      = 12,  /* Source Address     */ /* Only support IPv4 address */
	STUN_LH_CHANGED_ADDR     = 12,  /* Changed Address    */
	STUN_LH_USERNAME         = 32,  /* User name          */ /* Support only 32 characters */
	STUN_LH_PASSWORD         = 32,  /* Password           */ /* Support only 32 characters */
	STUN_LH_MSG_INTEGITY     = 24,  /* Message Integrity  */
	STUN_LH_ERROR_CODE       = 32,  /* Error Code         */
	STUN_LH_UNKNOWN_ATT      = 32,  /* Unknown Attribute  */
	STUN_LH_REFLECT_FROM     = 0,   /* Reflect From       */
	STUN_LH_REALM            = 0,   /* Realm              */
	STUN_LH_NONCE            = 0,   /* Nonce              */
	STUN_LH_XOR_MAPPED_ADDR  = 12,  /* XOR Mapped Address */
	STUN_LH_PADDING          = 0,   /* Padding            */
	STUN_LH_RESPONSE_PORT    = 8,   /* Response Port      */
	/* Comprehension-optional (0x8000-0xFFFF) */
	STUN_LH_SOFTWARE         = 0,   /* Software           */
	STUN_LH_ALTERNATE_SERVER = 0,   /* Alternate Server   */
	STUN_LH_CACHE_TIMEOUT    = 0,   /* Cache timeout      */
	STUN_LH_FINGERPRINT      = 0,   /* Fingerprint        */
	STUN_LH_RESPONSE_ORIGIN  = 12,  /* Response Origin    */
	STUN_LH_OTHER_ADDR       = 12,  /* Response Origin    */
} Stun_Att_Max_Length;

#endif /* STUN_CONST_H_ */
