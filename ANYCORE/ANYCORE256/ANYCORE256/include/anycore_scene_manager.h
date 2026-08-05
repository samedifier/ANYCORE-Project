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

#ifndef ANYCORE_SCENE_MANAGER_H
#define ANYCORE_SCENE_MANAGER_H

#include "configuration.h"
#include "primitives.h"

typedef struct {
    ANYCORE_SceneChunk* sceneChunks;

    uint32_t  chunkcount;
    uint32_t  dsize;
    uint32_t* fssize;
    uint16_t* usablefsc;
    uint8_t*  isusable;
    uint32_t  ufscsize;

    uint32_t chunkCountLimit;
} ANYCORE_SceneManager;

#endif // ANYCORE_SCENE_MANAGER_H