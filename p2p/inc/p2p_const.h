/*
 * File name: p2p_const.h
 *
 * Purpose:
 *
 * Created on: Nov 5, 2014
 *     Author: ddhuy
 *
 * File history:
 */


#ifndef P2P_INC_P2P_CONST_H_
#define P2P_INC_P2P_CONST_H_

/***************************************************************
 *
 * P2P constants
 *
 ***************************************************************/
#define P2P_IPV4 (0)
#define P2p_IPV6 (1)

/* P2P Message Types */
typedef enum
{
	P2P_MSG_NULL,
	P2P_MSG_REGISTRATION_REQUEST,
	P2P_MSG_REGISTRATION_RESPONSE,
	P2P_MSG_CONNECTION_REQUEST,
	P2P_MSG_CONNECTION_RESPONSE,
	P2P_MSG_HANDSHAKE,
	P2P_MSG_HANDSHAKE_ACK,
	P2P_MSG_KEEPALIVE,
	P2P_MSG_COUNT,
} P2P_Message_Type;

/* P2P Information Element Identifier Types */
typedef enum
{
	P2P_IE_NULL          = 0x00,
	P2P_IE_LOCAL_ADDRESS = 0x01,
	P2P_IE_PUBLIC_ADDRESS,
	P2P_IE_MAC_ADDRESS,
	P2P_IE_USER_INFO,
	P2P_IE_ERROR_CAUSE,
	P2P_IE_COUNT,
} P2P_Element_Type;

/* P2P Bad Message Causes */
typedef enum
{
	P2P_CAUSE_NULL     = 0x00,
	P2P_CAUSE_COUNT,
} P2P_Cause;

/* P2P Information Element Identifier Types */
typedef enum
{
	P2P_LL_LOCAL_ADDRESS  = 8,
	P2P_LL_PUBLIC_ADDRESS = 8,
	P2P_LL_MAC_ADDRESS    = 6,
	P2P_LL_USER_INFO      = 8,
	P2P_LL_ERROR_CAUSE    = 8,
} P2P_Min_IE_Length;

/* P2P Information Element Identifier Types */
typedef enum
{
	P2P_LH_LOCAL_ADDRESS   = 12,
	P2P_LH_PUBLIC_ADDRESS  = 12,
	P2P_LH_MAC_ADDRESS     = 6,
	P2P_LH_USER_INFO       = 12,
	P2P_LH_ERROR_CAUSE     = 8,
} P2P_Max_IE_Length;

#endif /* P2P_INC_P2P_CONST_H_ */
