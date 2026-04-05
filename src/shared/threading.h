/*
 * MibiCraft -- A small game in a world of cubes
 * Copyright (C) 2023  Mibi88
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see https://www.gnu.org/licenses/.
 */

#ifndef SHARED_THREADING_H
#define SHARED_THREADING_H

#include <shared/config.h>

#include <stddef.h>

#if THREADING

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include <tchar.h>

#include <buildconfig.h>

typedef HANDLE thread_t;

#define THREAD_CALL(name, data) DWORD WINAPI name(LPVOID data)

#define THREAD_CREATE(id, call, data) ((*(id) = CreateThread(NULL, 0, call, \
                                                             data, 0, \
                                                             NULL)) == NULL)
#define THREAD_JOIN(id) WaitForSingleObject(id, INFINITE); CloseHandle(id)

#define THREAD_EXIT() ExitThread(0); return 0

/* XXX: Is this working correctly? */
typedef HANDLE thread_lock_t;

#define THREAD_LOCK_INIT(l) ((l = CreateMutex(NULL, FALSE, NULL)) == NULL)
#define THREAD_LOCK_LOCK(l) WaitForSingleObject(l, INFINITE)
#define THREAD_LOCK_TRYLOCK(l) WaitForSingleObject(l, 0) != WAIT_OBJECT_0
#define THREAD_LOCK_UNLOCK(l) ReleaseMutex(l)
#define THREAD_LOCK_FREE(l) CloseHandle(l)

#if NPROC
#define THREAD_NPROC() NPROC
#else
int thread_win32_nproc(void);
#define THREAD_NPROC() thread_win32_nproc()
#endif

#else

#include <pthread.h>

typedef pthread_t thread_t;

#define THREAD_CALL(name, data) void *name(void *data)

#define THREAD_EXIT() pthread_exit(NULL)

#define THREAD_CREATE(id, call, data) (pthread_create(id, NULL, call, \
                                                      (void*)data))
#define THREAD_JOIN(id) pthread_join(id, NULL);

typedef pthread_mutex_t thread_lock_t;

#define THREAD_LOCK_INIT(l) pthread_mutex_init(&l, NULL)
#define THREAD_LOCK_LOCK(l) pthread_mutex_lock(&l)
#define THREAD_LOCK_TRYLOCK(l) pthread_mutex_trylock(&l)
#define THREAD_LOCK_UNLOCK(l) pthread_mutex_unlock(&l)
#define THREAD_LOCK_FREE(l) pthread_mutex_destroy(&l)

#if SYS == linux
#include <sys/sysinfo.h>

#if NPROC
#define THREAD_NPROC() NPROC
#else
#define THREAD_NPROC() get_nprocs()
#endif

#else


#if NPROC
#define THREAD_NPROC() NPROC
#else
#define THREAD_NPROC() 1 /* TODO: Support *BSD systems. */
#endif

#endif

#endif

#else

typedef unsigned char thread_t;
typedef unsigned char thread_lock_t;

#define THREAD_ID(id)

#define THREAD_CALL(name, data) int name(void *data)

#define THREAD_EXIT() return 0

#define THREAD_CREATE(id, call, data) call(data)
#define THREAD_JOIN(id) ;

#define THREAD_LOCK_INIT(l) 0
#define THREAD_LOCK_LOCK(l) 0
#define THREAD_LOCK_TRYLOCK(l) 0
#define THREAD_LOCK_UNLOCK(l) 0
#define THREAD_LOCK_FREE(l) 0

#if NPROC
#define THREAD_NPROC() NPROC
#else
#define THREAD_NPROC() 1
#endif

#endif

/* XXX: Are MRSW locks nativly supported by Windows XP and later? */

/* Read-preferring MRSW locks */
#if !(defined(_WIN32) || defined(_WIN64)) && SYS_MRSW_LOCK
/* pthreads implementation */

