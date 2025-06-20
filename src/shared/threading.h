/*
 * MibiCraft - A small game in a world of cubes
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

#ifndef THREADING_H
#define THREADING_H

#include <config.h>

#if THREADING

#ifdef _WIN32

#include <windows.h>
#include <tchar.h>

#define THREAD_ID(id) DWORD id;

#define THREAD_CALL(name, data) DWORD WINAPI name(LPVOID data)

#define THREAD_CREATE(id, call, data) CreateThread(NULL, 0, call, data, 0, &id)

#define THREAD_EXIT() ExitThread(0); return 0

#else

#include <pthread.h>

#define THREAD_ID(id) pthread_t id

#define THREAD_CALL(name, data) void *name(void *data)

#define THREAD_EXIT() pthread_exit(NULL)

#define THREAD_CREATE(id, call, data) pthread_create(&id, NULL, call, \
                                                     (void*)data); \
                                      pthread_detach(id)

#endif

#else

typedef void thread_t;

#define THREAD_ID(id)

#define THREAD_CALL(name, data) void name(void *data)

#define THREAD_EXIT() return

#define THREAD_CREATE(id, call, data) call(data)

#endif

#endif
