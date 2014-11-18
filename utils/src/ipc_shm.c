/*
 * File name: ipc_shm.c
 *
 * Purpose:
 *
 * Created on: Nov 13, 2014
 *     Author: ddhuy
 *
 * File history:
 */

#include "ipc_shm.h"

/*
 *
 */
int ipc_shm_init(ipc_shm_t *tshm_ptr, int key, char init_new)
{
	int shmid = -1;
	int mode  = -1;

	if (tshm_ptr == NULL)
	{
		return -1;
	}

	/* parse the create or get mode */
	mode = (init_new == 0) ? (IPC_SHM_PRIVILEGE) : (IPC_SHM_PRIVILEGE | IPC_CREAT);

	/* create/get the shared memory id */
	shmid = shmget(key, IPC_SHM_CONTENT_SIZE, mode);
	if (shmid < 0)
	{
		return -2;
	}
	/* attach to the segment and get the pointer to it */
	tshm_ptr->ipc_shm = (ipc_shm_content *)shmat(shmid, NULL, 0);
	if ((char*)tshm_ptr == (char*)(-1))
	{
		return -3;
	}
	/* init tutk shm attributes */
	tshm_ptr->owner = (init_new == 0) ? 0 : 1;
	tshm_ptr->key   = key;
	tshm_ptr->shmid = shmid;
	/* this is new segment */
	if (init_new)
	{
		/* init shm content */
		tshm_ptr->ipc_shm->pid  = getpid();
		tshm_ptr->ipc_shm->len  = 0;
		tshm_ptr->ipc_shm->seq  = 0;
		/* init shm thread safe mutex */
		if (pthread_mutexattr_init(&tshm_ptr->ipc_shm->mutex_attr) != 0)
		{
			return -4;
		}
		if (pthread_mutexattr_setpshared(&tshm_ptr->ipc_shm->mutex_attr, PTHREAD_PROCESS_SHARED) != 0)
		{
			return -5;
		}
		if (pthread_mutex_init(&tshm_ptr->ipc_shm->mutex, &tshm_ptr->ipc_shm->mutex_attr) != 0)
		{
			return -6;
		}
	}
	/* shm create/get successfully */
	return 0;
}


/*
 *
 */
int ipc_shm_destroy(ipc_shm_t *tshm_ptr)
{
	/* destroy the mutex first */
	pthread_mutex_unlock(&tshm_ptr->ipc_shm->mutex);
	pthread_mutex_destroy(&tshm_ptr->ipc_shm->mutex);

	/* Deallocate the shared memory segment */
	if (tshm_ptr->owner)
	{
		if (shmctl(tshm_ptr->shmid, IPC_RMID, NULL) == -1)
		{
			return -2;
		}
	}

	/* detach memory */
	if (shmdt(tshm_ptr->ipc_shm) == -1)
	{
		return -2;
	}
	/* destroy shm successfully */
	return 0;
}


/*
 *
 */
int ipc_shm_write(ipc_shm_t *tshm_ptr, char *data_ptr, int len, int sync)
{
	if (sync & IPC_CMD_SYNC)
	{
		while (1)
		{
			/* lock the shared segment before trying to write data */
			pthread_mutex_lock(&tshm_ptr->ipc_shm->mutex);

			/* unfortunately the reader still have not read the data */
			if (tshm_ptr->ipc_shm->len != 0)
			{
				/* release the mutex for the reader to continue reading */
				pthread_mutex_unlock(&tshm_ptr->ipc_shm->mutex);
			}
			else /* the reader completed read data */
			{
				/* write data to the shared segment */
				tshm_ptr->ipc_shm->pid = getpid(); /* the reader process may be want to know the sender pid */
				tshm_ptr->ipc_shm->seq = (tshm_ptr->ipc_shm->seq + 1) & 0xFFFF;
				tshm_ptr->ipc_shm->len = len;
				memcpy(tshm_ptr->ipc_shm->data, data_ptr, len); /* copy the actual data to shared segment */

				/* unlock the shared segment after writing data */
				pthread_mutex_unlock(&tshm_ptr->ipc_shm->mutex);

				/* write data synchronized successfully */
				return 0;
			}
		}
	}
	else if (sync & IPC_CMD_ASYNC)
	{
		if (pthread_mutex_trylock(&tshm_ptr->ipc_shm->mutex) < 0)
		{
			return -1;
		}
		if (tshm_ptr->ipc_shm->len != 0)
		{
			/* release the mutex since the reader has not completed */
			pthread_mutex_unlock(&tshm_ptr->ipc_shm->mutex);
			/* return here because we are in ASYNC mode */
			return -2;
		}

		/* write data to the shared segment */
		tshm_ptr->ipc_shm->pid = getpid(); /* the reader process may be want to know the sender pid */
		tshm_ptr->ipc_shm->seq = (tshm_ptr->ipc_shm->seq + 1) & 0xFFFF;
		tshm_ptr->ipc_shm->len = len;
		memcpy(tshm_ptr->ipc_shm->data, data_ptr, len); /* copy the actual data to shared segment */

		/* unlock the shared segment after writing data */
		pthread_mutex_unlock(&tshm_ptr->ipc_shm->mutex);
	}

	return 0;
}


/*
 *
 */
int ipc_shm_read(ipc_shm_t *tshm_ptr, char *data_ptr, int *len, int sync)
{
	if (sync & IPC_CMD_SYNC)
	{
		while (1)
		{
			/* lock the shared segment before trying to read data */
			pthread_mutex_lock(&tshm_ptr->ipc_shm->mutex);

			/* we still have not received any data */
			if (tshm_ptr->ipc_shm->len == 0)
			{
				/* release the mutex for the writer */
				pthread_mutex_unlock(&tshm_ptr->ipc_shm->mutex);
			}
			else
			{
				/* read the data */
				*len = tshm_ptr->ipc_shm->len;
				memcpy(data_ptr, tshm_ptr->ipc_shm->data, tshm_ptr->ipc_shm->len);
				/* then clear the segment */
				tshm_ptr->ipc_shm->pid = 0;
				tshm_ptr->ipc_shm->seq = 0;
				tshm_ptr->ipc_shm->len = 0; /* clear the length as we already read data */
				memset(tshm_ptr->ipc_shm->data, 0, *len);

				/* unlock the shared segment after reading data */
				pthread_mutex_unlock(&tshm_ptr->ipc_shm->mutex);

				/* synchronized read successfully */
				return 0;
			}
		}
	}
	else if (sync & IPC_CMD_ASYNC)
	{
		if (pthread_mutex_trylock(&tshm_ptr->ipc_shm->mutex) < 0)
		{
			return errno;
		}
		if (tshm_ptr->ipc_shm->len == 0)
		{
			/* release the mutex since the segment has not have any data yet */
			pthread_mutex_unlock(&tshm_ptr->ipc_shm->mutex);
			/* return here because we are in ASYNC mode */
			return -2;
		}

		/* read the data */
		*len = tshm_ptr->ipc_shm->len;
		memcpy(data_ptr, tshm_ptr->ipc_shm->data, tshm_ptr->ipc_shm->len);
		/* then clear the segment */
		tshm_ptr->ipc_shm->pid = 0;
		tshm_ptr->ipc_shm->seq = 0;
		tshm_ptr->ipc_shm->len = 0; /* clear the length as we already read data */
		memset(tshm_ptr->ipc_shm->data, 0, *len);

		/* unlock the shared segment after reading data */
		pthread_mutex_unlock(&tshm_ptr->ipc_shm->mutex);

		/* synchronized read successfully */
		return 0;
	}
	return 0;
}





