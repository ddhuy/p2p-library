/*
 * File name: msg_box.h
 *
 * Purpose:
 *
 * Created on: Nov 6, 2014
 *     Author: ddhuy
 *
 * File history:
 */


#ifndef MSG_BOX_H_
#define MSG_BOX_H_

/********************************************************
 * Headers & Include
 ********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include "network.h"


/********************************************************
 * Macros and constants
 ********************************************************/
/* Maximum items in a box */
#define ITEM_COUNT 16
/* Size of one item */
#define ITEM_SIZE 1024


/********************************************************
 * File types and definitions
 ********************************************************/
/*
 * Message item: store input/output data buffer
 */
typedef struct
{
	sockaddr_in from;
	sockaddr_in to;
	char        buf[ITEM_SIZE];
	int16_t     len;
} Message_Item;

/*
 * Message Box is a ring buffer queue and used to store message items.
 */
typedef struct
{
	Message_Item    items[ITEM_COUNT];
	uint8_t         read_idx;
	uint8_t         write_idx;
	uint16_t        item_count;
	pthread_mutex_t mutex;
} Message_Box;


/********************************************************
 * File functions
 ********************************************************/
/*
 * Purpose:
 * - Create mutex value for synchronizing insert/get item and Initiate
 * all others as Zero
 *
 * Input:
 * - [in/out] msgbox_ptr: the message box need to be created
 *
 * Output:
 * - return Error Code
 */
int8_t create_msgbox(Message_Box* msgbox_ptr);

/*
 * Purpose:
 * - Delete message box
 *
 * Input:
 * - [in/out] msgbox_ptr: the message need to be cleaned
 */
int8_t delete_msgbox(Message_Box* msgbox_ptr);


/*
 * Purpose:
 * - Check if the number of item in message box
 *
 * Input:
 * - [in] mb_ptr: pointer to the message box which is needed to check
 *
 * Output:
 * - Return the number of item
 */
int8_t get_item_count(Message_Box* mb_ptr);


/*
 * Purpose:
 * - Insert one more item to the bottom of the box
 *
 * Input:
 * - [in] mb_ptr: pointer to the message box which is needed to insert
 * - [in] new_item_ptr: pointer to the new item needed to insert
 *
 * Output:
 * - Return 1 if the insertion is successful
 * - Return 0 if the insertion is failed (the queue is full)
 */
int8_t insert_item(Message_Box* mb_ptr, Message_Item* new_item_ptr);


/*
 * Purpose:
 * - Get one item from the top of the box
 *
 * Input:
 * - [in]  mb_ptr: pointer to the message box which is needed to get
 * - [out] item_ptr: pointer to the gotten item
 *
 * Output:
 *
 */
void get_item(Message_Box* mb_ptr, Message_Item* item_ptr);





#endif /* MSG_BOX_H_ */
