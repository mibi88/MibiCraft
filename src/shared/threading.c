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

#include <shared/threading.h>

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include <stdio.h>

int thread_win32_nproc(void) {
    SYSTEM_INFO s;
    GetSystemInfo(&s);
    printf("nproc: %d\n", s.dwNumberOfProcessors);
    return s.dwNumberOfProcessors;
}

#endif

/* Read-preferring MRSW locks */
#if !(defined(_WIN32) || defined(_WIN64)) && SYS_MRSW_LOCK
/* pthreads implementation */

#if SYS == linux
int thread_rw_init(thread_rwlock_t *l) {
    if(pthread_rwlockattr_init(&l->attr)){
        return 1;
    }

    if(pthread_rwlockattr_setkind_np(&l->attr,
                                     PTHREAD_RWLOCK_PREFER_READER_NP)){
        pthread_rwlockattr_destroy(&l->attr);

        return 2;
    }

    if(pthread_rwlock_init(&l->lock, &l->attr)){
        pthread_rwlockattr_destroy(&l->attr);

        return 3;
    }

    return 0;
}
#endif

#else
/* Generic implementation */
int thread_rw_init(thread_rwlock_t *l) {
    if(THREAD_LOCK_INIT(l->lock)) return 1;
    l->writing = 0;
    l->readers = 0;

    return 0;
}

int thread_rw_trylock_read(thread_rwlock_t *l) {
    unsigned char writing;
    int rc = 1;

    THREAD_LOCK_LOCK((l)->lock);
    writing = (l)->writing;
    if(!writing){
        (l)->readers++;
        rc = 0;
    }
    THREAD_LOCK_UNLOCK((l)->lock);

    return rc;
}

int thread_rw_trylock_write(thread_rwlock_t *l) {
    size_t readers;
    int rc = 1;

    THREAD_LOCK_LOCK((l)->lock);
    readers = (l)->readers;
    if(!readers){
        (l)->writing = 1;
        rc = 0;
    }
    THREAD_LOCK_UNLOCK((l)->lock);

    return rc;
}

#endif
