/*
 * File name: stun_encode.c
 *
 * Purpose:
 *
 * Created on: Nov 3, 2014
 *     Author: ddhuy
 *
 * File history:
 */

/***************************************************************
 *
 * Include & header
 *
 ***************************************************************/
#include <string.h>

#include "stun_const.h"
#include "stun_func.h"

/***************************************************************
 *
 * Type definition and Data structures
 *
 ***************************************************************/


/***************************************************************
 *
 * File functions
 *
 ***************************************************************/

/*
 *
 */
Stun_ErrorCause stun_encode_message
(
	Stun_Message*       msg_ptr,   /* in: pointer to stored decoded message  */
	char*               data_ptr   /* out:  pointer to decoded message         */
)
{
	const Stun_MsgTbl* msgtbl_ptr;
	const Stun_Attribute_Entry* att_ent_ptr;
	Stun_Attribute* att_ptr;
	uint8_t data_offset = 0, msg_offset = 0, i = 0;
	char* len_ptr;

	/* encode message header*/
	for (i = 0; i < g_msgtype_tbl_size; ++i)
	{
		if (g_msgtype_tbl[i].msg_type == msg_ptr->header.msg_type)
		{
			/* shortcut to the message table */
			msgtbl_ptr = g_msgtype_tbl[i].msg_tbl;

			/* encode the message type */
			octet_cpy(data_ptr, (char*) &msg_ptr->header.msg_type, 2);
			data_offset += 2;

			/* we do not know exactly message length at this time
			 * but after encoding completely the attribute we can use
			 * the data_offset as message length
			 */
			len_ptr = data_ptr + data_offset; /* save the length position for later encoding */
			data_offset += 2;

			/* encode the magic cookie */
			octet_cpy(data_ptr + data_offset, (char*) &msg_ptr->header.magic_cookie, 4);
			data_offset += 4;

			/* encode the transaction id */
			octet_cpy(data_ptr + data_offset, (char*) &msg_ptr->header.transaction_id, 12);
			data_offset += 12;

			/* start position of attribute content */
			msg_offset += sizeof(Stun_Header);

			/* Start decoding the attribute content */
			while (msgtbl_ptr->att_type != STUN_AT_RESERVED)
			{
				att_ptr = (Stun_Attribute*) (((char*) msg_ptr) + msg_offset);

				att_ent_ptr = stun_get_attribute_entry(msgtbl_ptr->att_type);

				if (att_ent_ptr == NULL)
				{
					return STUN_EID_INVALID_ATTRIBUTE_TYPE;
				}

				if (att_ptr->dummy.header.valid)
				{
					if (att_ent_ptr != NULLPTR)
					{
						/* encode attribute type */
						octet_cpy(data_ptr + data_offset, (char*) &att_ptr->dummy.header.att_type, 2);
						data_offset += 2;
						/* encode attribute type */
						octet_cpy(data_ptr + data_offset, (char*) &att_ptr->dummy.header.att_len, 2);
						data_offset += 2;
						/* encode attribute content by calling its own function */
						att_ent_ptr->enc_ptr(att_ptr, data_ptr + data_offset);
						data_offset += att_ptr->dummy.header.att_len;
						/* update the message length */
						msg_ptr->header.msg_len += att_ptr->dummy.header.att_len + 4;
					}
					else
					{
						return STUN_EID_INVALID_ATTRIBUTE_TYPE;
					}
				}
				else
				{
					if (msgtbl_ptr->att_pre == ST_MAND)
					{
						return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
					}
				}
				/* move to offset of next attribute */
				msg_offset += att_ent_ptr->size;
				/* increase to next attribute entry */
				msgtbl_ptr++;
			}

			/* completely encoding the attribute content,
			 * now we get the message length via data_offset */
			octet_cpy(len_ptr, (char*) &msg_ptr->header.msg_len, 2);

			break;
		}
	}

	/* the message type does not exist in table: return false!!!  */
	if (g_msgtype_tbl_size == i)
	{
		return STUN_EID_INVALID_MESSAGE_TYPE;
	}

	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_mappped_addr)
{
	Stun_Att_Mapped_Address* mapped_addr_ptr = &att_ptr->mapped_addr;

	octet_cpy((char*) (data_ptr), (char*)&mapped_addr_ptr->addr_family, 2);
	octet_cpy((char*) (data_ptr + 2), (char*)&mapped_addr_ptr->port_number, 2);
	octet_cpy((char*) (data_ptr + 4), (char*)&mapped_addr_ptr->ip_address, 4);
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_response_addr)
{
	Stun_Att_Response_Address* response_addr_ptr = &att_ptr->response_addr;

	octet_cpy((char*) (data_ptr), (char*)&response_addr_ptr->addr_family, 2);
	octet_cpy((char*) (data_ptr + 2), (char*)&response_addr_ptr->port_number, 2);
	octet_cpy((char*) (data_ptr + 4), (char*)&response_addr_ptr->ip_address, 4);
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_change_req)
{
	Stun_Att_Change_Request* change_req_ptr = &att_ptr->change_req;

	octet_cpy((char*) (data_ptr), (char*)&change_req_ptr->change_ip_port, 4);
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_source_addr)
{
	Stun_Att_Source_Address* source_addr_ptr = &att_ptr->source_addr;

	octet_cpy((char*) (data_ptr), (char*)&source_addr_ptr->addr_family, 2);
	octet_cpy((char*) (data_ptr + 2), (char*)&source_addr_ptr->port_number, 2);
	octet_cpy((char*) (data_ptr + 4), (char*)&source_addr_ptr->ip_address, 4);
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_changed_addr)
{
	Stun_Att_Changed_Address* changed_addr_ptr = &att_ptr->changed_addr;

	octet_cpy((char*) (data_ptr), (char*)&changed_addr_ptr->addr_family, 2);
	octet_cpy((char*) (data_ptr + 2), (char*)&changed_addr_ptr->port_number, 2);
	octet_cpy((char*) (data_ptr + 4), (char*)&changed_addr_ptr->ip_address, 4);
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_username)
{
	Stun_Att_Username* username_ptr = &att_ptr->username;

	octet_cpy((char*) (data_ptr), (char*)&username_ptr->username, att_ptr->username.header.att_len);
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_password)
{
	Stun_Att_Password* password_ptr = &att_ptr->password;

	octet_cpy((char*) (data_ptr), (char*)&password_ptr->password, att_ptr->password.header.att_len);
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_msg_integrity)
{
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_error_code)
{
	Stun_Att_Error_Code* err_code_ptr = &att_ptr->error_code;

	octet_cpy((char*)&err_code_ptr->error_code, (char*) (data_ptr), 4);

	if (att_ptr->error_code.header.att_len <= strlen(err_code_ptr->reason) + 4)
	{
		strcpy(err_code_ptr->reason, (data_ptr + 4));
	}
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_unknown_att)
{
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_reflect_from)
{
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_realm)
{
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_nonce)
{
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_fingerprint)
{
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_xor_mapped_addr)
{
	Stun_Att_XOR_Mapped_Address* xor_mapped_addr_ptr = &att_ptr->xor_mapped_addr;

	uint16_t port = xor_mapped_addr_ptr->port_number ^ STUN_MAGIC_COOKIE_XOR_PORT;
	uint32_t   ip   = xor_mapped_addr_ptr->ip_address ^ STUN_MAGIC_COOKIE;

	octet_cpy((char*) (data_ptr), (char*)&xor_mapped_addr_ptr->addr_family, 2);
	octet_cpy((char*) (data_ptr + 2), (char*)&port, 2);
	octet_cpy((char*) (data_ptr + 4), (char*)&ip, 4);
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_padding)
{
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_response_port)
{
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_software)
{
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_alternative_server)
{
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_cache_timeout)
{
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_response_origin)
{
	Stun_Att_Response_Origin* res_origin_ptr = &att_ptr->response_origin;

	octet_cpy((char*) (data_ptr), (char*)&res_origin_ptr->addr_family, 2);
	octet_cpy((char*) (data_ptr + 2), (char*)&res_origin_ptr->port_number, 2);
	octet_cpy((char*) (data_ptr + 4), (char*)&res_origin_ptr->ip_address, 4);
}

/*
 *
 */
ATT_ENC_FUNC(stun_att_enc_other_addr)
{
	Stun_Att_Other_Address* other_addr_ptr = &att_ptr->other_addr;

	octet_cpy((char*) (data_ptr), (char*)&other_addr_ptr->addr_family, 2);
	octet_cpy((char*) (data_ptr + 2), (char*)&other_addr_ptr->port_number, 2);
	octet_cpy((char*) (data_ptr + 4), (char*)&other_addr_ptr->ip_address, 4);
}
