/*
 * File name: p2p_func.h
 *
 * Purpose:
 *
 * Created on: Nov 5, 2014
 *     Author: ddhuy
 *
 * File history:
 */


#ifndef INC_P2P_FUNC_H_
#define INC_P2P_FUNC_H_

/***************************************************************
 *
 * File headers and includes
 *
 ***************************************************************/
#include <string.h>

#include "p2p_const.h"
#include "p2p_message.h"

/***************************************************************
 *
 * Type constants
 *
 ***************************************************************/
typedef enum
{
	P2P_EID_NO_ERROR,
	P2P_EID_MSG_TYPE_INVALID,
	P2P_EID_IE_TYPE_INVALID,
	P2P_EID_MSG_CONTENT_INVALID,
	P2P_EID_IE_CONTENT_INVALID,
	P2P_EID_MSG_LEN_ERROR,
	P2P_EID_IE_LEN_ERROR,
} P2P_ErrorCause;

/***************************************************************
 *
 * Type definition and Data structures
 *
 ***************************************************************/
typedef struct
{
	P2P_Element_Type ie_type;
	uint16_t         offset;
} P2P_MsgTbl;

typedef P2P_ErrorCause (*msg_chk_func_ptr)(P2P_Message*);
typedef struct
{
	P2P_Message_Type   msg_type;
	const P2P_MsgTbl*  tbl_ptr;
} P2P_MsgTbl_Ent;
extern const P2P_MsgTbl_Ent   g_msgtbl[];
extern const uint16_t         g_msgtbl_size;

typedef P2P_ErrorCause (*ie_chk_func)(P2P_Element*);
typedef P2P_ErrorCause (*ie_dec_func)(char*, P2P_Element*);
typedef P2P_ErrorCause (*ie_enc_func)(P2P_Element*, char*);
typedef struct
{
	P2P_Element_Type ele_type;
	uint16_t         ele_size;
	uint16_t         min_size;
	uint16_t         max_size;
	ie_dec_func      dec_func;
	ie_enc_func      enc_func;
} P2P_IE_Ent;
extern const P2P_IE_Ent  g_ieitbl[];
extern const uint16_t    g_ieitbl_size;

/***************************************************************
 *
 * Miscellaneous Functions prototypes
 *
 ***************************************************************/
const P2P_MsgTbl_Ent* p2p_get_msg_entry(P2P_Message_Type msg_type);

const P2P_IE_Ent* p2p_get_ie_entry(P2P_Element_Type iei_type);


/***************************************************************
 *
 * P2P IE Decoding Function
 *
 ***************************************************************/

#define IE_DEC_FUNC(func_name) \
	P2P_ErrorCause func_name   \
	(                          \
		char*        data_ptr, \
		P2P_Element* ie_ptr    \
    )                          \

IE_DEC_FUNC(p2p_ie_dec_local_addr);
IE_DEC_FUNC(p2p_ie_dec_public_addr);
IE_DEC_FUNC(p2p_ie_dec_mac_addr);
IE_DEC_FUNC(p2p_ie_dec_user_info);
IE_DEC_FUNC(p2p_ie_dec_err_cause);

P2P_ErrorCause p2p_decode_msg(char* data_ptr, P2P_Message* msg_ptr);
P2P_ErrorCause p2p_decode_ies(char* data_ptr, P2P_Message* msg_ptr, uint8_t msg_len, const P2P_MsgTbl* msgtbl_ptr);


/***************************************************************
 *
 * P2P IE Encoding Function
 *
 ***************************************************************/

#define IE_ENC_FUNC(func_name) \
	P2P_ErrorCause func_name   \
	(                          \
		P2P_Element* ie_ptr,   \
		char*        data_ptr  \
    )                          \

IE_ENC_FUNC(p2p_ie_enc_local_addr);
IE_ENC_FUNC(p2p_ie_enc_public_addr);
IE_ENC_FUNC(p2p_ie_enc_mac_addr);
IE_ENC_FUNC(p2p_ie_enc_user_info);
IE_ENC_FUNC(p2p_ie_enc_err_cause);

P2P_ErrorCause p2p_encode_msg(P2P_Message* msg_ptr, char* data_ptr);


#endif /* INC_P2P_FUNC_H_ */
