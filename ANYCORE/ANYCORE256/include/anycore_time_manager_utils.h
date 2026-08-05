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

#ifndef ANYCORE_TIME_MANAGER_UTILS_H
#define ANYCORE_TIME_MANAGER_UTILS_H

#include "anycore_time_manager.h"

static inline void ANYCORE_TimeManager_Start(ANYCORE_TimeManager* tm) {
    tm->deltaTime = 0.0;
    tm->elapsed   = 0.0;
    tm->frame     = 0;

    clock_gettime(CLOCK_MONOTONIC, &tm->frameStart);
}

static inline void ANYCORE_TimeManager_Update(ANYCORE_TimeManager* tm) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    tm->deltaTime = (now.tv_sec - tm->frameStart.tv_sec) +
                    (now.tv_nsec - tm->frameStart.tv_nsec) / 1000000000.0;
    tm->frameStart = now;

    tm->elapsed += tm->deltaTime;
}

#endif // ANYCORE_TIME_MANAGER_UTILS_H