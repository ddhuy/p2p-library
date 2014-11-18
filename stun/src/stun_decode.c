/*
 * File name: stun_decode.c
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
 * Include
 *
 ***************************************************************/
#include <string.h>
#include <netinet/in.h>

#include "stun_const.h"
#include "stun_func.h"

/***************************************************************
 *
 * Type definition and Data structures
 *
 ***************************************************************/


/***************************************************************
 *
 * Function definition
 *
 ***************************************************************/
Stun_ErrorCause stun_decode_message(char* data_ptr, Stun_Message* msg_ptr)
{
	/* declare local variables */
	Stun_ErrorCause rval = STUN_EID_NO_ERROR; /* the return value when decoding STUN message */
	int32_t   idx  = 0;      /* position of byte in raw data to begin decoding */
	uint8_t   i    = 0;

	/* clear the msg buffer */
	memset((char*) msg_ptr, 0, sizeof(Stun_Message));

	/* extract the message type: 2 bytes */
	octet_cpy((char*) &msg_ptr->header.msg_type, (char*) data_ptr, 2);
	idx += 2; /* increase the position for length decoding */

	/* Loop over the table to find the checking function for current message type */
	for (i = 0; i < g_msgtype_tbl_size; ++i)
	{
		/* find one valid checking function */
		if (g_msgtype_tbl[i].msg_type == msg_ptr->header.msg_type)
		{
			/* extract the message length: 2 bytes */
			octet_cpy((char*) &msg_ptr->header.msg_len, (char*) (data_ptr + idx), 2);
			idx += 2;
			if (msg_ptr->header.msg_len >= 0)
			{
				/* extract the magic cookie: 4 bytes */
				octet_cpy((char*) &msg_ptr->header.magic_cookie, (char*) (data_ptr + idx), 4);
				idx += 4;
				/* extract the transaction ID: 12 bytes */
				octet_cpy((char*) &msg_ptr->header.transaction_id, (char*) (data_ptr + idx), 12);
				idx += 12;
				/* continue to extract the stun attributes */
				rval = stun_decode_attributes(data_ptr + idx, msg_ptr, msg_ptr->header.msg_len);
			}
			else
			{
				rval = STUN_EID_INVALID_MESSAGE_LENGTH;
			}
			break;
		}
	}
	/* check the message type is valid or not */
	if (g_msgtype_tbl_size == i)
	{
		/* Cannot find the message type! FATAL ERROR!!! */
		rval = STUN_EID_INVALID_MESSAGE_TYPE;
	}

	return rval;
}

