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

#include "anycore_scene_manager_utils.h"
#include "anycore_transform_manager_utils.h"

#if ANYCORE_ENABLE_IS_ENTITY_ALIVE
ANYCORE_EXPORT bool ANYCORE_isEntityAlive(ANYCORE* anycore, const EntityID entityID) {
    ANYCORE_SceneManager* sm = &anycore->sceneManager;

    uint32_t page = entityID.slot >> CHUNKSHIFT;
    if (page >= sm->chunkcount) { return false; }

    ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
    uint32_t slot = entityID.slot & CHUNKMASK;

    return ((sc->validFlags[slot >> 5] & (1u << (slot & 31))) &&
            (sc->generations[slot] == entityID.generation));
}
#endif

#if ANYCORE_ENABLE_IS_ENTITY_LOCKED
ANYCORE_EXPORT bool ANYCORE_isEntityLocked(ANYCORE* anycore, const EntityID entityID) {
    ANYCORE_SceneManager* sm = &anycore->sceneManager;

    uint32_t page = entityID.slot >> CHUNKSHIFT;
    if (page >= sm->chunkcount) { return false; }

    uint32_t slot = entityID.slot & CHUNKMASK;
    
    return (sm->sceneChunks[page].lockFlags[slot >> 5] & (1u << (slot & 31)));
}
#endif

