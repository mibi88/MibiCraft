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

#ifdef _WIN32

#include <windows.h>
#include <tchar.h>

#include <buildconfig.h>

#define THREAD_ID(id) DWORD id;

#define THREAD_CALL(name, data) DWORD WINAPI name(LPVOID data)

#define THREAD_CREATE(id, call, data) CreateThread(NULL, 0, call, data, 0, &id)

#define THREAD_EXIT() ExitThread(0); return 0

/* XXX: Is this working correctly? */
typedef HANDLE thread_lock_t;

#define THREAD_LOCK_INIT(l) l = CreateMutex(NULL, FALSE, NULL)
#define THREAD_LOCK_LOCK(l) WaitForSingleObject(l, INFINITE)
#define THREAD_LOCK_TRYLOCK(l) WaitForSingleObject(l, 0) == WAIT_OBJECT_0
#define THREAD_LOCK_UNLOCK(l) ReleaseMutex(l)
#define THREAD_LOCK_FREE(l) CloseHandle(l)

#define THREAD_NPROC() 1 /* TODO */

#else

#include <pthread.h>

#define THREAD_ID(id) pthread_t id

#define THREAD_CALL(name, data) void *name(void *data)

#define THREAD_EXIT() pthread_exit(NULL)

#define THREAD_CREATE(id, call, data) (pthread_create(&id, NULL, call, \
                                                      (void*)data) | \
                                       pthread_detach(id))

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

#endif

#endif