Stun_ErrorCause stun_decode_attributes(char* data_ptr, Stun_Message* msg_ptr, uint16_t msg_len)
{
	Stun_ErrorCause rval          = STUN_EID_NO_ERROR;  /* return value */
	Stun_Attribute* att_ptr       = NULLPTR; /* pointer point to STUN attribute for decoding */
	uint32_t        data_offset   = 0;       /* position of decode byte in raw data */
	uint32_t        att_offset    = 0;

	const Stun_Attribute_Entry* att_entry_ptr; /* get the attribute entry */
	Stun_Attribute_Type    att_type; /* shortcut to attribute type in STUN message */

	/* begin decoding each attribute in STUN message */
	while ((rval == STUN_EID_NO_ERROR) && (data_offset < msg_len))
	{
		/* point to the begin offset of all attributes in STUN message */
		att_ptr = (Stun_Attribute*) ((char*)(&msg_ptr->data) + att_offset);

		/* extract the attribute type: 2 bytes */
		octet_cpy((char*) &att_ptr->dummy.header.att_type, (char*) (data_ptr + data_offset), 2);
		data_offset += 2;
		/* get the shortcut */
		att_type = att_ptr->dummy.header.att_type;

		/* extract the attribute length: 2 bytes */
		octet_cpy((char*) &att_ptr->dummy.header.att_len, (char*) (data_ptr + data_offset), 2);
		data_offset += 2;

		/* start decoding attribute content */
		if ((STUN_COMPREHENSION_REQUIRED_1ST_L <= att_type && att_type < STUN_COMPREHENSION_REQUIRED_1ST_H) ||
			(STUN_COMPREHENSION_REQUIRED_2ND_L <= att_type && att_type < STUN_COMPREHENSION_REQUIRED_2ND_H) ||
			(STUN_COMPREHENSION_OPTIONAL_1ST_L <= att_type && att_type < STUN_COMPREHENSION_OPTIONAL_1ST_H) ||
			(STUN_COMPREHENSION_OPTIONAL_2ND_L <= att_type && att_type < STUN_COMPREHENSION_OPTIONAL_2ND_H))
		{
			if (att_ptr->dummy.header.att_len + data_offset <= msg_len)
			{
				att_entry_ptr = stun_get_attribute_entry(att_type);
				if (att_entry_ptr != NULLPTR)
				{
					att_offset += att_entry_ptr->size;
					if (att_entry_ptr->dec_ptr != NULLPTR)
					{
						rval = att_entry_ptr->dec_ptr(data_ptr + data_offset, att_ptr);
						data_offset += att_ptr->dummy.header.att_len;
					}
				}
				else
				{
					rval = STUN_EID_INVALID_ATTRIBUTE_TYPE;
				}
			}
			else
			{
				rval = STUN_EID_INVALID_ATTRIBUTE_LENGTH;
			}
		}
		else
		{
			rval = STUN_EID_INVALID_ATTRIBUTE_TYPE;
		}
	}

	return rval;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_mappped_addr)
{
	Stun_Att_Mapped_Address* mapped_addr_ptr = &att_ptr->mapped_addr;

	octet_cpy((char*)&mapped_addr_ptr->addr_family, (char*) (data_ptr), 2);
	octet_cpy((char*)&mapped_addr_ptr->port_number, (char*) (data_ptr + 2), 2);
	if (mapped_addr_ptr->addr_family == STUN_IPV4_ADDRESS)
	{
		octet_cpy((char*)&mapped_addr_ptr->ip_address, (char*) (data_ptr + 4), 4);
	}
	else if (mapped_addr_ptr->addr_family == STUN_IPV6_ADDRESS) /* not supported for IPv6 */
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	}
	else
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	}

	mapped_addr_ptr->header.valid = TRUE;

	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_response_addr)
{
	Stun_Att_Response_Address* response_addr_ptr = &att_ptr->response_addr;

	octet_cpy((char*)&response_addr_ptr->addr_family, (char*) (data_ptr), 2);
	octet_cpy((char*)&response_addr_ptr->port_number, (char*) (data_ptr + 2), 2);
	if (response_addr_ptr->addr_family == STUN_IPV4_ADDRESS)
	{
		octet_cpy((char*)&response_addr_ptr->ip_address,  (char*) (data_ptr + 4), 4);
	}
	else if (response_addr_ptr->addr_family == STUN_IPV6_ADDRESS) /* not supported for IPv6 */
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	}
	else
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	};

	response_addr_ptr->header.valid = TRUE;

	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_change_req)
{
	Stun_Att_Change_Request* change_req_ptr = &att_ptr->change_req;

	octet_cpy((char*)&change_req_ptr->change_ip_port, (char*) (data_ptr), 4);
	change_req_ptr->change_ip_port &= (STUN_CHANGE_ADDR | STUN_CHANGE_PORT);

	change_req_ptr->header.valid = TRUE;

	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_source_addr)
{
	Stun_Att_Source_Address* source_addr_ptr = &att_ptr->source_addr;

	octet_cpy((char*)&source_addr_ptr->addr_family, (char*) (data_ptr), 2);
	octet_cpy((char*)&source_addr_ptr->port_number, (char*) (data_ptr + 2), 2);
	if (source_addr_ptr->addr_family == STUN_IPV4_ADDRESS)
	{
		octet_cpy((char*)&source_addr_ptr->ip_address,  (char*) (data_ptr + 4), 4);
	}
	else if (source_addr_ptr->addr_family == STUN_IPV6_ADDRESS) /* not supported for IPv6 */
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	}
	else
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	};

	source_addr_ptr->header.valid = TRUE;

	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_changed_addr)
{
	Stun_Att_Changed_Address* changed_addr_ptr = &att_ptr->changed_addr;

	octet_cpy((char*)&changed_addr_ptr->addr_family, (char*) (data_ptr), 2);
	octet_cpy((char*)&changed_addr_ptr->port_number, (char*) (data_ptr + 2), 2);
	if (changed_addr_ptr->addr_family == STUN_IPV4_ADDRESS)
	{
		octet_cpy((char*)&changed_addr_ptr->ip_address,  (char*) (data_ptr + 4), 4);
	}
	else if (changed_addr_ptr->addr_family == STUN_IPV6_ADDRESS) /* not supported for IPv6 */
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	}
	else
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	};

	changed_addr_ptr->header.valid = TRUE;

	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_username)
{
	Stun_Att_Username* username_ptr = &att_ptr->username;
	if (att_ptr->username.header.att_len < strlen(username_ptr->username))
	{
		octet_cpy((char*)&username_ptr->username, (char*) (data_ptr), att_ptr->username.header.att_len);
	}
	else
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	}

	username_ptr->header.valid = TRUE;

	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_password)
{
	Stun_Att_Password* password_ptr = &att_ptr->password;
	if (att_ptr->username.header.att_len <= strlen(password_ptr->password))
	{
		octet_cpy((char*)&password_ptr->password, (char*) (data_ptr), att_ptr->username.header.att_len);
	}
	else
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	}

	password_ptr->header.valid = TRUE;

	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_msg_integrity)
{
	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_error_code)
{
	Stun_Att_Error_Code* err_code_ptr = &att_ptr->error_code;

	octet_cpy((char*)&err_code_ptr->error_code, (char*) (data_ptr), 4);

	if (att_ptr->error_code.header.att_len <= strlen(err_code_ptr->reason) + 4)
	{
		strcpy(err_code_ptr->reason, (data_ptr + 4));
	}
	else
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	}

	err_code_ptr->header.valid = TRUE;

	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_unknown_att)
{
	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_reflect_from)
{
	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_realm)
{
	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_nonce)
{
	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_fingerprint)
{
	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_padding)
{
	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_response_port)
{
	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_software)
{
	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_alternative_server)
{
	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_cache_timeout)
{
	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_response_origin)
{
	Stun_Att_Response_Origin* res_origin_ptr = &att_ptr->response_origin;

	octet_cpy((char*)&res_origin_ptr->addr_family, (char*) (data_ptr), 2);
	octet_cpy((char*)&res_origin_ptr->port_number, (char*) (data_ptr + 2), 2);
	if (res_origin_ptr->addr_family == STUN_IPV4_ADDRESS)
	{
		octet_cpy((char*)&res_origin_ptr->ip_address,  (char*) (data_ptr + 4), 4);
	}
	else if (res_origin_ptr->addr_family == STUN_IPV6_ADDRESS) /* not supported for IPv6 */
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	}
	else
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	};

	res_origin_ptr->header.valid = TRUE;

	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_other_addr)
{
	Stun_Att_Other_Address* other_addr_ptr = &att_ptr->other_addr;

	octet_cpy((char*)&other_addr_ptr->addr_family, (char*) (data_ptr), 2);
	octet_cpy((char*)&other_addr_ptr->port_number, (char*) (data_ptr + 2), 2);
	if (other_addr_ptr->addr_family == STUN_IPV4_ADDRESS)
	{
		octet_cpy((char*)&other_addr_ptr->ip_address,  (char*) (data_ptr + 4), 4);
	}
	else if (other_addr_ptr->addr_family == STUN_IPV6_ADDRESS) /* not supported for IPv6 */
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	}
	else
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	};

	other_addr_ptr->header.valid = TRUE;

	return STUN_EID_NO_ERROR;
}