#if ANYCORE_ENABLE_ALLOC_NEW_CHUNK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_allocNewChunk(ANYCORE* anycore) {
    ANYCORE_SceneManager* sm = &anycore->sceneManager;
    if (sm->chunkcount >= sm->chunkCountLimit) { return ANYCORE_ERR_CHUNK_LIMIT; }

    uint32_t i = sm->chunkcount;
    ANYCORE_SceneChunk* sc = &sm->sceneChunks[i];

    uint32_t prec1 = CHUNKSIZE * sizeof(uint16_t);
    uint32_t prec2 = 0X800     * sizeof(uint32_t);

    uint16_t* gen = ANYCORE_mmap(prec1);
    uint16_t* fs  = ANYCORE_mmap(prec1);
    uint32_t* vf  = ANYCORE_mmap(prec2);
    uint32_t* lf  = ANYCORE_mmap(prec2);

    if (gen == ANYCORE_MAP_FAILED || fs == ANYCORE_MAP_FAILED || vf == ANYCORE_MAP_FAILED || lf == ANYCORE_MAP_FAILED) {
        if (gen != ANYCORE_MAP_FAILED) { ANYCORE_munmap(gen, prec1); }
        if (fs  != ANYCORE_MAP_FAILED) { ANYCORE_munmap(fs,  prec1); }
        if (vf  != ANYCORE_MAP_FAILED) { ANYCORE_munmap(vf,  prec2); }
        if (lf  != ANYCORE_MAP_FAILED) { ANYCORE_munmap(lf,  prec2); }
        return ANYCORE_ERR_ALLOC_FAILED;
    }

    for (uint32_t j = 0; j < CHUNKSIZE; ++j) { fs[j] = (uint16_t)j; }

    ANYCORE_RESULT vmresult = VertexManager_onGrow(anycore, i);
    ANYCORE_RESULT tmresult = TransformManager_onGrow(anycore, i);

    if (tmresult != ANYCORE_SUCCESS || vmresult != ANYCORE_SUCCESS) {
        ANYCORE_munmap(gen, prec1);
        ANYCORE_munmap(fs,  prec1);
        ANYCORE_munmap(vf,  prec2);
        ANYCORE_munmap(lf,  prec2);
        return ANYCORE_ERR_ALLOC_FAILED;
    }

    sc->generations = gen;
    sc->freeSlots   = fs;
    sc->validFlags  = vf;
    sc->lockFlags   = lf;

    sm->fssize[i] = CHUNKSIZE;
    sm->usablefsc[sm->ufscsize++] = i;
    sm->isusable[i] = 1;

    sm->chunkcount++;
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_FREE_LAST_CHUNK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_freeLastChunk(ANYCORE* anycore) {
    ANYCORE_SceneManager*     sm  = &anycore->sceneManager;
    ANYCORE_TransformManager* ttm = &anycore->transformManager;
    ANYCORE_VertexManager*    vm  = &anycore->vertexManager;

    uint32_t prec1 = CHUNKSIZE * sizeof(PRESICION);
    uint32_t prec2 = CHUNKSIZE * sizeof(uint16_t);
    uint32_t prec3 = 0X800     * sizeof(uint32_t);

    if (sm->chunkcount == 0) { return ANYCORE_ERR_NO_CHUNK; }
    uint32_t last = sm->chunkcount - 1;

    ANYCORE_SceneChunk* sc = sm->sceneChunks;
    ANYCORE_DirtyChunk* dc = ttm->dirtyChunks;
#if SPACE == SPACE_2D
    TC2DTYPE* tc = ttm->transformChunks;
#elif SPACE == SPACE_3D
    TC3DTYPE* tc = ttm->transformChunks;
#endif

    ANYCORE_munmap(sc[last].generations, prec2);
    ANYCORE_munmap(sc[last].freeSlots,   prec2);
    ANYCORE_munmap(sc[last].validFlags,  prec3);
    ANYCORE_munmap(sc[last].lockFlags,   prec3);

#if SPACE == SPACE_2D
    ANYCORE_munmap(tc[last].posx, prec1); ANYCORE_munmap(tc[last].posy, prec1);
    ANYCORE_munmap(tc[last].scax, prec1); ANYCORE_munmap(tc[last].scay, prec1);
    ANYCORE_munmap(tc[last].rotz, prec1);
#elif SPACE == SPACE_3D
    ANYCORE_munmap(tc[last].posx, prec1); ANYCORE_munmap(tc[last].posy, prec1); ANYCORE_munmap(tc[last].posz, prec1);
    ANYCORE_munmap(tc[last].rotx, prec1); ANYCORE_munmap(tc[last].roty, prec1); ANYCORE_munmap(tc[last].rotz, prec1);
    ANYCORE_munmap(tc[last].scax, prec1); ANYCORE_munmap(tc[last].scay, prec1); ANYCORE_munmap(tc[last].scaz, prec1);
#endif
    ANYCORE_munmap(dc[last].dirties,   CHUNKSIZE * sizeof(uint32_t));
    ANYCORE_munmap(dc[last].dirtyList, CHUNKSIZE * sizeof(uint16_t));

    ANYCORE_munmap(vm->instanceChunks[last].instances, CHUNKSIZE * sizeof(uint32_t));

    sm->chunkcount = last;
    
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_CREATE_ENTITY
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_createEntity(ANYCORE* anycore, const ModelID modelID, EntityID* outEntityID) {
    ANYCORE_SceneManager*     sm  = &anycore->sceneManager;
    ANYCORE_TransformManager* ttm = &anycore->transformManager;
    ANYCORE_DirtyChunk* dc  = ttm->dirtyChunks;

    uint32_t ufscsize = sm->ufscsize;
    if (ufscsize == 0) { return ANYCORE_ERR_FREE_SLOT_NOT_FOUND; }

    uint32_t chunk = sm->usablefsc[ufscsize - 1];
    ANYCORE_SceneChunk* sc = &sm->sceneChunks[chunk];

    uint32_t idx  = --sm->fssize[chunk];
    uint32_t slot = sc->freeSlots[idx];

    uint32_t wordindx = slot >> 5;
    uint32_t mask     = 1u << (slot & 31);

    sc->validFlags[wordindx] |= mask;

    if (sm->fssize[chunk] == 0) {
        sm->isusable[chunk] = 0;
        sm->ufscsize--;
    }
    sm->dsize++;

#if SPACE == SPACE_2D
    #if PRESICION_ == PRESICION_FLOAT
        ANYCORE_TransformChunk2Df* tc = &ttm->transformChunks[chunk];
    #elif PRESICION_ == PRESICION_DOUBLE
        ANYCORE_TransformChunk2Dd* tc = &ttm->transformChunks[chunk];
    #endif
#elif SPACE == SPACE_3D
    #if PRESICION_ == PRESICION_FLOAT
        ANYCORE_TransformChunk3Df* tc = &ttm->transformChunks[chunk];
    #elif PRESICION_ == PRESICION_DOUBLE
        ANYCORE_TransformChunk3Dd* tc = &ttm->transformChunks[chunk];
    #endif
#endif

    tc->posx[slot] = (PRESICION)0;
    tc->posy[slot] = (PRESICION)0;
#if SPACE == SPACE_3D
    tc->posz[slot] = (PRESICION)0;
#endif

#if SPACE == SPACE_3D
    tc->rotx[slot] = (PRESICION)0;
    tc->roty[slot] = (PRESICION)0;
#endif
    tc->rotz[slot] = (PRESICION)0;
    

    tc->scax[slot] = (PRESICION)1;
    tc->scay[slot] = (PRESICION)1;
#if SPACE == SPACE_3D
    tc->scaz[slot] = (PRESICION)1;
#endif

    anycore->vertexManager.instanceChunks[chunk].instances[slot] = modelID;

    ID32 outslot        = (chunk << 16) | slot;
    ID16 outgeneration  = ++sc->generations[slot];

    if (outEntityID) {
        outEntityID->slot       = outslot;
        outEntityID->generation = outgeneration;
    }

    markDirty(ttm, dc, ttm->dcsflags, chunk, slot, wordindx, mask);
    markCreateFlag(dc, wordindx, mask, 1);
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_RESTORE_ENTITY
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_restoreEntity(ANYCORE* anycore, EntityID* outEntityID) {
    ANYCORE_SceneManager* sm = &anycore->sceneManager;

    uint32_t ufscsize  = sm->ufscsize;
    if (ufscsize == 0) { return ANYCORE_ERR_FREE_SLOT_NOT_FOUND; }

    uint32_t chunk = sm->usablefsc[ufscsize - 1];
    ANYCORE_SceneChunk* sc = &sm->sceneChunks[chunk];

    uint32_t idx  = --sm->fssize[chunk];
    uint32_t slot = sc->freeSlots[idx];

    uint32_t wordindx = slot >> 5;
    uint32_t mask     = 1u << (slot & 31);

    sc->validFlags[wordindx] |= mask;

    if (sm->fssize[chunk] == 0) {
        sm->isusable[chunk] = 0;
        sm->ufscsize--;
    }
    sm->dsize++;

    ID32 outslot       = (chunk << 16) | slot;
    ID16 outgeneration = sc->generations[slot];

    if (outEntityID) {
        outEntityID->slot       = outslot;
        outEntityID->generation = outgeneration;
    }

    ANYCORE_TransformManager* ttm = &anycore->transformManager;
    ANYCORE_DirtyChunk* dc  = ttm->dirtyChunks;
    markDirty(ttm, dc, ttm->dcsflags, chunk, slot, wordindx, mask);
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_DESTROY_ENTITY
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_destroyEntity(ANYCORE* anycore, const EntityID entityID) {
    ANYCORE_SceneManager* sm = &anycore->sceneManager;

    uint32_t chunk    = entityID.slot >> CHUNKSHIFT;
    uint32_t slot     = entityID.slot &  CHUNKMASK;
    uint32_t mask     = 1u << (slot & 31);
    uint32_t wordindx = slot >> 5;

    if (chunk >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
    ANYCORE_SceneChunk* sc = &sm->sceneChunks[chunk];

    if (!((sc->validFlags[wordindx] & mask) &&
        (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

    if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

    sc->validFlags[wordindx] &= ~mask;
    sc->lockFlags[wordindx]  &= ~mask;
    sc->freeSlots[sm->fssize[chunk]++] = slot;

    if (sm->isusable[chunk] != 1) {
        sm->usablefsc[sm->ufscsize++] = chunk;
        sm->isusable[chunk] = 1;
    }

    sm->dsize--;

    ANYCORE_TransformManager* ttm = &anycore->transformManager;
    ANYCORE_DirtyChunk* dc  = ttm->dirtyChunks;
    markDirty(ttm, dc, ttm->dcsflags, chunk, slot, wordindx, mask);
    markCreateFlag(dc, wordindx, mask, 0);
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_LOCK_ENTITY
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_lockEntity(ANYCORE* anycore, const EntityID entityID) {
    ANYCORE_SceneManager* sm = &anycore->sceneManager;

    uint32_t chunk    = entityID.slot >> CHUNKSHIFT;
    uint32_t slot     = entityID.slot &  CHUNKMASK;
    uint32_t mask     = 1u << (slot & 31);
    uint32_t wordindx = slot >> 5;

    if (chunk >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
    ANYCORE_SceneChunk* sc = &sm->sceneChunks[chunk];

    if (!((sc->validFlags[wordindx] & mask) &&
    (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

    sc->lockFlags[wordindx] |= mask;
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_UNLOCK_ENTITY
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_unlockEntity(ANYCORE* anycore, const EntityID entityID) {
    ANYCORE_SceneManager* sm = &anycore->sceneManager;

    uint32_t chunk    = entityID.slot >> CHUNKSHIFT;
    uint32_t slot     = entityID.slot &  CHUNKMASK;
    uint32_t mask     = 1u << (slot & 31);
    uint32_t wordindx = slot >> 5;

    if (chunk >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
    ANYCORE_SceneChunk* sc = &sm->sceneChunks[chunk];

    if (!((sc->validFlags[wordindx] & mask) &&
    (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

    sc->lockFlags[wordindx] &= ~mask;
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_CREATE_ENTITY_BULK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_createEntityBulk(ANYCORE* anycore, const ModelID modelID, EntityID* outEntityIDs, uint32_t count) {
    if (!anycore) { return ANYCORE_ERR_INVALID_ANYCORE; }

    ANYCORE_SceneManager* restrict sm = &anycore->sceneManager;
    ANYCORE_TransformManager* restrict ttm = &anycore->transformManager;
    ANYCORE_DirtyChunk* restrict dc  = ttm->dirtyChunks;

    if (count == 0) { return ANYCORE_SUCCESS; }

    uint32_t ufscsize = sm->ufscsize;
    if (ufscsize == 0) { return ANYCORE_ERR_FREE_SLOT_NOT_FOUND; }

    uint32_t totalfs = 0;
    for (uint32_t i = 0; i < ufscsize; i++) { totalfs += sm->fssize[i]; }
    if (totalfs < count) { return ANYCORE_ERR_FREE_SLOT_NOT_FOUND; }

    uint32_t chunk = sm->usablefsc[ufscsize - 1];
    ANYCORE_SceneChunk* restrict sc = &sm->sceneChunks[chunk];
#if SPACE == SPACE_2D
    #if PRESICION_ == PRESICION_FLOAT
        ANYCORE_TransformChunk2Df* restrict tc = &ttm->transformChunks[chunk];
    #elif PRESICION_ == PRESICION_DOUBLE
        ANYCORE_TransformChunk2Dd* restrict tc = &ttm->transformChunks[chunk];
    #endif
#elif SPACE == SPACE_3D
    #if PRESICION_ == PRESICION_FLOAT
        ANYCORE_TransformChunk3Df* restrict tc = &ttm->transformChunks[chunk];
    #elif PRESICION_ == PRESICION_DOUBLE
        ANYCORE_TransformChunk3Dd* restrict tc = &ttm->transformChunks[chunk];
    #endif
#endif

    for (uint32_t i = 0; i < count; i++) {
        uint32_t currentChunk = sm->usablefsc[sm->ufscsize - 1];
        if (chunk != currentChunk) {
            chunk = currentChunk;

            sc = &sm->sceneChunks[chunk];
            tc = &ttm->transformChunks[chunk];
        }

        uint32_t idx  = --sm->fssize[chunk];
        uint32_t slot = sc->freeSlots[idx];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        sc->validFlags[wordindx] |= mask;

        if (sm->fssize[chunk] == 0) {
            sm->isusable[chunk] = 0;
            sm->ufscsize--;
        }

        tc->posx[slot] = (PRESICION)0;
        tc->posy[slot] = (PRESICION)0;
#if SPACE == SPACE_3D
        tc->posz[slot] = (PRESICION)0;
#endif

#if SPACE == SPACE_3D
        tc->rotx[slot] = (PRESICION)0;
        tc->roty[slot] = (PRESICION)0;
#endif
        tc->rotz[slot] = (PRESICION)0;
    

        tc->scax[slot] = (PRESICION)1;
        tc->scay[slot] = (PRESICION)1;
#if SPACE == SPACE_3D
        tc->scaz[slot] = (PRESICION)1;
#endif

        anycore->vertexManager.instanceChunks[chunk].instances[slot] = modelID;

        if (outEntityIDs) { outEntityIDs[i] = (EntityID){ (chunk << 16) | slot, ++sc->generations[slot] }; }
        markDirty(ttm, dc, ttm->dcsflags, chunk, slot, wordindx, mask);
        markCreateFlag(dc, wordindx, mask, 1);
    }
    sm->dsize += count;

    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_RESTORE_ENTITY_BULK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_restoreEntityBulk(ANYCORE* anycore, EntityID* outEntityIDs, uint32_t count) {
    if (!anycore) { return ANYCORE_ERR_INVALID_ANYCORE; }

    ANYCORE_SceneManager* restrict sm = &anycore->sceneManager;
    ANYCORE_TransformManager* restrict ttm = &anycore->transformManager;
    ANYCORE_DirtyChunk* restrict dc  = ttm->dirtyChunks;

    if (count == 0) { return ANYCORE_SUCCESS; }

    uint32_t ufscsize = sm->ufscsize;
    if (ufscsize == 0) { return ANYCORE_ERR_FREE_SLOT_NOT_FOUND; }

    uint32_t totalfs = 0;
    for (uint32_t i = 0; i < ufscsize; i++) { totalfs += sm->fssize[i]; }
    if (totalfs < count) { return ANYCORE_ERR_FREE_SLOT_NOT_FOUND; }

    uint32_t chunk = sm->usablefsc[ufscsize - 1];
    const ANYCORE_SceneChunk* restrict sc = &sm->sceneChunks[chunk];

    for (uint32_t i = 0; i < count; i++) {
        uint32_t currentChunk = sm->usablefsc[sm->ufscsize - 1];
        if (chunk != currentChunk) {
            chunk = currentChunk;
            sc = &sm->sceneChunks[chunk];
        }

        uint32_t idx  = --sm->fssize[chunk];
        uint32_t slot = sc->freeSlots[idx];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        sc->validFlags[wordindx] |= mask;

        if (sm->fssize[chunk] == 0) {
            sm->isusable[chunk] = 0;
            sm->ufscsize--;
        }

        if (outEntityIDs) { outEntityIDs[i] = (EntityID){ (chunk << 16) | slot, sc->generations[slot] }; }
        markDirty(ttm, dc, ttm->dcsflags, chunk, slot, wordindx, mask);
    }
    sm->dsize += count;

    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_DESTROY_ENTITY_BULK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_destroyEntityBulk(ANYCORE* anycore, EntityID* entityIDs, uint32_t count) {
    if (!anycore) { return ANYCORE_ERR_INVALID_ANYCORE; }
    if (!entityIDs) { return ANYCORE_ERR_NULL_POINTER; }

    ANYCORE_SceneManager* restrict sm = &anycore->sceneManager;
    ANYCORE_TransformManager* restrict ttm = &anycore->transformManager;
    ANYCORE_DirtyChunk* restrict dc  = ttm->dirtyChunks;

    ANYCORE_SceneChunk* restrict asc = sm->sceneChunks;

    uint32_t skippedcount = 0;
    for (uint32_t i = 0; i < count; i++) {
        const EntityID entityID = entityIDs[i];

        const uint32_t chunk    = entityID.slot >> CHUNKSHIFT;
        const uint32_t slot     = entityID.slot &  CHUNKMASK;
        const uint32_t mask     = 1u << (slot & 31);
        const uint32_t wordindx = slot >> 5;

        if (chunk >= sm->chunkcount) { skippedcount++; goto skip; }
        ANYCORE_SceneChunk* restrict sc = &asc[chunk];

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { skippedcount++; goto skip; }

        if (sc->lockFlags[wordindx] & mask) { skippedcount++; goto skip; }

        sc->validFlags[wordindx] &= ~mask;
        sc->lockFlags[wordindx]  &= ~mask;
        sc->freeSlots[sm->fssize[chunk]++] = slot;

        if (sm->isusable[chunk] != 1) {
            sm->usablefsc[sm->ufscsize++] = chunk;
            sm->isusable[chunk] = 1;
        }

        markDirty(ttm, dc, ttm->dcsflags, chunk, slot, wordindx, mask);
        markCreateFlag(dc, wordindx, mask, 0);
        skip:
    }
    sm->dsize -= (count - skippedcount);

    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_LOCK_ENTITY_BULK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_lockEntityBulk(ANYCORE* anycore, EntityID* entityIDs, uint32_t count) {
    if (!anycore) { return ANYCORE_ERR_INVALID_ANYCORE; }
    if (!entityIDs) { return ANYCORE_ERR_NULL_POINTER; }
    
    ANYCORE_SceneManager* sm = &anycore->sceneManager;

    for (uint32_t i = 0; i < count; i++) {
        const EntityID eid = entityIDs[i];
        uint32_t chunk    = eid.slot >> CHUNKSHIFT;
        uint32_t slot     = eid.slot &  CHUNKMASK;
        
        if (chunk >= sm->chunkcount) { goto skip; }
        
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[chunk];
        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) && (sc->generations[slot] == eid.generation))) { goto skip; }

        sc->lockFlags[wordindx] |= mask;

        skip:        
    }
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_UNLOCK_ENTITY_BULK
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_unlockEntityBulk(ANYCORE* anycore, EntityID* entityIDs, uint32_t count) {
    if (!anycore) { return ANYCORE_ERR_INVALID_ANYCORE; }
    if (!entityIDs) { return ANYCORE_ERR_NULL_POINTER; }
    
    ANYCORE_SceneManager* sm = &anycore->sceneManager;

    for (uint32_t i = 0; i < count; i++) {
        const EntityID eid = entityIDs[i];
        uint32_t chunk    = eid.slot >> CHUNKSHIFT;
        uint32_t slot     = eid.slot &  CHUNKMASK;
        
        if (chunk >= sm->chunkcount) { goto skip; }
        
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[chunk];
        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) && (sc->generations[slot] == eid.generation))) { goto skip; }

        sc->lockFlags[wordindx] &= ~mask;
        
        skip:        
    }
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_DESTROY_ALL_ENTITIES
ANYCORE_EXPORT uint32_t ANYCORE_destroyAllEntities(ANYCORE* anycore) {
    if (!anycore) { return ANYCORE_ERR_INVALID_ANYCORE; }

    ANYCORE_SceneManager* restrict sm = &anycore->sceneManager;
    ANYCORE_TransformManager* restrict ttm = &anycore->transformManager;
    ANYCORE_DirtyChunk* restrict dc  = ttm->dirtyChunks;

    uint32_t totaldestroyed = 0;

    for (uint32_t c = 0; c < sm->chunkcount; c++) {
        ANYCORE_SceneChunk* restrict sc = &sm->sceneChunks[c];
        uint32_t words = (CHUNKMASK >> 5) + 1;

        for (uint32_t w = 0; w < words; w++) {
            uint32_t todelete = sc->validFlags[w] & ~sc->lockFlags[w];
            if (todelete == 0) { goto skip; }

            for (uint32_t bit = 0; bit < 32; bit++) {
                uint32_t mask = 1u << bit;
                if (todelete & mask) {
                    uint32_t slot = (w << 5) + bit;
                    
                    sc->validFlags[w] &= ~mask;
                    sc->freeSlots[sm->fssize[c]++] = slot;
                    
                    markDirty(ttm, dc, ttm->dcsflags, c, slot, w, mask);
                    totaldestroyed++;
                }
            }

            if (sm->isusable[c] != 1) {
                sm->usablefsc[sm->ufscsize++] = c;
                sm->isusable[c] = 1;
            }

            skip:
        }
    }
    sm->dsize -= totaldestroyed;

    return totaldestroyed;
}
#endif

#if ANYCORE_ENABLE_LOCK_ALL_ENTITIES
ANYCORE_EXPORT uint32_t ANYCORE_lockAllEntities(ANYCORE* anycore) {
    if (!anycore) { return 0; }
    ANYCORE_SceneManager* restrict sm = &anycore->sceneManager;
    uint32_t totallocked = 0;

    for (uint32_t c = 0; c < sm->chunkcount; c++) {
        ANYCORE_SceneChunk* restrict sc = &sm->sceneChunks[c];
        uint32_t words = (CHUNKMASK >> 5) + 1;

        for (uint32_t w = 0; w < words; w++) {
            uint32_t tolock = sc->validFlags[w] & ~sc->lockFlags[w];
            if (tolock == 0) { goto skip; }

            sc->lockFlags[w] |= tolock;
            totallocked += __builtin_popcount(tolock);

            skip:
        }
    }
    return totallocked;
}
#endif

#if ANYCORE_ENABLE_UNLOCK_ALL_ENTITIES
ANYCORE_EXPORT uint32_t ANYCORE_unlockAllEntities(ANYCORE* anycore) {
    if (!anycore) { return 0; }
    ANYCORE_SceneManager* restrict sm = &anycore->sceneManager;
    uint32_t totalunlocked = 0;

    for (uint32_t c = 0; c < sm->chunkcount; c++) {
        ANYCORE_SceneChunk* restrict sc = &sm->sceneChunks[c];
        uint32_t words = (CHUNKMASK >> 5) + 1;

        for (uint32_t w = 0; w < words; w++) {
            uint32_t tounlock = sc->lockFlags[w];
            if (tounlock == 0) { goto skip; }

            sc->lockFlags[w] &= ~tounlock;
            
            totalunlocked += __builtin_popcount(tounlock);

            skip:
        }
    }
    return totalunlocked;
}
#endif

#if ANYCORE_ENABLE_GET_ACTIVE_CHUNK_COUNT
ANYCORE_EXPORT uint32_t ANYCORE_getActiveChunkCount(ANYCORE* anycore, uint32_t index) {
    return CHUNKSIZE - anycore->sceneManager.fssize[index];
}
#endif

#if ANYCORE_ENABLE_GET_LAST_CHUNK_VALID_ENTITY_COUNT
ANYCORE_EXPORT uint32_t ANYCORE_getLastChunkValidEntityCount(ANYCORE* anycore) {
    ANYCORE_SceneManager* sm = &anycore->sceneManager;
    return CHUNKSIZE - sm->fssize[sm->chunkcount - 1];
}
#endif
