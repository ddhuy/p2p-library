/*
 * File name: stun_gvars.c
 *
 * Purpose:
 *
 * Created on: Nov 3, 2014
 *     Author: ddhuy
 *
 * File history:
 */

#include "stun_func.h"

/* =========================================================================
 * File local variables & constants
 * =========================================================================
 */

const Stun_MsgTbl g_msgtbl_binding_req[] =
{
	{ STUN_AT_RESP_ADDR,    ST_OPT, 0 },
	{ STUN_AT_CHANGE_REQ,   ST_OPT, 0 },
	{ STUN_AT_USERNAME,     ST_OPT, 0 },
	{ STUN_AT_MSG_INTEGITY, ST_OPT, 0 },
	{ STUN_AT_RESERVED,     ST_OPT, 0 }
};

const Stun_MsgTbl g_msgtbl_binding_res[] =
{
	{ STUN_AT_MAPPED_ADDR,     ST_MAND, 0 },
	{ STUN_AT_SOURCE_ADDR,     ST_MAND, 0 },
	{ STUN_AT_CHANGED_ADDR,    ST_MAND, 0 },
	{ STUN_AT_MSG_INTEGITY,    ST_OPT,  0 },
	{ STUN_AT_RESPONSE_ORIGIN, ST_OPT,  0 },
	{ STUN_AT_OTHER_ADDR,      ST_OPT,  0 },
	{ STUN_AT_XOR_MAPPED_ADDR, ST_OPT,  0 },
	{ STUN_AT_REFLECT_FROM,    ST_COND, 0 },
	{ STUN_AT_RESERVED,        ST_OPT , 0 }
};

const Stun_MsgTbl g_msgtbl_binding_err_res[] =
{
	{ STUN_AT_ERROR_CODE,   ST_MAND, 0 },
	{ STUN_AT_UNKNOWN_ATT,  ST_COND, 0 },
	{ STUN_AT_RESERVED,     ST_OPT , 0 }
};

const Stun_MsgTbl g_msgtbl_shared_secret_req[] =
{
	/* empty table */
	{ STUN_AT_RESERVED,     ST_OPT, 0 }
};

const Stun_MsgTbl g_msgtbl_shared_secret_res[] =
{
	{ STUN_AT_USERNAME, ST_MAND, 0 },
	{ STUN_AT_PASSWORD, ST_MAND, 0 },
	{ STUN_AT_RESERVED, ST_OPT , 0 }
};

const Stun_MsgTbl g_msgtbl_shared_secret_err_res[] =
{
	{ STUN_AT_ERROR_CODE,   ST_MAND, 0 },
	{ STUN_AT_UNKNOWN_ATT,  ST_COND, 0 },
	{ STUN_AT_RESERVED,     ST_OPT , 0 }
};

const Stun_MsgType_Entry g_msgtype_tbl[] =
{
	/* Message Type                  Message table                     */
	{ STUN_MT_BINDING_REQ,           &g_msgtbl_binding_req[0]           },
	{ STUN_MT_BINDING_RES,           &g_msgtbl_binding_res[0]           },
	{ STUN_MT_BINDING_ERR_RES,       &g_msgtbl_binding_err_res[0]       },
	{ STUN_MT_SHARED_SECRET_REQ,     &g_msgtbl_shared_secret_req[0]     },
	{ STUN_MT_SHARED_SECRET_RES,     &g_msgtbl_shared_secret_res[0]     },
	{ STUN_MT_SHARED_SECRET_ERR_RES, &g_msgtbl_shared_secret_err_res[0] },
};
const uint8_t g_msgtype_tbl_size = sizeof(g_msgtype_tbl) / sizeof(g_msgtype_tbl[0]);

