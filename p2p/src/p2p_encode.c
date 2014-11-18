/*
 * File name: p2p_encode.c
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
 * P2P Message Encoding Function
 *******************************************************/
/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 */
P2P_ErrorCause p2p_encode_msg(P2P_Message* msg_ptr, char* data_ptr)
{
	P2P_ErrorCause        rval        = P2P_EID_NO_ERROR;
	uint16_t              data_offset = 0;
	uint16_t              msg_offset  = 0;
	const P2P_MsgTbl_Ent* msgtbl_ent_ptr;
	const P2P_MsgTbl*     msgtbl_ptr;
	const P2P_IE_Ent*     ie_ent_ptr;
	P2P_Element*          ele_ptr;

	/* clean the buffer used to store the decoded data */
	memset(data_ptr, 0, sizeof(P2P_Message));

	/* get message table which contains the element existence */
	msgtbl_ent_ptr = p2p_get_msg_entry(msg_ptr->hdr.msg_type);
	if ( msgtbl_ent_ptr != NULL )
	{
		/* encode message type */
		memcpy(data_ptr + data_offset, (char*)&msg_ptr->hdr.msg_type, 2);
		data_offset += 2;
		/* skip encode message length, will be encoded later */
		data_offset += 2;

		/* start encoding each element */
		/* get the message type table which contains the needed elements */
		msgtbl_ptr     = msgtbl_ent_ptr->tbl_ptr;

		/* for each needed element type */
		while ( msgtbl_ptr->ie_type != P2P_IE_NULL )
		{
			/* point to the current element */
			ele_ptr = (P2P_Element*) ((char*)&msg_ptr->msg + msg_offset);
			/* find the encoding function */
			ie_ent_ptr = p2p_get_ie_entry(msgtbl_ptr->ie_type);

			if ( ele_ptr->dummy.ie_hdr.valid )
			{
				/* encode IE type */
				memcpy(data_ptr + data_offset, (char*)&ele_ptr->dummy.ie_hdr.ie_type, 2);
				data_offset += 2;
				/* encode IE length */
				memcpy(data_ptr + data_offset, (char*)&ele_ptr->dummy.ie_hdr.ie_len, 2);
				data_offset += 2;

				/* found the encoding function */
				if ( ie_ent_ptr != NULL && ie_ent_ptr->enc_func != NULL )
				{
					/* encode it */
					rval = ie_ent_ptr->enc_func(ele_ptr, data_ptr + data_offset);
					if ( rval != P2P_EID_NO_ERROR )
					{
						break;
					}
					/* move the offset to next needed encoding position */
					data_offset += ele_ptr->dummy.ie_hdr.ie_len;
					/* update the message len for later encoding */
					msg_ptr->hdr.msg_len += ele_ptr->dummy.ie_hdr.ie_len + 4;
				}
			}
			/* update the position of next element */
			msg_offset  += ie_ent_ptr->ele_size;
			/* go to next element */
			msgtbl_ptr++;
		}
		/* message elements encoded done */
		/* continue to encode the whole message length */
		memcpy(data_ptr + 2, (char*)&msg_ptr->hdr.msg_len, 2);
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
IE_ENC_FUNC(p2p_ie_enc_local_addr)
{
	P2P_IE_Local_Address* local_addr_ptr = &ie_ptr->local_addr;

	memcpy((char*) (data_ptr), (char*)&local_addr_ptr->family, 2);
	memcpy((char*) (data_ptr + 2), (char*)&local_addr_ptr->port, 2);
	memcpy((char*) (data_ptr + 4), (char*)&local_addr_ptr->ip, 4);

	return P2P_EID_NO_ERROR;
}


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 */
IE_ENC_FUNC(p2p_ie_enc_public_addr)
{
	P2P_IE_Public_Address* public_addr_ptr = &ie_ptr->public_addr;

	memcpy((char*) (data_ptr), (char*)&public_addr_ptr->family, 2);
	memcpy((char*) (data_ptr + 2), (char*)&public_addr_ptr->port, 2);
	memcpy((char*) (data_ptr + 4), (char*)&public_addr_ptr->ip, 4);

	return P2P_EID_NO_ERROR;
}

/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 */
IE_ENC_FUNC(p2p_ie_enc_mac_addr)
{
	P2P_IE_MAC_Address* mac_addr_ptr = &ie_ptr->mac_addr;

	memcpy((char*) (data_ptr), (char*)&mac_addr_ptr->mac_addr, 6);

	return P2P_EID_NO_ERROR;
}


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 */
IE_ENC_FUNC(p2p_ie_enc_user_info)
{
	P2P_IE_User_Info* user_info_ptr = &ie_ptr->user_info;

	memcpy(data_ptr, (char*)&user_info_ptr->user_id, 2); /* decode user id */
	memcpy(data_ptr + 2, (char*)&user_info_ptr->mac_addr, 6); /* decode the mac address */

	return P2P_EID_NO_ERROR;
}


/*
 * Purpose:
 *
 * Input:
 *
 * Output:
 */
P2P_ErrorCause p2p_ie_enc_err_cause(P2P_Element* ele_ptr, char* data_ptr)
{
	return P2P_EID_NO_ERROR;
}

