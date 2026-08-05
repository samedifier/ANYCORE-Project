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

#ifndef ANYCORE_UTILS_H
#define ANYCORE_UTILS_H

#include "configuration.h"
#include "anycore.h"

ANYCORE_EXPORT ANYCORE_RESULT initANYCORE(ANYCORE** outanycore,
    uint32_t chunkCountLimit, uint32_t modelChunkCountLimit, uint32_t startCapacity,
    uint32_t loopCapacity, uint32_t moduleCapacity, uint32_t stride);
void freeANYCORE(ANYCORE* anycore);

ANYCORE_EXPORT uint32_t getANYCOREBuildID(ANYCORE* anycore);

#endif // ANYCORE_UTILS_H