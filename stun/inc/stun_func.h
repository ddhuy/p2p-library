/*
 * File name: stun_func.h
 *
 * Purpose:
 *
 * Created on: Nov 3, 2014
 *     Author: ddhuy
 *
 * File history:
 */


#ifndef STUN_FUNC_H_
#define STUN_FUNC_H_

/***************************************************************
 *
 * Include
 *
 ***************************************************************/
#include <stdint.h>

#include "stun_message.h"

/***************************************************************
 *
 * Macros and Preprocessors define
 *
 ***************************************************************/
#define TRUE  (1)
#define FALSE (0)

#define NULLPTR (0)

#define GP_GETOCTET(word_ptr, oct_offset)     (*((const uint8_t*)(word_ptr) + (oct_offset)))

/***************************************************************
 *
 * Type definition and Data structures
 *
 ***************************************************************/
typedef enum
{
	STUN_EID_NO_ERROR,
	STUN_EID_INVALID_MESSAGE_TYPE,
	STUN_EID_INVALID_MESSAGE_CONTENT,
	STUN_EID_INVALID_MESSAGE_LENGTH,
	STUN_EID_INVALID_ATTRIBUTE_TYPE,
	STUN_EID_INVALID_ATTRIBUTE_CONTENT,
	STUN_EID_INVALID_ATTRIBUTE_LENGTH,
	STUN_EID_INVALID_MAGIC_COOKIE
} Stun_ErrorCause;

/* STUN Attribute type presence:                               */
typedef enum
{
   ST_MAND         = 0, /* Mandatory Attribute   */
   ST_OPT          = 1, /* Optional Attribute    */
   ST_COND         = 2  /* Conditional Attribute */
} Stun_Att_Presence;

/* Message Table */
typedef struct
{
	Stun_Attribute_Type att_type;
	Stun_Att_Presence   att_pre;
	uint16_t            att_offset;
} Stun_MsgTbl;


/* Message Type Entry                                                    */
typedef struct
{
	Stun_Message_Type   msg_type;   /* Message Type                       */
	const Stun_MsgTbl*  msg_tbl;    /* pointer to message table           */
} Stun_MsgType_Entry;
/* export all message table */
extern const Stun_MsgType_Entry g_msgtype_tbl[];
extern const uint8_t            g_msgtype_tbl_size;


/* Attribute Table                                                       */
typedef Stun_ErrorCause (*Dec_Func_Ptr)(char* data_ptr, Stun_Attribute* att_ptr);
typedef void      (*Enc_Func_Ptr)(Stun_Attribute* att_ptr, char* data_ptr);
typedef struct
{
	Stun_Attribute_Type    att_type; /* Attribute Type                   */
	uint8_t                min_len;  /* minimum length of attribute      */
	uint8_t                max_len;  /* maximum length of attribute      */
	Dec_Func_Ptr           dec_ptr;  /* pointer to decode function       */
	Enc_Func_Ptr           enc_ptr;  /* pointer to encode function       */
	uint32_t               size ;    /* size of IE structure             */
} Stun_Attribute_Entry;
/* export all attribute table */
extern const Stun_Attribute_Entry g_attribute_tbl[];
extern const uint8_t              g_attribute_tbl_size;


/***************************************************************
 *
 * Miscellaneous Functions prototypes
 *
 ***************************************************************/
/* the function get_octet work same as memcpy but it is used BIG ENDIAN copying */
void *octet_cpy(void *dest, const void *src, int32_t n);

/* find & return attribute type entry */
const Stun_Attribute_Entry* stun_get_attribute_entry(Stun_Attribute_Type att_type);

/* find & return message type entry */
const Stun_MsgType_Entry* stun_get_msgtype_entry(Stun_Message_Type msg_type);

/***************************************************************
 *
 * Decode Functions prototypes
 *
 ***************************************************************/
Stun_ErrorCause stun_decode_message
(
	char*               data_ptr, /* in:  pointer to decoded message         */
	Stun_Message*       msg_ptr   /* out: pointer to stored decoded message  */
);

