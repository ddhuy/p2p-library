/*
 * File name: p2p_gvars.c
 *
 * Purpose:
 *
 * Created on: Nov 5, 2014
 *     Author: ddhuy
 *
 * File history:
 */

/***************************************************************
 *
 * File headers & includes
 *
 ***************************************************************/
#include <memory.h>

#include "p2p_func.h"


/***************************************************************
 *
 * Message Table
 *
 ***************************************************************/
const P2P_MsgTbl g_msg_reg_req[] =
{
	{ P2P_IE_USER_INFO,      0 },
	{ P2P_IE_LOCAL_ADDRESS,  0 },
	{ P2P_IE_NULL,           0 },
};

const P2P_MsgTbl g_msg_reg_res[] =
{
	{ P2P_IE_USER_INFO, 0 },
	{ P2P_IE_NULL,      0 },
};

const P2P_MsgTbl g_msg_conn_req[] =
{
	{ P2P_IE_USER_INFO,      0 },
	{ P2P_IE_LOCAL_ADDRESS,  0 },
	{ P2P_IE_PUBLIC_ADDRESS, 0 },
	{ P2P_IE_NULL,           0 },
};

const P2P_MsgTbl g_msg_conn_res[] =
{
	{ P2P_IE_USER_INFO,      0 },
	{ P2P_IE_LOCAL_ADDRESS,  0 },
	{ P2P_IE_PUBLIC_ADDRESS, 0 },
	{ P2P_IE_NULL,           0 },
};

const P2P_MsgTbl g_msg_hand_shk[] =
{
	{ P2P_IE_USER_INFO, 0 },
	{ P2P_IE_USER_INFO, 0 },
	{ P2P_IE_NULL,      0 },
};

const P2P_MsgTbl g_msg_hshk_ack[] =
{
	{ P2P_IE_USER_INFO, 0 },
	{ P2P_IE_USER_INFO, 0 },
	{ P2P_IE_NULL,      0 },
};

/***************************************************************
 *
 * Message Table Entry
 *
 ***************************************************************/
const P2P_MsgTbl_Ent g_msgtbl[] =
{
	{ P2P_MSG_REGISTRATION_REQUEST,   &g_msg_reg_req[0],  },
	{ P2P_MSG_REGISTRATION_RESPONSE,  &g_msg_reg_res[0],  },
	{ P2P_MSG_CONNECTION_REQUEST,     &g_msg_conn_req[0], },
	{ P2P_MSG_CONNECTION_RESPONSE,    &g_msg_conn_res[0], },
	{ P2P_MSG_HANDSHAKE,              &g_msg_hand_shk[0], },
	{ P2P_MSG_HANDSHAKE_ACK,          &g_msg_hshk_ack[0], },
	{ P2P_MSG_NULL,                   0,                  },

};
const uint16_t g_msgtbl_size = sizeof(g_msgtbl) / sizeof(g_msgtbl[0]);


/***************************************************************
 *
 * Element Table
 *
 ***************************************************************/
const P2P_IE_Ent g_ieitbl[] =
{
	{
		P2P_IE_LOCAL_ADDRESS,   sizeof(P2P_IE_Local_Address),  P2P_LL_LOCAL_ADDRESS,  P2P_LH_LOCAL_ADDRESS,
		p2p_ie_dec_local_addr, p2p_ie_enc_local_addr
	},
	{
		P2P_IE_PUBLIC_ADDRESS,  sizeof(P2P_IE_Public_Address), P2P_LL_PUBLIC_ADDRESS, P2P_LH_PUBLIC_ADDRESS,
		p2p_ie_dec_public_addr, p2p_ie_enc_public_addr
	},
	{
		P2P_IE_MAC_ADDRESS   ,  sizeof(P2P_IE_MAC_Address),    P2P_LL_MAC_ADDRESS,    P2P_LH_MAC_ADDRESS,
		p2p_ie_dec_mac_addr,    p2p_ie_enc_mac_addr
	},
	{
		P2P_IE_USER_INFO,       sizeof(P2P_IE_User_Info),      P2P_LL_USER_INFO,      P2P_LH_USER_INFO,
		p2p_ie_dec_user_info,   p2p_ie_enc_user_info
	},
	{
		P2P_IE_ERROR_CAUSE,     sizeof(P2P_IE_Error_Cause),    P2P_LL_ERROR_CAUSE,    P2P_LH_ERROR_CAUSE,
		p2p_ie_dec_err_cause,   p2p_ie_enc_err_cause
	},
	{
		P2P_IE_NULL, 0, 0, 0, 0
	},
};
const uint16_t g_ieitbl_size = sizeof(g_ieitbl) / sizeof(g_ieitbl[0]);



/***************************************************************
 *
 * Miscellaneous functions
 *
 ***************************************************************/


/*
 * Purpose:
 * -
 * Input:
 * -
 * Output:
 * -
 */
const P2P_MsgTbl_Ent* p2p_get_msg_entry(P2P_Message_Type msg_type)
{
	uint8_t idx = 0;
	while (g_msgtbl[idx].msg_type != P2P_MSG_NULL)
	{
		if (msg_type == g_msgtbl[idx].msg_type)
		{
			return &g_msgtbl[idx];
		}
		idx += 1;
	}
	return NULL;
}

/*
 * Purpose:
 * -
 * Input:
 * -
 * Output:
 * -
 */
const P2P_IE_Ent* p2p_get_ie_entry(P2P_Element_Type ie_type)
{
	uint8_t idx = 0;
	while (g_ieitbl[idx].ele_type != P2P_IE_NULL)
	{
		if (ie_type == g_ieitbl[idx].ele_type)
		{
			return &g_ieitbl[idx];
		}
		idx += 1;
	}
	return NULL;
}