#if SYS == linux
/* Linux specific implementation */
typedef struct {
    pthread_rwlock_t lock;
    pthread_rwlockattr_t attr;
} thread_rwlock_t;

int thread_rw_init(thread_rwlock_t *l);
#define THREAD_RW_INIT(l) thread_rw_init(l)
#define THREAD_RW_LOCK_READ(l) pthread_rwlock_rdlock(&(l)->lock)
#define THREAD_RW_UNLOCK_READ(l) pthread_rwlock_unlock(&(l)->lock)
#define THREAD_RW_LOCK_WRITE(l) pthread_rwlock_wrlock(&(l)->lock)
#define THREAD_RW_UNLOCK_WRITE(l) pthread_rwlock_unlock(&(l)->lock)
#define THREAD_RW_TRYLOCK_READ(l) pthread_rwlock_tryrdlock(&(l)->lock)
#define THREAD_RW_TRYLOCK_WRITE(l) pthread_rwlock_trywrlock(&(l)->lock)
#define THREAD_RW_FREE(l) \
    { \
        pthread_rwlock_destroy(&(l)->lock); \
        pthread_rwlockattr_destroy(&(l)->attr); \
    }

#else
/* Generic pthreads implementation */
typedef pthread_rwlock_t thread_rwlock_t;

#define THREAD_RW_INIT(l) pthread_rwlock_init(l, NULL)
#define THREAD_RW_LOCK_READ(l) pthread_rwlock_rdlock(l)
#define THREAD_RW_UNLOCK_READ(l) pthread_rwlock_unlock(l)
#define THREAD_RW_LOCK_WRITE(l) pthread_rwlock_wrlock(l)
#define THREAD_RW_UNLOCK_WRITE(l) pthread_rwlock_unlock(l)
#define THREAD_RW_TRYLOCK_READ(l) pthread_rwlock_tryrdlock(l)
#define THREAD_RW_TRYLOCK_WRITE(l) pthread_rwlock_trywrlock(l)
#define THREAD_RW_FREE(l) pthread_rwlock_destroy(l)

#endif

#else
/* Generic implementation */

typedef struct {
    thread_lock_t lock;
    size_t readers;
    unsigned int writing : 1;
} thread_rwlock_t;

int thread_rw_init(thread_rwlock_t *l);
#define THREAD_RW_INIT(l) thread_rw_init(l)

#define THREAD_RW_LOCK_READ(l) \
    { \
        unsigned char writing; \
 \
        do{ \
            THREAD_LOCK_LOCK((l)->lock); \
            writing = (l)->writing; \
            if(!writing) (l)->readers++; \
            THREAD_LOCK_UNLOCK((l)->lock); \
        }while(writing); \
    }

#define THREAD_RW_UNLOCK_READ(l) \
    { \
        THREAD_LOCK_LOCK((l)->lock); \
        (l)->readers--; \
        THREAD_LOCK_UNLOCK((l)->lock); \
    }

#define THREAD_RW_LOCK_WRITE(l) \
    { \
        size_t readers; \
        unsigned char writing; \
 \
        do{ \
            THREAD_LOCK_LOCK((l)->lock); \
            readers = (l)->readers; \
            writing = (l)->writing; \
            if(!readers) (l)->writing = 1; \
            THREAD_LOCK_UNLOCK((l)->lock); \
        }while(readers || writing); \
    }

#define THREAD_RW_UNLOCK_WRITE(l) \
    { \
        THREAD_LOCK_LOCK((l)->lock); \
        (l)->writing = 0; \
        THREAD_LOCK_UNLOCK((l)->lock); \
    }

int thread_rw_trylock_read(thread_rwlock_t *l);
#define THREAD_RW_TRYLOCK_READ(l) thread_rw_trylock_read(l)
int thread_rw_trylock_write(thread_rwlock_t *l);
#define THREAD_RW_TRYLOCK_WRITE(l) thread_rw_trylock_write(l)

#define THREAD_RW_FREE(l) THREAD_LOCK_FREE((l)->lock)

#endif

#endif
