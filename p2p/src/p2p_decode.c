/*
 * File name: p2p_decode.c
 *
 * Purpose:
 *
 * Created on: Nov 5, 2014
 *     Author: ddhuy
 *
 * File history:
 */

/********************************************************
 * Headers & Include
 ********************************************************/
#include "p2p_func.h"


/*******************************************************
 * P2P Message Decoding Function
 *******************************************************/

/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 */
P2P_ErrorCause p2p_decode_msg(char* data_ptr, P2P_Message* msg_ptr)
{
	P2P_ErrorCause   rval     = P2P_EID_NO_ERROR;
	uint16_t         offset   = 0;

	const P2P_MsgTbl_Ent* msgtbl_ent_ptr;
	const P2P_MsgTbl*     msgtbl_ptr;


	/* clean the buffer used to store the decoded data */
	memset((char*) msg_ptr, 0, sizeof(P2P_Message));

	/* extract message type */
	memcpy((char*)&msg_ptr->hdr.msg_type, data_ptr + offset, 2);
	offset += 2;
	/* extract the message length */
	memcpy((char*)&msg_ptr->hdr.msg_len, data_ptr + offset, 2);
	offset += 2;

	if (P2P_MSG_NULL < msg_ptr->hdr.msg_type && msg_ptr->hdr.msg_type < P2P_MSG_COUNT)
	{
		/* get message table which contains the element existence */
		msgtbl_ent_ptr = p2p_get_msg_entry(msg_ptr->hdr.msg_type);
		if ( msgtbl_ent_ptr != NULL )
		{
			/* get the message type table which contains the needed elements */
			msgtbl_ptr = msgtbl_ent_ptr->tbl_ptr;

			if (msg_ptr->hdr.msg_len >= 4)
			{
				rval = p2p_decode_ies(data_ptr + offset, msg_ptr, msg_ptr->hdr.msg_len, msgtbl_ptr);
			}/* if msg_len >= 4 */
			else
			{
				rval = P2P_EID_MSG_LEN_ERROR;
			}
		}
	}/* if msg_type > P2P_MSG_NULL && msg_type < P2P_MSG_COUNT */
	else
	{
		rval = P2P_EID_MSG_TYPE_INVALID;
	}

	return rval;
}


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 */
P2P_ErrorCause p2p_decode_ies(char* data_ptr, P2P_Message* msg_ptr, uint8_t msg_len, const P2P_MsgTbl* msgtbl_ptr)
{
	P2P_ErrorCause rval = P2P_EID_NO_ERROR;
	uint16_t data_offset = 0, ie_offset = 0, ie_type = P2P_IE_NULL;
	int16_t ie_len;

	P2P_Element* ie_ptr;
	const P2P_IE_Ent* ie_ent_ptr;
	const P2P_MsgTbl* m_msgtbl_ptr;

	while((rval == P2P_EID_NO_ERROR) && (data_offset < msg_len))
	{
		/* decode the IE type */
		memcpy((char *)&ie_type, (char *)(data_ptr + data_offset), 2);
		data_offset += 2;
		/* decode the IE length */
		memcpy((char *)&ie_len, (char *)(data_ptr + data_offset), 2);
		data_offset += 2;

		/* get the ie entry */
		ie_ent_ptr = p2p_get_ie_entry(ie_type);
		if (ie_ent_ptr == NULL)
		{
			rval = P2P_EID_IE_TYPE_INVALID;
			continue;
		}

		m_msgtbl_ptr = msgtbl_ptr;
		ie_offset    = 0;
		while (m_msgtbl_ptr->ie_type != P2P_IE_NULL && m_msgtbl_ptr->ie_type != ie_type)
		{
			/* find the offset of ie in P2P_Message structure */
			ie_offset += ie_ent_ptr->ele_size;
			/* move to next ie in message structure */
			m_msgtbl_ptr++;
			/* get the ie entry */
			ie_ent_ptr = p2p_get_ie_entry(ie_type);
		}

		/* point to the IE data */
		ie_ptr = (P2P_Element*) ((char*)&msg_ptr->msg + ie_offset);

		if (ie_len >= ie_ent_ptr->min_size &&
			ie_len <= ie_ent_ptr->max_size &&
			ie_len + data_offset <= msg_len)
		{
			ie_ptr->dummy.ie_hdr.ie_type = ie_type;
			ie_ptr->dummy.ie_hdr.ie_len  = ie_len;

			rval = ie_ent_ptr->dec_func(data_ptr + data_offset, ie_ptr);

			data_offset += ie_ptr->dummy.ie_hdr.ie_len;
		}
		else
		{
			rval = P2P_EID_IE_LEN_ERROR;
		}
	}/* while */
	return rval;
}


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
IE_DEC_FUNC(p2p_ie_dec_local_addr)
{
	P2P_IE_Local_Address* local_addr_ptr = &ie_ptr->local_addr;

	memcpy((char*)&local_addr_ptr->family, data_ptr, 2); /* decode the address family */
	memcpy((char*)&local_addr_ptr->port, data_ptr + 2, 2); /* decode port */
	memcpy((char*)&local_addr_ptr->ip, data_ptr + 4, 4); /* decode ip address */

	local_addr_ptr->ie_hdr.valid = TRUE;

	return P2P_EID_NO_ERROR;
}


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
IE_DEC_FUNC(p2p_ie_dec_public_addr)
{
	P2P_IE_Public_Address* public_addr_ptr = &ie_ptr->public_addr;

	memcpy((char*)&public_addr_ptr->family, data_ptr, 2); /* decode the address family */
	memcpy((char*)&public_addr_ptr->port, data_ptr + 2, 2); /* decode port */
	memcpy((char*)&public_addr_ptr->ip, data_ptr + 4, 4); /* decode ip address */

	public_addr_ptr->ie_hdr.valid = TRUE;

	return P2P_EID_NO_ERROR;
}

/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
IE_DEC_FUNC(p2p_ie_dec_mac_addr)
{
	P2P_IE_MAC_Address* mac_addr_ptr = &ie_ptr->mac_addr;

	memcpy((char*)&mac_addr_ptr->mac_addr, data_ptr, 6); /* decode the address family */

	mac_addr_ptr->ie_hdr.valid = TRUE;

	return P2P_EID_NO_ERROR;
}


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
IE_DEC_FUNC(p2p_ie_dec_user_info)
{
	P2P_IE_User_Info* user_info_ptr = &ie_ptr->user_info;

	memcpy((char*)&user_info_ptr->user_id, data_ptr, 2); /* decode user id */
	memcpy((char*)&user_info_ptr->mac_addr, data_ptr + 2, 6); /* decode the address family */

	user_info_ptr->ie_hdr.valid = TRUE;

	return P2P_EID_NO_ERROR;
}


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 *
 */
P2P_ErrorCause p2p_ie_dec_err_cause(char* data_ptr, P2P_Element* ele_ptr)
{
	return P2P_EID_NO_ERROR;
}
