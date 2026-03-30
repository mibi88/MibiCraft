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

#if THREADING

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include <tchar.h>

#include <buildconfig.h>

typedef HANDLE thread_t;

#define THREAD_CALL(name, data) DWORD WINAPI name(LPVOID data)

#define THREAD_CREATE(id, call, data) CreateThread(NULL, 0, call, data, 0, \
                                                   *(id))
#define THREAD_JOIN(id) WaitForSingleObject(id, INFINITE); CloseHandle(id)

#define THREAD_EXIT() ExitThread(0); return 0

/* XXX: Is this working correctly? */
typedef HANDLE thread_lock_t;

#define THREAD_LOCK_INIT(l) ((l = CreateMutex(NULL, FALSE, NULL)) == NULL)
#define THREAD_LOCK_LOCK(l) WaitForSingleObject(l, INFINITE)
#define THREAD_LOCK_TRYLOCK(l) WaitForSingleObject(l, 0) != WAIT_OBJECT_0
#define THREAD_LOCK_UNLOCK(l) ReleaseMutex(l)
#define THREAD_LOCK_FREE(l) CloseHandle(l)

int thread_win32_nproc(void);
#define THREAD_NPROC() thread_win32_nproc()

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

#define THREAD_NPROC() get_nprocs()
#else
#define THREAD_NPROC() 1 /* TODO: Support *BSD systems. */
#endif

#endif

#else

typedef void thread_t;

#define THREAD_ID(id)

#define THREAD_CALL(name, data) void name(void *data)

#define THREAD_EXIT() return

#define THREAD_CREATE(id, call, data) call(data)
#define THREAD_JOIN(id) ;

#endif

/* Read-preferring MRSW locks */
/* TODO: Do not use this implementation when the system provides MRSW locks. */

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
 \
        do{ \
            THREAD_LOCK_LOCK((l)->lock); \
            readers = (l)->readers; \
            if(!readers) (l)->writing = 1; \
            THREAD_LOCK_UNLOCK((l)->lock); \
        }while(readers); \
    }

#define THREAD_RW_UNLOCK_WRITE(l) \
    { \
        THREAD_LOCK_LOCK((l)->lock); \
        (l)->writing = 0; \
        THREAD_LOCK_UNLOCK((l)->lock); \
    }

#define THREAD_RW_FREE(l) THREAD_LOCK_FREE((l)->lock)

#endif
