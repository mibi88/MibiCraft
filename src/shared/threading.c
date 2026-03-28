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
