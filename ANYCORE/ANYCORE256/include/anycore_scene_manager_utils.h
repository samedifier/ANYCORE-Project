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

#ifndef ANYCORE_SCENE_MANAGER_UTILS_H
#define ANYCORE_SCENE_MANAGER_UTILS_H

#include "anycore_scene_manager.h"
#include "anycore_transform_manager_utils.h"
#include "anycore_vertex_manager_utils.h"

#if ANYCORE_ENABLE_IS_ENTITY_ALIVE
ANYCORE_EXPORT bool ANYCORE_isEntityAlive(ANYCORE* anycore, const EntityID entityID);
#endif

#if ANYCORE_ENABLE_IS_ENTITY_LOCKED
ANYCORE_EXPORT bool ANYCORE_isEntityLocked(ANYCORE* anycore, const EntityID entityID);
#endif

static inline ANYCORE_RESULT ANYCORE_initSceneManager(ANYCORE* anycore, uint32_t chunkCountLimit, uint32_t modelChunkCountLimit, uint32_t stride) {
    if (!anycore)              { return ANYCORE_ERR_INVALID_ANYCORE; }
    if (anycore->isinitalized) { return ANYCORE_ERR_ALREADY_INIT;    }

    ANYCORE_SceneManager* sm = &anycore->sceneManager;

    if (chunkCountLimit > CHUNKLIMIT) { chunkCountLimit = CHUNKLIMIT; }
    sm->chunkCountLimit = chunkCountLimit;

    sm->chunkcount = 0;
    sm->dsize      = 0;
    sm->ufscsize   = 0;

    ANYCORE_SceneChunk* scenechunks = ANYCORE_mmap(chunkCountLimit * sizeof(ANYCORE_SceneChunk));
    uint32_t*  fssize               = ANYCORE_mmap(chunkCountLimit * sizeof(uint32_t ));
    uint16_t*  usablefsc            = ANYCORE_mmap(chunkCountLimit * sizeof(uint16_t ));
    uint8_t*   isusable             = ANYCORE_mmap(chunkCountLimit * sizeof(uint8_t  ));

    if (scenechunks == ANYCORE_MAP_FAILED ||
        fssize      == ANYCORE_MAP_FAILED || usablefsc == ANYCORE_MAP_FAILED || isusable   == ANYCORE_MAP_FAILED) {
        if (scenechunks != ANYCORE_MAP_FAILED) { ANYCORE_munmap(scenechunks, chunkCountLimit * sizeof(ANYCORE_SceneChunk)); }
        if (fssize      != ANYCORE_MAP_FAILED) { ANYCORE_munmap(fssize,      chunkCountLimit * sizeof(uint32_t )); }
        if (usablefsc   != ANYCORE_MAP_FAILED) { ANYCORE_munmap(usablefsc,   chunkCountLimit * sizeof(uint16_t )); }
        if (isusable    != ANYCORE_MAP_FAILED) { ANYCORE_munmap(isusable,    chunkCountLimit * sizeof(uint8_t  )); }
        return ANYCORE_ERR_ALLOC_FAILED;
    }

    ANYCORE_RESULT vmresult = VertexManager_onInit(anycore, stride, chunkCountLimit, modelChunkCountLimit);
    ANYCORE_RESULT tmresult = TransformManager_onInit(anycore, chunkCountLimit);
    if (tmresult != ANYCORE_SUCCESS || vmresult != ANYCORE_SUCCESS) {
        ANYCORE_munmap(scenechunks, chunkCountLimit * sizeof(ANYCORE_SceneChunk));
        ANYCORE_munmap(fssize,      chunkCountLimit * sizeof(uint32_t ));
        ANYCORE_munmap(usablefsc,   chunkCountLimit * sizeof(uint16_t ));
        ANYCORE_munmap(isusable,    chunkCountLimit * sizeof(uint8_t  ));
        return ANYCORE_ERR_ALLOC_FAILED;
    }

    sm->sceneChunks = scenechunks;
    sm->fssize      = fssize;
    sm->usablefsc   = usablefsc;
    sm->isusable    = isusable;

    return ANYCORE_SUCCESS;
}

#if ANYCORE_ENABLE_ALLOC_NEW_CHUNK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_allocNewChunk(ANYCORE* anycore);
#endif

