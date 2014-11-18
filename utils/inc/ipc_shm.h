/*
 * File name: ipc_shm.h
 *
 * Purpose:
 *
 * Created on: Nov 13, 2014
 *     Author: ddhuy
 *
 * File history:
 */


#ifndef IPC_SHM_H_
#define IPC_SHM_H_

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define IPC_SHM_PRIVILEGE     0666             /* rw-rw-rw- */

#define IPC_CMD_SYNC     0x01 /* synchronized read/write  */
#define IPC_CMD_ASYNC    0x02 /* asynchronized read/write */

#define IPC_SHM_DATA_SIZE     (1024 * 512)     /* 512 KB   */
#define IPC_SHM_HEADER_SIZE   (sizeof(ipc_shm_content) - IPC_SHM_DATA_SIZE)
#define IPC_SHM_CONTENT_SIZE  (sizeof(ipc_shm_content))
#define IPC_SHM_SIZE          (sizeof(ipc_shm_t))

/*
 *
 */
typedef struct
{
	pthread_mutex_t     mutex;
	pthread_mutexattr_t mutex_attr;
	int  pid;
	int  seq;
	int  len;
	char data[IPC_SHM_DATA_SIZE];
} ipc_shm_content;

/*
 *
 */
typedef struct
{
	int              shmid;
	int              key;
	int              owner;
	ipc_shm_content *ipc_shm;
} ipc_shm_t;

/*
 *
 */
int ipc_shm_init(ipc_shm_t *tshm_ptr, int key, char init_new);

/*
 *
 */
int ipc_shm_destroy(ipc_shm_t *tshm_ptr);

/*
 *
 */
int ipc_shm_write(ipc_shm_t *tshm_ptr, char *data_ptr, int len, int sync);

/*
 *
 */
int ipc_shm_read(ipc_shm_t *tshm_ptr, char *data_ptr, int *len, int sync);

#endif /* IPC_SHM_H_ */