Stun_ErrorCause stun_decode_attributes
(
	char*               data_ptr, /* in:  pointer to decoded message         */
	Stun_Message*       msg_ptr,  /* out: pointer to stored decoded message  */
	uint16_t            msg_len   /* in:  length of stun message             */
);

#define ATT_DEC_FUNC(func_name)   \
Stun_ErrorCause func_name          \
(                                 \
	char*               data_ptr, \
	Stun_Attribute*     att_ptr   \
)
ATT_DEC_FUNC(stun_att_dec_mappped_addr);
ATT_DEC_FUNC(stun_att_dec_response_addr);
ATT_DEC_FUNC(stun_att_dec_change_req);
ATT_DEC_FUNC(stun_att_dec_source_addr);
ATT_DEC_FUNC(stun_att_dec_changed_addr);
ATT_DEC_FUNC(stun_att_dec_username);
ATT_DEC_FUNC(stun_att_dec_password);
ATT_DEC_FUNC(stun_att_dec_msg_integrity);
ATT_DEC_FUNC(stun_att_dec_error_code);
ATT_DEC_FUNC(stun_att_dec_unknown_att);
ATT_DEC_FUNC(stun_att_dec_reflect_from);
ATT_DEC_FUNC(stun_att_dec_realm);
ATT_DEC_FUNC(stun_att_dec_nonce);
ATT_DEC_FUNC(stun_att_dec_xor_mapped_addr);
ATT_DEC_FUNC(stun_att_dec_padding);
ATT_DEC_FUNC(stun_att_dec_response_port);
ATT_DEC_FUNC(stun_att_dec_software);
ATT_DEC_FUNC(stun_att_dec_alternative_server);
ATT_DEC_FUNC(stun_att_dec_cache_timeout);
ATT_DEC_FUNC(stun_att_dec_fingerprint);
ATT_DEC_FUNC(stun_att_dec_response_origin);
ATT_DEC_FUNC(stun_att_dec_other_addr);


/***************************************************************
 *
 * Encode Functions
 *
 ***************************************************************/
Stun_ErrorCause stun_encode_message
(
	Stun_Message*       msg_ptr,   /* out: pointer to stored decoded message  */
	char*               data_ptr /* in:  pointer to decoded message         */
);

#define ATT_ENC_FUNC(func_name)  \
void func_name                   \
(                                \
	Stun_Attribute*     att_ptr, \
	char*               data_ptr \
)
ATT_ENC_FUNC(stun_att_enc_mappped_addr);
ATT_ENC_FUNC(stun_att_enc_response_addr);
ATT_ENC_FUNC(stun_att_enc_change_req);
ATT_ENC_FUNC(stun_att_enc_source_addr);
ATT_ENC_FUNC(stun_att_enc_changed_addr);
ATT_ENC_FUNC(stun_att_enc_username);
ATT_ENC_FUNC(stun_att_enc_password);
ATT_ENC_FUNC(stun_att_enc_msg_integrity);
ATT_ENC_FUNC(stun_att_enc_error_code);
ATT_ENC_FUNC(stun_att_enc_unknown_att);
ATT_ENC_FUNC(stun_att_enc_reflect_from);
ATT_ENC_FUNC(stun_att_enc_realm);
ATT_ENC_FUNC(stun_att_enc_nonce);
ATT_ENC_FUNC(stun_att_enc_xor_mapped_addr);
ATT_ENC_FUNC(stun_att_enc_padding);
ATT_ENC_FUNC(stun_att_enc_response_port);
ATT_ENC_FUNC(stun_att_enc_software);
ATT_ENC_FUNC(stun_att_enc_alternative_server);
ATT_ENC_FUNC(stun_att_enc_cache_timeout);
ATT_ENC_FUNC(stun_att_enc_fingerprint);
ATT_ENC_FUNC(stun_att_enc_response_origin);
ATT_ENC_FUNC(stun_att_enc_other_addr);

#endif /* STUN_FUNC_H_ */
