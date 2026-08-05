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

#ifndef ANYCORE_APP_MANAGER_H
#define ANYCORE_APP_MANAGER_H

#include "configuration.h"
#include "primitives.h"

#include <dlfcn.h>

typedef struct {
    void (**Starts)(void*);
    void (**Loops)(void*, const double);
    uint32_t startCount; uint32_t startCapacity;
    uint32_t loopCount;  uint32_t loopCapacity;

    Module* modules;
    uint32_t moduleCapacity; uint32_t moduleCount;

    ANYCORE_Stream stream;

    bool isAppRunning;
} ANYCORE_AppManager;

#endif // ANYCORE_APP_MANAGER_H