/*
 * ANYCORE
 * Copyright (C) 2026 Samed
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2 only,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the LICENSE file for the full license text.
 */

#ifndef ANYCORE_TIME_MANAGER_H
#define ANYCORE_TIME_MANAGER_H

#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 199309L
#endif

#include <time.h>
#include "configuration.h"

typedef struct {
    struct timespec frameStart;

    uint64_t frame;

    double deltaTime;
    double elapsed;
} ANYCORE_TimeManager;
    
#endif // ANYCORE_TIME_MANAGER_H