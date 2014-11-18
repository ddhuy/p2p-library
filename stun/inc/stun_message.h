/*
 * File name: stun_message.h
 *
 * Purpose:
 *
 * Created on: Nov 3, 2014
 *     Author: ddhuy
 *
 * File history:
 */


#ifndef STUN_MESSAGE_H_
#define STUN_MESSAGE_H_

/*****************************************************************************/
/*-------------------------  HEADERS & INCLUDE ------------------------------*/
/*****************************************************************************/
#include <stdint.h>

#include "stun_attribute.h"

/*****************************************************************************/
/*----------------------  STUN MESSAGES DEFINES  ----------------------------*/
/*****************************************************************************/

/* STUN header */
typedef struct
{
	Stun_Message_Type msg_type;
	int16_t           msg_len;
	uint32_t          magic_cookie;
	uint8_t           transaction_id[12];
} Stun_Header;

/* Message type Binding Request */
typedef struct
{
	Stun_Att_Response_Address   response_addr;
	Stun_Att_Change_Request     change_req;
	Stun_Att_Username           username;
	Stun_Att_Message_Integrity  msg_integrity;
} Stun_Msg_Binding_Req;

/* Message type Binding Response */
typedef struct
{
	Stun_Att_Mapped_Address     mapped_addr;
	Stun_Att_Response_Origin    res_origin;
	Stun_Att_Other_Address      other_addr;
	Stun_Att_XOR_Mapped_Address xor_mapped_addr;
	Stun_Att_Source_Address     source_addr;
	Stun_Att_Changed_Address    changed_addr;
	Stun_Att_Message_Integrity  msg_integrity;
	Stun_Att_Reflect_From       reflect_from;
} Stun_Msg_Binding_Res;

/* Message type Binding Error Response */
typedef struct
{
	Stun_Att_Error_Code         error_code;
	Stun_Att_Unknown_Attribute  unknown_att;
} Stun_Msg_Binding_Err_Res;

/* Message type Binding Request */
typedef struct
{
	/* Empty struct */
} Stun_Msg_Shared_Secret_Req;

/* Message type Binding Response */
typedef struct
{
	Stun_Att_Username username;
	Stun_Att_Password password;
} Stun_Msg_Shared_Secret_Res;

/* Message type Binding Error Response */
typedef struct
{
	Stun_Att_Error_Code         error_code;
	Stun_Att_Unknown_Attribute  unknown_att;
} Stun_Msg_Shared_Secret_Err_Res;

/* STUN Message */
typedef struct
{
	/* STUN header */
	Stun_Header header;
	/* Union of all STUN messages */
	union
	{
		Stun_Msg_Binding_Req            binding_req;
		Stun_Msg_Binding_Res            binding_res;
		Stun_Msg_Binding_Err_Res        binding_err_res;
		Stun_Msg_Shared_Secret_Req      shared_secret_req;
		Stun_Msg_Shared_Secret_Res      shared_secret_res;
		Stun_Msg_Shared_Secret_Err_Res  shared_secret_err_res;
	} data;

} Stun_Message;

#endif /* STUN_MESSAGE_H_ */
