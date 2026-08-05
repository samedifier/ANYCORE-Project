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

#ifndef ANYCORE_TRANSFORM_MANAGER_H
#define ANYCORE_TRANSFORM_MANAGER_H

#include "primitives.h"
#include "configuration.h"

typedef struct {
#if SPACE == SPACE_2D
    #if PRESICION_ == PRESICION_FLOAT
        ANYCORE_TransformChunk2Df* transformChunks;
    #elif PRESICION_ == PRESICION_DOUBLE
        ANYCORE_TransformChunk2Dd* transformChunks;
    #endif
#elif SPACE == SPACE_3D
    #if PRESICION_ == PRESICION_FLOAT
        ANYCORE_TransformChunk3Df* transformChunks;
    #elif PRESICION_ == PRESICION_DOUBLE
        ANYCORE_TransformChunk3Dd* transformChunks;
    #endif
#endif

    ANYCORE_DirtyChunk* dirtyChunks;

    uint8_t*  dcsflags;
    uint16_t* chunkDirtyList;
    uint32_t  chunkDirtyCount;
} ANYCORE_TransformManager;

#endif // ANYCORE_TRANSFORM_MANAGER_H
