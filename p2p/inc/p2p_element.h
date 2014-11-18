/*
 * File name: p2p_element.h
 *
 * Purpose:
 *
 * Created on: Nov 5, 2014
 *     Author: ddhuy
 *
 * File history:
 */


#ifndef INC_P2P_ELEMENT_H_
#define INC_P2P_ELEMENT_H_

#include <stdint.h>

#define TRUE  (1)
#define FALSE (0)

#define P2P_IE_HEADER_SIZE 4

/* IE Header */
typedef struct
{
	uint16_t ie_type;
	uint16_t ie_len;
	uint32_t valid;
} P2P_IE_Header;


/* IE Local Address */
typedef struct
{
	P2P_IE_Header ie_hdr;
	uint16_t      family;
	uint16_t      port;
	uint32_t      ip;
} P2P_IE_Local_Address;

/* IE Public Address */
typedef struct
{
	P2P_IE_Header ie_hdr;
	uint16_t      family;
	uint16_t      port;
	uint32_t      ip;
} P2P_IE_Public_Address;

/* IE MAC address */
typedef struct
{
	P2P_IE_Header ie_hdr;
	char          mac_addr[6]; /* MAC address : 6 bytes length */
	char          reserved[2];
} P2P_IE_MAC_Address;

/* IE User Info */
typedef struct
{
	P2P_IE_Header ie_hdr;
	uint16_t      user_id;
	char          mac_addr[6];
} P2P_IE_User_Info;

/* IE Bad Message */
typedef struct
{
	P2P_IE_Header ie_hdr;
	uint32_t      cause;
} P2P_IE_Error_Cause;

/* IE Dummy */
typedef struct
{
	P2P_IE_Header ie_hdr;
	uint8_t       ie_msg[32];
} P2P_IE_Dummy;

/* P2P Common Information Element Identifier */
typedef union
{
	P2P_IE_Dummy            dummy;
	P2P_IE_Local_Address    local_addr;
	P2P_IE_MAC_Address      mac_addr;
	P2P_IE_Public_Address   public_addr;
	P2P_IE_User_Info        user_info;
	P2P_IE_Error_Cause      err_cause;
} P2P_Element;

#endif /* INC_P2P_ELEMENT_H_ */
