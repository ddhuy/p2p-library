/*
 * File name: msg_io.h
 *
 * Purpose:
 *
 * Created on: Nov 5, 2014
 *     Author: ddhuy
 *
 * File history:
 */


#ifndef MSG_IO_H_
#define MSG_IO_H_

/********************************************************
 * Headers & Include
 ********************************************************/
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>

#include "msg_box.h"
#include "network.h"


/********************************************************
 * Macros and constants
 ********************************************************/





/********************************************************
 * File types and definitions
 ********************************************************/

/*
 * Message IO
 */
typedef uint32_t (*func_msg_handler)(void*, void*);
typedef struct
{
	pthread_t      tid;         /* Thread ID of Message IO thread  */
	Message_Box*   msgbox_ptr;  /* The message box of Message IO   */
	EndPoint*      ep_ptr;
} Message_IO;

/********************************************************
 * File global variables
 ********************************************************/



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
Message_IO* create_msgio(Message_Box* mb_ptr, EndPoint* ep_ptr);

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
void delete_msgio(Message_IO* msgio_ptr);


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
int8_t start_msgio(Message_IO* msgio_ptr, int8_t (*handler_func)(void*), void* hndlr_arg);

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
int8_t stop_msgio(Message_IO* msgio_ptr);

/*
 *
 */
void* recv_hndlr(void* arg_ptr);

/*
 *
 */
void* send_hndlr(void* arg_ptr);



#endif /* MSG_IO_H_ */
