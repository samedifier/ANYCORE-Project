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

#include "anycore_transform_manager_utils.h"
#include "anycore_scene_manager_utils.h"

#if SPACE == SPACE_2D
#if ANYCORE_ENABLE_SET_POSITION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_setPosition(ANYCORE* anycore, EntityID entityID, const VEC2DTYPE newPosition) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        TC2DTYPE* tc = &ttm->transformChunks[page];

        tc->posx[slot] = newPosition.x;
        tc->posy[slot] = newPosition.y;
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_SET_ROTATION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_setRotation(ANYCORE* anycore, EntityID entityID, const PRESICION newRotation) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        ttm->transformChunks[page].rotz[slot] = newRotation;
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_SET_SCALE
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_setScale(ANYCORE* anycore, EntityID entityID, const VEC2DTYPE newScale) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        TC2DTYPE* tc = &ttm->transformChunks[page];

        tc->scax[slot] = newScale.x;
        tc->scay[slot] = newScale.y;
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_GET_POSITION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_getPosition(ANYCORE* anycore, EntityID entityID, VEC2DTYPE* outPosition) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        if (outPosition) {
            TC2DTYPE* tc = &ttm->transformChunks[page];

            outPosition->x = tc->posx[slot];
            outPosition->y = tc->posy[slot];
        }
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_GET_ROTATION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_getRotation(ANYCORE* anycore, EntityID entityID, PRESICION* outRotation) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        if (outRotation) {
            *outRotation = ttm->transformChunks[page].rotz[slot];
        }
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_GET_SCALE
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_getScale(ANYCORE* anycore, EntityID entityID, VEC2DTYPE* outScale) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        if (outScale) {
            TC2DTYPE* tc = &ttm->transformChunks[page];

            outScale->x = tc->scax[slot];
            outScale->y = tc->scay[slot];
        }
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_ADD_POSITION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_addPosition(ANYCORE* anycore, EntityID entityID, const VEC2DTYPE additionalPosition) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        TC2DTYPE* tc = &ttm->transformChunks[page];

        tc->posx[slot] += additionalPosition.x;
        tc->posy[slot] += additionalPosition.y;
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_ADD_ROTATION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_addRotation(ANYCORE* anycore, EntityID entityID, const PRESICION additionalRotation) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        ttm->transformChunks[page].rotz[slot] += additionalRotation;
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_ADD_SCALE
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_addScale(ANYCORE* anycore, EntityID entityID, const VEC2DTYPE additionalScale) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        TC2DTYPE* tc = &ttm->transformChunks[page];

        tc->scax[slot] += additionalScale.x;
        tc->scay[slot] += additionalScale.y;

        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_SET_POSITION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_setPositionBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC2DTYPE* newPositions, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC2DTYPE* tc = &ttm->transformChunks[page];

            VEC2DTYPE newPosition = newPositions[i];
            tc->posx[slot] = newPosition.x;
            tc->posy[slot] = newPosition.y;

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_SET_ROTATION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_setRotationBulk(ANYCORE* anycore, EntityID* entityIDs, const PRESICION* newRotations, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC2DTYPE* tc = &ttm->transformChunks[page];

            tc->rotz[slot] = newRotations[i];

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_SET_SCALE_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_setScaleBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC2DTYPE* newScales, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC2DTYPE* tc = &ttm->transformChunks[page];

            VEC2DTYPE newScale = newScales[i];
            tc->scax[slot] = newScale.x;
            tc->scay[slot] = newScale.y;

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_GET_POSITION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_getPositionBulk(ANYCORE* anycore, EntityID* entityIDs, VEC2DTYPE* outPositions, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC2DTYPE* tc = &ttm->transformChunks[page];

            outPositions[i] = (VEC2DTYPE){ tc->posx[slot], tc->posy[slot] };

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_GET_ROTATION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_getRotationBulk(ANYCORE* anycore, EntityID* entityIDs, PRESICION* outRotations, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC2DTYPE* tc = &ttm->transformChunks[page];

            outRotations[i] = tc->rotz[slot];

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_GET_SCALE_BULK    
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_getScaleBulk(ANYCORE* anycore, EntityID* entityIDs, VEC2DTYPE* outScales, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC2DTYPE* tc = &ttm->transformChunks[page];

            outScales[i] = (VEC2DTYPE){ tc->scax[slot], tc->scay[slot] };

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_ADD_POSITION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_addPositionBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC2DTYPE* values, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC2DTYPE* tc = &ttm->transformChunks[page];

            VEC2DTYPE value = values[i];
            tc->posx[slot] += value.x;
            tc->posy[slot] += value.y;

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_ADD_ROTATION_BULK    
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_addRotationBulk(ANYCORE* anycore, EntityID* entityIDs, const PRESICION* values, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC2DTYPE* tc = &ttm->transformChunks[page];

            tc->rotz[slot] += values[i];

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_ADD_SCALE_BULK  
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_addScaleBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC2DTYPE* values, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC2DTYPE* tc = &ttm->transformChunks[page];

            VEC2DTYPE value = values[i];
            tc->scax[slot] += value.x;
            tc->scay[slot] += value.y;

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#elif SPACE == SPACE_3D
#if ANYCORE_ENABLE_SET_POSITION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_setPosition(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE newPosition) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
        
        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        TC3DTYPE* tc = &ttm->transformChunks[page];

        tc->posx[slot] = newPosition.x;
        tc->posy[slot] = newPosition.y;
        tc->posz[slot] = newPosition.z;
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_SET_ROTATION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_setRotation(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE newRotation) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        TC3DTYPE* tc = &ttm->transformChunks[page];

        tc->rotx[slot] = newRotation.x;
        tc->roty[slot] = newRotation.y;
        tc->rotz[slot] = newRotation.z;
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_SET_SCALE
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_setScale(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE newScale) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        TC3DTYPE* tc = &ttm->transformChunks[page];

        tc->scax[slot] = newScale.x;
        tc->scay[slot] = newScale.y;
        tc->scaz[slot] = newScale.z;
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_GET_POSITION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_getPosition(ANYCORE* anycore, EntityID entityID, VEC3DTYPE* outPosition) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        if (outPosition) {
            TC3DTYPE* tc = &ttm->transformChunks[page];

            outPosition->x = tc->posx[slot];
            outPosition->y = tc->posy[slot];
            outPosition->z = tc->posz[slot];
        }
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_GET_ROTATION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_getRotation(ANYCORE* anycore, EntityID entityID, VEC3DTYPE* outRotation) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        if (outRotation) {
            TC3DTYPE* tc = &ttm->transformChunks[page];

            outRotation->x = tc->rotx[slot];
            outRotation->y = tc->roty[slot];
            outRotation->z = tc->rotz[slot];
        }
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_GET_SCALE
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_getScale(ANYCORE* anycore, EntityID entityID, VEC3DTYPE* outScale) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        if (outScale) {
            TC3DTYPE* tc = &ttm->transformChunks[page];

            outScale->x = tc->scax[slot];
            outScale->y = tc->scay[slot];
            outScale->z = tc->scaz[slot];
        }
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_ADD_POSITION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_addPosition(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE additionalPosition) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        TC3DTYPE* tc = &ttm->transformChunks[page];

        tc->posx[slot] += additionalPosition.x;
        tc->posy[slot] += additionalPosition.y;
        tc->posz[slot] += additionalPosition.z;
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_ADD_ROTATION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_addRotation(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE additionalRotation) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

        TC3DTYPE* tc = &ttm->transformChunks[page];

        tc->rotx[slot] += additionalRotation.x;
        tc->roty[slot] += additionalRotation.y;
        tc->rotz[slot] += additionalRotation.z;
        
        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_ADD_SCALE
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_addScale(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE additionalScale) {
        uint32_t page = entityID.slot >> CHUNKSHIFT;
        uint32_t slot = entityID.slot &  CHUNKMASK;

        ANYCORE_SceneManager* sm = &anycore->sceneManager;
        if (page >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
        ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];

        uint32_t wordindx = slot >> 5;
        uint32_t mask     = 1u << (slot & 31);

        if (!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

        if (sc->lockFlags[wordindx] & mask) { return ANYCORE_ERR_ENTITY_LOCKED; }

        ANYCORE_TransformManager* ttm = &anycore->transformManager;
        ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];
        TC3DTYPE* tc = &ttm->transformChunks[page];

        tc->scax[slot] += additionalScale.x;
        tc->scay[slot] += additionalScale.y;
        tc->scaz[slot] += additionalScale.z;

        markDirty(ttm, dc, ttm->dcsflags, page, slot, wordindx, mask);
        return ANYCORE_SUCCESS;
    }
#endif
#if ANYCORE_ENABLE_SET_POSITION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_setPositionBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* newPositions, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;

        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC3DTYPE* tc = &ttm->transformChunks[page];

            VEC3DTYPE newPosition = newPositions[i];
            tc->posx[slot] = newPosition.x;
            tc->posy[slot] = newPosition.y;
            tc->posz[slot] = newPosition.z;

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_SET_ROTATION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_setRotationBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* newRotations, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;

        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC3DTYPE* tc = &ttm->transformChunks[page];

            VEC3DTYPE newRotation = newRotations[i];
            tc->rotx[slot] = newRotation.x;
            tc->roty[slot] = newRotation.y;
            tc->rotz[slot] = newRotation.z;

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_SET_SCALE_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_setScaleBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* newScales, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC3DTYPE* tc = &ttm->transformChunks[page];

            VEC3DTYPE newScale = newScales[i];
            tc->scax[slot] = newScale.x;
            tc->scay[slot] = newScale.y;
            tc->scaz[slot] = newScale.z;

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_GET_POSITION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_getPositionBulk(ANYCORE* anycore, EntityID* entityIDs, VEC3DTYPE* outPositions, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC3DTYPE* tc = &ttm->transformChunks[page];

            outPositions[i] = (VEC3DTYPE){ tc->posx[slot], tc->posy[slot], tc->posz[slot] };

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_GET_ROTATION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_getRotationBulk(ANYCORE* anycore, EntityID* entityIDs, VEC3DTYPE* outRotations, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC3DTYPE* tc = &ttm->transformChunks[page];

            outRotations[i] = (VEC3DTYPE){ tc->rotx[slot], tc->roty[slot], tc->rotz[slot] };

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_GET_SCALE_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_getScaleBulk(ANYCORE* anycore, EntityID* entityIDs, VEC3DTYPE* outScales, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC3DTYPE* tc = &ttm->transformChunks[page];

            outScales[i] = (VEC3DTYPE){ tc->scax[slot], tc->scay[slot], tc->scaz[slot] };

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_ADD_POSITION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_addPositionBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* values, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC3DTYPE* tc = &ttm->transformChunks[page];

            VEC3DTYPE value = values[i];
            tc->posx[slot] += value.x;
            tc->posy[slot] += value.y;
            tc->posz[slot] += value.z;

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_ADD_ROTATION_BULK    
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_addRotationBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* values, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC3DTYPE* tc = &ttm->transformChunks[page];

            VEC3DTYPE value = values[i];
            tc->rotx[slot] += value.x;
            tc->roty[slot] += value.y;
            tc->rotz[slot] += value.z;

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_ADD_SCALE_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_addScaleBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* values, const uint32_t count) {
        if (!anycore || !entityIDs) { return count; }
        
        ANYCORE_SceneManager* sm  = &anycore->sceneManager;
        ANYCORE_TransformManager* ttm = &anycore->transformManager;

        uint8_t* dcsflags = ttm->dcsflags;
        
        uint32_t skippedCount = 0;

        for (uint32_t i = 0; i < count; i++) {
            EntityID entityID = entityIDs[i];
            uint32_t page = entityID.slot >> CHUNKSHIFT;
            uint32_t slot = entityID.slot &  CHUNKMASK;

            ANYCORE_SceneChunk* sc = &sm->sceneChunks[page];
            ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[page];

            uint32_t wordindx = slot >> 5;
            uint32_t mask     = 1u << (slot & 31);

            if ((!((sc->validFlags[wordindx] & mask) &&
            (sc->generations[slot] == entityID.generation))) ||
            (sc->lockFlags[wordindx] & mask)) { skippedCount++; goto skip; }

            TC3DTYPE* tc = &ttm->transformChunks[page];

            VEC3DTYPE value = values[i];
            tc->scax[slot] += value.x;
            tc->scay[slot] += value.y;
            tc->scaz[slot] += value.z;

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);

            skip:
        }
        
        return skippedCount;
    }
#endif
#endif