/*
 *
 */
ATT_DEC_FUNC(stun_att_dec_xor_mapped_addr)
{
	uint32_t port;
	uint32_t ip;

	Stun_Att_XOR_Mapped_Address* xor_mapped_addr_ptr = &att_ptr->xor_mapped_addr;

	/* Address Family is not XORed, decode normally */
	octet_cpy((char*)&xor_mapped_addr_ptr->addr_family, (char*) (data_ptr), 2);
	/* Port is XORed, need to copy to an temporary variable and then decode later */
	octet_cpy((char*)&port, (char*) (data_ptr + 2), 2);

	if (xor_mapped_addr_ptr->addr_family == STUN_IPV4_ADDRESS)
	{
		/* IP is XORed, need to copy to an temporary variable and then decode later */
		octet_cpy((char*)&ip,  (char*) (data_ptr + 4), 4);
	}
	else if (xor_mapped_addr_ptr->addr_family == STUN_IPV6_ADDRESS) /* not supported for IPv6 */
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	}
	else
	{
		return STUN_EID_INVALID_ATTRIBUTE_CONTENT;
	};

	/* Start decoding IP & Port */
	xor_mapped_addr_ptr->port_number = ntohs(port) | STUN_MAGIC_COOKIE_XOR_PORT;
	xor_mapped_addr_ptr->ip_address  = ntohl(ip) | STUN_MAGIC_COOKIE;

	xor_mapped_addr_ptr->header.valid = TRUE;

	return STUN_EID_NO_ERROR;
}

