/*
 * File name: msg_io.c
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
#include <memory.h>
#include <stdlib.h>

#include "msg_io.h"


/********************************************************
 * File functions
 ********************************************************/



/*
 * Purpose:
 * - Create & initiate the message io
 *
 * Input:
 * - [in] mb_ptr: pointer to message box used as a queue for putting/getting message
 * - [in] ep_ptr: pointer to end point configurator
 *
 * Output:
 * - Return error code
 *
 */
Message_IO* create_msgio(Message_Box* mb_ptr, EndPoint* ep_ptr)
{
	Message_IO* msgio_ptr = (Message_IO*) malloc(sizeof(Message_IO));

	if ( msgio_ptr != NULL )
	{
		/* point to the message box */
		msgio_ptr->msgbox_ptr = mb_ptr;
		/* initiate the thread id, we dont create thread here because we need to start thread later */
		msgio_ptr->tid = -1;
		/* create the EndPoint for sending/receiving data */
		msgio_ptr->ep_ptr = ep_ptr;

		printf("created message io\n");
	}
	/* return the IO */
	return msgio_ptr;
}

/*
 * Purpose:
 * - Delete message io
 *
 * Input:
 * - [in] mb_ptr: pointer to message box need to be deleted
 *
 * Output:
 * - Return error code
 *
 */
void delete_msgio(Message_IO* msgio_ptr)
{
	if ( msgio_ptr != NULL )
	{
		/* connection could not be established, free the IO */
		free(msgio_ptr);
		/* NULL value for the IO */
		msgio_ptr = NULL;

		printf("delete message io\n");
	}
}

/*
 * Purpose:
 * - Create a thread for message io to send/receive data simultaneously
 *
 * Input:
 * - [in] msgio_ptr: the message io need to be run in another thread
 * - [in] handler_func: callback function to the message handler
 * - [in] hndlr_arg: the argument of handler function
 *
 * Output:
 * - Return error code
 *
 */
int8_t start_msgio(Message_IO* msgio_ptr, int8_t (*handler_func)(void*), void* hndlr_arg)
{
	int32_t rval;
	if ( msgio_ptr != NULL )
	{
		rval = pthread_create(&msgio_ptr->tid, NULL, (void*)handler_func, hndlr_arg);
		printf("start message io\n");
	}
	return rval;
}

/*
 * Purpose:
 * - Stop the sending/receiving data thread
 *
 * Input:
 * - [in] msgio_ptr: the message io need to be stopped
 *
 * Output:
 * - Return error code
 *
 */
int8_t stop_msgio(Message_IO* msgio_ptr)
{
	int rval = 0;
	if ( msgio_ptr != NULL )
	{
		if ( msgio_ptr->tid > 0 )
		{
			rval = pthread_cancel(msgio_ptr->tid);
			printf("Stop message io\n");
		}
	}
	return rval;
}

/*
 *
 */
void* recv_hndlr(void* arg_ptr)
{

	uint8_t      error;
	Message_Item item;
	/* cast the argument to Message_IO */
	Message_IO*  msgio_ptr = (Message_IO*) arg_ptr;

	while (1)
	{
		/* clean the item before receiving new message*/
		memset((char*)&item, 0, sizeof(Message_Item));
		/* waiting for incoming message */
		error = recv_message(msgio_ptr->ep_ptr, item.buf, &item.len);

		/* if has no error in getting incoming message */
		if ( !error )
		{
			printf("receive message from %s:%d\n",
					inet_ntoa(msgio_ptr->ep_ptr->dst_sa_in.sin_addr),
					msgio_ptr->ep_ptr->dst_sa_in.sin_port);

			/* pin the from & to address to the item for later sending */
			memcpy((char*)&item.to, (char*)&msgio_ptr->ep_ptr->dst_sa_in, sizeof (sockaddr_in));

			/* then insert new item to inbox for next decoding */
			insert_item(msgio_ptr->msgbox_ptr, &item);
		}
		else
		{
			//pthread_exit((void*)&error);
		}
	}
	return 0;
}



/*
 *
 */
void* send_hndlr(void* arg_ptr)
{
	Message_Item item;
	int8_t       count;
	int32_t      socklen = sizeof(sockaddr_in);

	/* cast the argument to Message_IO */
	Message_IO*  msgio_ptr = (Message_IO*) arg_ptr;

	while (1)
	{
		/* check if the outbox has item */
		count = get_item_count(msgio_ptr->msgbox_ptr);
		if ( count > 0 )
		{
			/* the outbox has at least 1 item, get it out */
			get_item(msgio_ptr->msgbox_ptr, &item);

			/* extract the "to" address and then send it */
			sendto(msgio_ptr->ep_ptr->socket_fd, item.buf, item.len, 0, (sockaddr*)&item.to, socklen);
			printf("send item to %s:%d\n", inet_ntoa(item.to.sin_addr), item.to.sin_port);
		}
		else if ( count < 0 )
		{
			pthread_exit((void*)&count);
		}
		else
		{
		}
	}
}