#if ANYCORE_ENABLE_FREE_LAST_CHUNK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_freeLastChunk(ANYCORE* anycore);
#endif

#if ANYCORE_ENABLE_CREATE_ENTITY
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_createEntity(ANYCORE* anycore, const ModelID modelID, EntityID* outEntityID);
#endif

#if ANYCORE_ENABLE_RESTORE_ENTITY
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_restoreEntity(ANYCORE* anycore, EntityID* outEntityID);
#endif

#if ANYCORE_ENABLE_DESTROY_ENTITY
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_destroyEntity(ANYCORE* anycore, const EntityID entityID);
#endif

#if ANYCORE_ENABLE_LOCK_ENTITY
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_lockEntity(ANYCORE* anycore, const EntityID entityID);
#endif

#if ANYCORE_ENABLE_UNLOCK_ENTITY
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_unlockEntity(ANYCORE* anycore, const EntityID entityID);
#endif

#if ANYCORE_ENABLE_CREATE_ENTITY_BULK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_createEntityBulk(ANYCORE* anycore, const ModelID modelID, EntityID* outEntityIDs, uint32_t count);
#endif

#if ANYCORE_ENABLE_RESTORE_ENTITY_BULK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_restoreEntityBulk(ANYCORE* anycore, EntityID* outEntityIDs, uint32_t count);
#endif

#if ANYCORE_ENABLE_DESTROY_ENTITY_BULK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_destroyEntityBulk(ANYCORE* anycore, EntityID* entityIDs, uint32_t count);
#endif

#if ANYCORE_ENABLE_LOCK_ENTITY_BULK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_lockEntityBulk(ANYCORE* anycore, EntityID* entityIDs, uint32_t count);
#endif

#if ANYCORE_ENABLE_UNLOCK_ENTITY_BULK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_unlockEntityBulk(ANYCORE* anycore, EntityID* entityIDs, uint32_t count);
#endif

#if ANYCORE_ENABLE_DESTROY_ALL_ENTITIES
ANYCORE_EXPORT uint32_t ANYCORE_destroyAllEntities(ANYCORE* anycore);
#endif

#if ANYCORE_ENABLE_LOCK_ALL_ENTITIES
ANYCORE_EXPORT uint32_t ANYCORE_lockAllEntities(ANYCORE* anycore);
#endif

#if ANYCORE_ENABLE_UNLOCK_ALL_ENTITIES
ANYCORE_EXPORT uint32_t ANYCORE_unlockAllEntities(ANYCORE* anycore);
#endif

static inline void freeScene(ANYCORE* anycore) {
    ANYCORE_SceneManager* sm = &anycore->sceneManager;
    ANYCORE_SceneChunk* sc = sm->sceneChunks;

    uint32_t prec1 = CHUNKSIZE * sizeof(uint16_t);
    uint32_t prec2 = 0X800     * sizeof(uint32_t);

    for (uint32_t i = 0; i < sm->chunkcount; ++i) {
        ANYCORE_munmap(sc[i].generations, prec1);
        ANYCORE_munmap(sc[i].freeSlots,   prec1);
        ANYCORE_munmap(sc[i].validFlags,  prec2);
        ANYCORE_munmap(sc[i].lockFlags,   prec2);
    }
    
    uint32_t ccl = sm->chunkCountLimit;

    ANYCORE_munmap(sc,            ccl   * sizeof(ANYCORE_SceneChunk));
    ANYCORE_munmap(sm->fssize,    ccl   * sizeof(uint32_t));
    ANYCORE_munmap(sm->usablefsc, ccl   * sizeof(uint16_t));
    ANYCORE_munmap(sm->isusable,  ccl   * sizeof(uint8_t ));
}

#if ANYCORE_ENABLE_GET_ACTIVE_CHUNK_COUNT
ANYCORE_EXPORT uint32_t ANYCORE_getActiveChunkCount(ANYCORE* anycore, uint32_t index);
#endif

#if ANYCORE_ENABLE_GET_LAST_CHUNK_VALID_ENTITY_COUNT
ANYCORE_EXPORT uint32_t ANYCORE_getLastChunkValidEntityCount(ANYCORE* anycore);
#endif

#endif // ANYCORE_SCENE_MANAGER_UTILS_H