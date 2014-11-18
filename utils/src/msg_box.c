/*
 * File name: msg_box.c
 *
 * Purpose:
 *
 * Created on: Nov 6, 2014
 *     Author: ddhuy
 *
 * File history:
 */


/********************************************************
 * Headers & Include
 ********************************************************/
#include "msg_box.h"



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
int8_t create_msgbox(Message_Box* msgbox_ptr)
{
	int rval = 0;
	if ( msgbox_ptr != NULL )
	{
		/* clean the message box memory first */
		memset((char*)msgbox_ptr, 0, sizeof(Message_Box));
		/* create the mutex lock */
		rval = pthread_mutex_init(&msgbox_ptr->mutex, NULL);

		printf("create message box \n");
	}
	return rval;
}

/*
 * Purpose:
 * - Delete message box
 *
 * Input:
 * - [in/out] msgbox_ptr: the message need to be cleaned
 *
 * Output:
 * - Return error code
 */
int8_t delete_msgbox(Message_Box* msgbox_ptr)
{
	int rval = 0;
	if ( msgbox_ptr != NULL )
	{
		/* unlock the mutex and destroy it before cleaning the box*/
		if ( (rval = pthread_mutex_unlock(&msgbox_ptr->mutex)) == 0 )
		{
			rval = pthread_mutex_destroy(&msgbox_ptr->mutex);
			/* clean the message box memory first */
			memset((char*)&msgbox_ptr, 0, sizeof(Message_Box));

			printf("delete message box \n");
		}
	}
	return rval;
}


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
int8_t get_item_count(Message_Box* mb_ptr)
{
	if ( mb_ptr == NULL )
	{
		return -1;
	}
	return mb_ptr->item_count;
}


/*
 * Purpose:
 * - Insert one more item to the bottom of the box ( Thread Safety )
 *
 * Input:
 * - [in] mb_ptr: pointer to the message box which is needed to insert
 * - [in] new_item_ptr: pointer to the new item needed to insert
 *
 * Output:
 * - Return the inserted position if the insertion is successful
 * - Return -1 if the insertion is failed (the queue is full)
 */
int8_t insert_item(Message_Box* mb_ptr, Message_Item* new_item_ptr)
{
	uint8_t pos = -1;
	/* check if the message box is not full then we can insert more item */
	if ( mb_ptr->item_count < ITEM_COUNT && new_item_ptr != NULL )
	{
		/* lock the shared resource before inserting */
		pthread_mutex_lock(&mb_ptr->mutex);
		{
			/* save the position which is used to store new item */
			pos = mb_ptr->write_idx;
			/* clean the item data before duplicating from the new one */
			memset((char*)&mb_ptr->items[mb_ptr->write_idx], 0, sizeof(Message_Item));
			/* duplicate data from new item into the box */
			memcpy((char*)&mb_ptr->items[mb_ptr->write_idx], (char*) new_item_ptr, sizeof(Message_Item));
			/* increase then next possible insert position */
			mb_ptr->write_idx = (mb_ptr->write_idx + 1) % ITEM_COUNT;
			/* increase number of item in the box */
			mb_ptr->item_count += 1;

			printf("insert item to %d in message box \n", pos);
		}
		/* unlock the shared resource then another threads can continue */
		pthread_mutex_unlock(&mb_ptr->mutex);
	}
	return pos;
}


/*
 * Purpose:
 * - Get one item from the top of the box ( Thread Safety )
 *
 * Input:
 * - [in]  mb_ptr: pointer to the message box which is needed to get
 * - [out] item_ptr: pointer to the gotten item
 *
 * Output:
 *
 */
void get_item(Message_Box* mb_ptr, Message_Item* got_item_ptr)
{
	Message_Item* item_ptr;
	if ( mb_ptr->item_count > 0 && got_item_ptr != NULL )
	{
		/* lock the shared resource before inserting */
		pthread_mutex_lock(&mb_ptr->mutex);
		{
			printf("get item %d from message box \n", mb_ptr->read_idx);

			/* get the shortcut to read item */
			item_ptr = &mb_ptr->items[mb_ptr->read_idx];
			/* copy data from the box */
			memcpy((char*)got_item_ptr, (char*)item_ptr, sizeof(Message_Item));
			/* increase the read index to the next possible one */
			mb_ptr->read_idx = (mb_ptr->read_idx + 1) % ITEM_COUNT;
			/* decrease number of item in the box */
			mb_ptr->item_count -= 1;
		}
		/* unlock the shared resource then another threads can continue */
		pthread_mutex_unlock(&mb_ptr->mutex);
	}
}
