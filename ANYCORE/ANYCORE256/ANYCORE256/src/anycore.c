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

#include "configuration.h"
#include "primitives.h"

#include "anycore_utils.h"
#include "anycore_time_manager_utils.h"
#include "anycore_scene_manager_utils.h"
#include "anycore_transform_manager_utils.h"
#include "anycore_vertex_manager_utils.h"
#include "anycore_app_manager_utils.h"

ANYCORE_EXPORT ANYCORE_RESULT initANYCORE(ANYCORE** outanycore,
    uint32_t chunkCountLimit, uint32_t modelChunkCountLimit, uint32_t startCapacity,
    uint32_t loopCapacity, uint32_t moduleCapacity, uint32_t stride) {

    uint32_t prec1 = sizeof(ANYCORE);
    
    ANYCORE* core = (ANYCORE*)ANYCORE_mmap(prec1);
    if (core == ANYCORE_MAP_FAILED) { return ANYCORE_ERR_ALLOC_FAILED; }
        
    ANYCORE_memset(core, 0, sizeof(ANYCORE));

    if (chunkCountLimit == 0)          { chunkCountLimit = 1;          }
    if (chunkCountLimit >= CHUNKLIMIT) { chunkCountLimit = CHUNKLIMIT; }

    if (modelChunkCountLimit == 0)          { modelChunkCountLimit = 1;          }
    if (modelChunkCountLimit >= CHUNKLIMIT) { modelChunkCountLimit = CHUNKLIMIT; }

    ANYCORE_RESULT smresult = ANYCORE_initSceneManager(core, chunkCountLimit, modelChunkCountLimit, stride);
    ANYCORE_RESULT amresult = initAppManager(core, startCapacity, loopCapacity, moduleCapacity);
    if (smresult != ANYCORE_SUCCESS) { 
        ANYCORE_munmap(core, prec1);
        return smresult; }
    if (amresult != ANYCORE_SUCCESS) { 
        freeScene(core);
        ANYCORE_munmap(core, prec1);
        return amresult; 
    }

    ANYCORE_TimeManager_Start(&core->timeManager);

    core->isinitalized = true;
    *outanycore = core; 
    return ANYCORE_SUCCESS;
}

void freeANYCORE(ANYCORE* anycore) {
    freeScene(anycore); TransformManager_onFree(anycore); VertexManager_onFree(anycore); ANYCORE_munmap(anycore, sizeof(ANYCORE));
}

ANYCORE_EXPORT uint32_t getANYCOREBuildID(ANYCORE* anycore) { return ANYCORE_BUILD_ID; }