const Stun_Attribute_Entry g_attribute_tbl[] =
{
		/* Attribute                Min. Length             Max. Length
		 * decode function          encode function         size of structure */
		{ STUN_AT_MAPPED_ADDR,     STUN_LL_MAPPED_ADDR,     STUN_LH_MAPPED_ADDR,
		  stun_att_dec_mappped_addr,        stun_att_enc_mappped_addr,        sizeof(Stun_Att_Mapped_Address)     },

   		{ STUN_AT_RESP_ADDR,       STUN_LL_RESP_ADDR,       STUN_LH_RESP_ADDR,
   		  stun_att_dec_response_addr,       stun_att_enc_response_addr,       sizeof(Stun_Att_Response_Address)   },

   		{ STUN_AT_CHANGE_REQ,      STUN_LL_CHANGE_REQ,      STUN_LH_CHANGE_REQ,
   		  stun_att_dec_change_req,          stun_att_enc_change_req,          sizeof(Stun_Att_Change_Request)     },

   		{ STUN_AT_SOURCE_ADDR,     STUN_LL_SOURCE_ADDR,     STUN_LH_SOURCE_ADDR,
   		  stun_att_dec_source_addr,         stun_att_enc_source_addr,         sizeof(Stun_Att_Source_Address)     },

   		{ STUN_AT_CHANGED_ADDR,    STUN_LL_CHANGED_ADDR,    STUN_LH_CHANGED_ADDR,
          stun_att_dec_changed_addr,        stun_att_enc_changed_addr,        sizeof(Stun_Att_Changed_Address)    },

   		{ STUN_AT_USERNAME,        STUN_LL_USERNAME,        STUN_LH_USERNAME,
   		  stun_att_dec_username,            stun_att_enc_username,            sizeof(Stun_Att_Username)           },

   		{ STUN_AT_PASSWORD,        STUN_LL_PASSWORD,        STUN_LH_PASSWORD,
   		  stun_att_dec_password,            stun_att_enc_password,            sizeof(Stun_Att_Password)           },

   		{ STUN_AT_MSG_INTEGITY,    STUN_LL_MSG_INTEGITY,    STUN_LH_MSG_INTEGITY,
		  stun_att_dec_msg_integrity,       stun_att_enc_msg_integrity,       sizeof(Stun_Att_Message_Integrity)  },

		{ STUN_AT_RESPONSE_ORIGIN, STUN_LL_RESPONSE_ORIGIN, STUN_LH_RESPONSE_ORIGIN,
		  stun_att_dec_response_origin,     stun_att_enc_response_origin,     sizeof(Stun_Att_Response_Origin)    },

		{ STUN_AT_XOR_MAPPED_ADDR, STUN_LL_XOR_MAPPED_ADDR, STUN_LH_XOR_MAPPED_ADDR,
		  stun_att_dec_xor_mapped_addr,     stun_att_enc_xor_mapped_addr,     sizeof(Stun_Att_XOR_Mapped_Address) },

		{ STUN_AT_OTHER_ADDR,      STUN_LL_OTHER_ADDR,      STUN_LH_OTHER_ADDR,
		  stun_att_dec_other_addr,          stun_att_enc_other_addr,          sizeof(Stun_Att_XOR_Mapped_Address) },

		{ STUN_AT_RESERVED,        STUN_LL_RESERVED,        STUN_LH_RESERVED,
	      NULLPTR,                 NULLPTR,                 0                              },
};
const uint8_t g_attribute_tbl_size = sizeof(g_attribute_tbl) / sizeof(g_attribute_tbl[0]);

/* =========================================================================
 * Function definitions
 * =========================================================================
 */

/*
 *
 */
const Stun_Attribute_Entry* stun_get_attribute_entry(Stun_Attribute_Type att_type)
{
	uint8_t i = 0;
	while (i < g_attribute_tbl_size)
	{
		if (att_type == g_attribute_tbl[i].att_type)
		{
			return &g_attribute_tbl[i];
		}
		i++;
	}
	return NULLPTR;
}

/*
 *
 */
const Stun_MsgType_Entry* stun_get_msgtype_entry(Stun_Message_Type msg_type)
{
	uint8_t i = 0;
	while (i < g_msgtype_tbl_size)
	{
		if (msg_type == g_msgtype_tbl[i].msg_type)
		{
			return &g_msgtype_tbl[i];
		}
		i++;
	}
	return NULLPTR;
}

/*
 * The function get_octet work same as memcpy but it is used with BIG ENDIAN copying
 */
void *octet_cpy(void *src, const void *dst, int32_t n)
{
	while(n--)
	{
		*((char*)src++) = *((char*)(dst + n));
	}
	return src;
}
