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

        TC2DTYPE* tc = ttm->transformChunks[page];

        tc[slot].posx = newPosition.x;
        tc[slot].posy = newPosition.y;
        
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

        ttm->transformChunks[page][slot].rotz = newRotation;
        
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

        TC2DTYPE* tc = ttm->transformChunks[page];

        tc[slot].scax = newScale.x;
        tc[slot].scay = newScale.y;
        
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
            TC2DTYPE* tc = ttm->transformChunks[page];

            outPosition->x = tc[slot].posx;
            outPosition->y = tc[slot].posy;
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
            *outRotation = ttm->transformChunks[page][slot].rotz;
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
            TC2DTYPE* tc = ttm->transformChunks[page];

            outScale->x = tc[slot].scax;
            outScale->y = tc[slot].scay;
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

        TC2DTYPE* tc = ttm->transformChunks[page];

        tc[slot].posx += additionalPosition.x;
        tc[slot].posy += additionalPosition.y;
        
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

        ttm->transformChunks[page][slot].rotz += additionalRotation;
        
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

        TC2DTYPE* tc = ttm->transformChunks[page];

        tc[slot].scax += additionalScale.x;
        tc[slot].scay += additionalScale.y;

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

            TC2DTYPE* tc = ttm->transformChunks[page];

            VEC2DTYPE newPosition = newPositions[i];
            tc[slot].posx = newPosition.x;
            tc[slot].posy = newPosition.y;

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

            TC2DTYPE* tc = ttm->transformChunks[page];

            tc[slot].rotz = newRotations[i];

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

            TC2DTYPE* tc = ttm->transformChunks[page];

            VEC2DTYPE newScale = newScales[i];
            tc[slot].scax = newScale.x;
            tc[slot].scay = newScale.y;

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

            TC2DTYPE* tc = ttm->transformChunks[page];

            outPositions[i] = (VEC2DTYPE){ tc[slot].posx, tc[slot].posy };

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

            TC2DTYPE* tc = ttm->transformChunks[page];

            outRotations[i] = tc[slot].rotz;

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

            TC2DTYPE* tc = ttm->transformChunks[page];

            outScales[i] = (VEC2DTYPE){ tc[slot].scax, tc[slot].scay };

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

            TC2DTYPE* tc = ttm->transformChunks[page];

            VEC2DTYPE value = values[i];
            tc[slot].posx += value.x;
            tc[slot].posy += value.y;

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

            TC2DTYPE* tc = ttm->transformChunks[page];

            tc[slot].rotz += values[i];

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

            TC2DTYPE* tc = ttm->transformChunks[page];

            VEC2DTYPE value = values[i];
            tc[slot].scax += value.x;
            tc[slot].scay += value.y;

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

        TC3DTYPE* tc = ttm->transformChunks[page];

        tc[slot].posx = newPosition.x;
        tc[slot].posy = newPosition.y;
        tc[slot].posz = newPosition.z;

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

        TC3DTYPE* tc = ttm->transformChunks[page];

        tc[slot].rotx = newRotation.x;
        tc[slot].roty = newRotation.y;
        tc[slot].rotz = newRotation.z;
        
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

        TC3DTYPE* tc = ttm->transformChunks[page];

        tc[slot].scax = newScale.x;
        tc[slot].scay = newScale.y;
        tc[slot].scaz = newScale.z;
        
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
            TC3DTYPE* tc = ttm->transformChunks[page];

            outPosition->x = tc[slot].posx;
            outPosition->y = tc[slot].posy;
            outPosition->z = tc[slot].posz;
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
            TC3DTYPE* tc = ttm->transformChunks[page];

            outRotation->x = tc[slot].rotx;
            outRotation->y = tc[slot].roty;
            outRotation->z = tc[slot].rotz;
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
            TC3DTYPE* tc = ttm->transformChunks[page];

            outScale->x = tc[slot].scax;
            outScale->y = tc[slot].scay;
            outScale->z = tc[slot].scaz;
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

        TC3DTYPE* tc = ttm->transformChunks[page];

        tc[slot].posx += additionalPosition.x;
        tc[slot].posy += additionalPosition.y;
        tc[slot].posz += additionalPosition.z;
        
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

        TC3DTYPE* tc = ttm->transformChunks[page];

        tc[slot].rotx += additionalRotation.x;
        tc[slot].roty += additionalRotation.y;
        tc[slot].rotz += additionalRotation.z;
        
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
        TC3DTYPE* tc = ttm->transformChunks[page];

        tc[slot].scax += additionalScale.x;
        tc[slot].scay += additionalScale.y;
        tc[slot].scaz += additionalScale.z;

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

            TC3DTYPE* tc = ttm->transformChunks[page];

            VEC3DTYPE newPosition = newPositions[i];
            tc[slot].posx = newPosition.x;
            tc[slot].posy = newPosition.y;
            tc[slot].posz = newPosition.z;

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

            TC3DTYPE* tc = ttm->transformChunks[page];

            VEC3DTYPE newRotation = newRotations[i];
            tc[slot].rotx = newRotation.x;
            tc[slot].roty = newRotation.y;
            tc[slot].rotz = newRotation.z;

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

            TC3DTYPE* tc = ttm->transformChunks[page];

            VEC3DTYPE newScale = newScales[i];
            tc[slot].scax = newScale.x;
            tc[slot].scay = newScale.y;
            tc[slot].scaz = newScale.z;

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

            TC3DTYPE* tc = ttm->transformChunks[page];
            outPositions[i] = (VEC3DTYPE){ tc[slot].posx, tc[slot].posy, tc[slot].posz };

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

            TC3DTYPE* tc = ttm->transformChunks[page];
            outRotations[i] = (VEC3DTYPE){ tc[slot].rotx, tc[slot].roty, tc[slot].rotz };

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

            TC3DTYPE* tc = ttm->transformChunks[page];
            outScales[i] = (VEC3DTYPE){ tc[slot].scax, tc[slot].scay, tc[slot].scaz };

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

            TC3DTYPE* tc = ttm->transformChunks[page];
            VEC3DTYPE value = values[i];

            tc[slot].posx += value.x;
            tc[slot].posy += value.y;
            tc[slot].posz += value.z;

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

            TC3DTYPE* tc = ttm->transformChunks[page];
            VEC3DTYPE value = values[i];

            tc[slot].rotx += value.x;
            tc[slot].roty += value.y;
            tc[slot].rotz += value.z;

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);
            skip:
        }
        
        return skippedCount;
    }
#endif
#if ANYCORE_ENABLE_ADD_SCALE_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_addScaleBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* values, const uint32_t count) {
        if (!anycore || !entityIDs || !values) { return count; }
        
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

            TC3DTYPE* tc = ttm->transformChunks[page];
            VEC3DTYPE value = values[i];

            tc[slot].scax += value.x;
            tc[slot].scay += value.y;
            tc[slot].scaz += value.z;

            markDirty(ttm, dc, dcsflags, page, slot, wordindx, mask);
            skip:
        }
        
        return skippedCount;
    }
#endif
#endif
