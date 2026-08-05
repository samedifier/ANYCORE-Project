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

#ifndef ANYCORE_VERTEX_MANAGER_H
#define ANYCORE_VERTEX_MANAGER_H

#include "primitives.h"

typedef struct {
    ANYCORE_InstanceChunk* instanceChunks;

    ANYCORE_ModelChunk* modelChunks;
    uint32_t modelCount;

    ANYCORE_ModelDirtyChunk* modelDirtyChunks;

    uint8_t*  mdcsflags;
    uint16_t* chunkModelDirtyList;
    uint32_t  chunkModelDirtyCount;
    
    uint32_t stride;
    uint32_t modelChunkCountLimit;
} ANYCORE_VertexManager;

#endif // ANYCORE_VERTEX_MANAGER_H