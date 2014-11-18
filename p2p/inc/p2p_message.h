/*
 * File name: p2p_message.h
 *
 * Purpose:
 *
 * Created on: Nov 5, 2014
 *     Author: ddhuy
 *
 * File history:
 */


#ifndef INC_P2P_MESSAGE_H_
#define INC_P2P_MESSAGE_H_

#include "p2p_element.h"

#define P2P_MSG_HEADER_SIZE 4

/* MSG Registration Request */
typedef struct
{
	P2P_IE_User_Info      user_info;
	P2P_IE_Local_Address  local_addr;
} P2P_MSG_Registration_Request;

/* MSG Registration Response */
typedef struct
{
	P2P_IE_User_Info user_info;
} P2P_MSG_Registration_Response;

/* MSG Connection Request */
typedef struct
{
	P2P_IE_User_Info       user_info;
	P2P_IE_Local_Address   local_addr;
	P2P_IE_Public_Address  public_addr;
} P2P_MSG_Connection_Request;

/* MSG Connection Response */
typedef struct
{
	P2P_IE_User_Info       user_info;
	P2P_IE_Local_Address   local_addr;
	P2P_IE_Public_Address  public_addr;
} P2P_MSG_Connection_Response;

/* MSG Handshake */
typedef struct
{
	P2P_IE_User_Info user_info;
	P2P_IE_User_Info peer_info;
} P2P_MSG_Handshake;

/* MSG Handshake ACK */
typedef struct
{
	P2P_IE_User_Info user_info;
	P2P_IE_User_Info peer_info;
} P2P_MSG_Handshake_Ack;


/* P2P Message Header */
typedef struct
{
	uint16_t msg_type;
	uint16_t msg_len;
} P2P_Message_Header;

/* P2P Common Message */
typedef struct
{
	P2P_Message_Header hdr; /* structured data to P2P header */
	union
	{
		P2P_MSG_Connection_Request     con_req;
		P2P_MSG_Connection_Response    con_res;
		P2P_MSG_Registration_Request   reg_req;
		P2P_MSG_Registration_Response  reg_res;
		P2P_MSG_Handshake              handshk;
		P2P_MSG_Handshake_Ack          hdsk_ack;
	} msg;
} P2P_Message;

#endif /* INC_P2P_MESSAGE_H_ */
