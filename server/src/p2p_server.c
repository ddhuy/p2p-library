/*
 * File name: p2p_server.c
 *
 * Purpose:
 *
 * Created on: Nov 18, 2014
 *     Author: ddhuy
 *
 * File history:
 */


/***************************************************************
 *
 * Include
 *
 ***************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <remote_client.h>
#include "ipc_shm.h"
#include "msg_box.h"
#include "msg_io.h"
#include "network.h"
#include "stun.h"
#include "p2p.h"


/***************************************************************
 *
 * Macros and constants
 *
 ***************************************************************/
#define TIMEOUT_SEC   0
#define TIMEOUT_USEC  0

#define MAX_DEV      10


/***************************************************************
 *
 * Type definition and Data structures
 *
 ***************************************************************/
/* store 1 device information */
typedef struct
{
	uint16_t id;
	uint16_t local_port;
	uint32_t local_ip;
	uint16_t public_port;
	uint32_t public_ip;
	char     mac[6];
} Device_t;



/***************************************************************
 *
 * File global variables
 *
 ***************************************************************/
/* The Inbox used for store all incoming messages before decoding */
Message_Box g_mb_inbox;
/* The Outbox used for store all outgoing message after encoding  */
Message_Box g_mb_outbox;

/* The message IO of Receiver: store all received message */
Message_IO*  g_msgio_sender;
/* The message IO of Sender: store all message for sending */
Message_IO*  g_msgio_receiver;

/* List for storing all connected devices */
Device_t ls_device[10] = { 0 };

/* Main server loop flag */
int8_t g_svr_down = FALSE;


/***************************************************************
 *
 * File local functions
 *
 ***************************************************************/




/***************************************************************
 *
 * Main p2p_server Function
 *
 ***************************************************************/
int main(int argc, char** argv)
{
	int16_t port;
	uint8_t rval, i;
	Message_Item msg_item;
	Device_t* dev_ptr;

	P2P_Message p2p_rcvmsg, p2p_sndmsg;
	P2P_Cause   p2p_cause = P2P_CAUSE_NULL;

	if (argc < 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		return -1;
	}

	/* init network */
	port = atoi(argv[1]);
	EndPoint* ep_ptr = create_EP(port, SOCK_DGRAM, IPPROTO_IP, TIMEOUT_SEC, TIMEOUT_USEC, &rval);

	/* init message box & message io */
	create_msgbox(&g_mb_inbox);  /* create Inbox  */
	create_msgbox(&g_mb_outbox); /* create Outbox */

	g_msgio_receiver = create_msgio(&g_mb_inbox, ep_ptr);  /* create the receiving handler */
	g_msgio_sender   = create_msgio(&g_mb_outbox, ep_ptr); /* create the sending handler */

	/* init stun protocol stack */

	/* init p2p protocol stack */

	/* start message io */
	start_msgio(g_msgio_receiver, (void*)recv_hndlr, g_msgio_receiver); /* start receiving message */
	start_msgio(g_msgio_sender, (void*)send_hndlr, g_msgio_sender);     /* start sending message   */

	/*
	 * Main server loop
	 */
	while (g_svr_down == FALSE)
	{

	}

	return 0;
}
