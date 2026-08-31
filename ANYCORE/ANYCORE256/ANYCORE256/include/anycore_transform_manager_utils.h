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

#ifndef ANYCORE_TRANSFORM_MANAGER_UTILS_H
#define ANYCORE_TRANSFORM_MANAGER_UTILS_H

#include "anycore_transform_manager.h"
#include "anycore.h"

#if PRESICION_ == PRESICION_FLOAT
    #define VEC2DTYPE Vec2f
    #define VEC3DTYPE Vec3f
    #define TC2DTYPE  ANYCORE_Transform2Df
    #define TC3DTYPE  ANYCORE_Transform3Df
#elif PRESICION_ == PRESICION_DOUBLE
    #define VEC2DTYPE Vec2d
    #define VEC3DTYPE Vec3d
    #define TC2DTYPE  ANYCORE_Transform2Dd
    #define TC3DTYPE  ANYCORE_Transform3Dd
#endif

static inline ANYCORE_RESULT TransformManager_onInit(ANYCORE* anycore, uint32_t ccl) {
#if SPACE == SPACE_2D
    #if PRESICION_ == PRESICION_FLOAT
        uint32_t prec1 = ccl * sizeof(ANYCORE_Transform2Df*);
    #elif PRESICION_ == PRESICION_DOUBLE
        uint32_t prec1 = ccl * sizeof(ANYCORE_Transform2Dd*);
    #endif
#elif SPACE == SPACE_3D
    #if PRESICION_ == PRESICION_FLOAT
        uint32_t prec1 = ccl * sizeof(ANYCORE_Transform3Df*);
    #elif PRESICION_ == PRESICION_DOUBLE
        uint32_t prec1 = ccl * sizeof(ANYCORE_Transform3Dd*);
    #endif
#endif
    uint32_t prec3 = ccl * sizeof(ANYCORE_DirtyChunk);
    uint32_t prec4 = ccl * sizeof(uint16_t);
    uint32_t prec5 = ((ccl + 7) >> 3) * sizeof(uint8_t);

    ANYCORE_TransformManager* ttm = &anycore->transformManager;

#if SPACE == SPACE_2D
    #if PRESICION_ == PRESICION_FLOAT
        ANYCORE_Transform2Df** transformchunks = ANYCORE_mmap(prec1);
    #elif PRESICION_ == PRESICION_DOUBLE
        ANYCORE_Transform2Dd** transformchunks = ANYCORE_mmap(prec1);
    #endif
#elif SPACE == SPACE_3D
    #if PRESICION_ == PRESICION_FLOAT
        ANYCORE_Transform3Df** transformchunks = ANYCORE_mmap(prec1);
    #elif PRESICION_ == PRESICION_DOUBLE
        ANYCORE_Transform3Dd** transformchunks = ANYCORE_mmap(prec1);
    #endif
#endif

    ANYCORE_DirtyChunk* dirtychunks = ANYCORE_mmap(prec3);

    uint16_t* chunkdirtylist = ANYCORE_mmap(prec4);
    uint8_t*  dcsflags       = ANYCORE_mmap(prec5);

    if (transformchunks == ANYCORE_MAP_FAILED || dirtychunks == ANYCORE_MAP_FAILED ||
        chunkdirtylist  == ANYCORE_MAP_FAILED || dcsflags    == ANYCORE_MAP_FAILED) {
        if (transformchunks != ANYCORE_MAP_FAILED) { ANYCORE_munmap(transformchunks, prec1); }
        if (dirtychunks     != ANYCORE_MAP_FAILED) { ANYCORE_munmap(dirtychunks,     prec3); }
        if (chunkdirtylist  != ANYCORE_MAP_FAILED) { ANYCORE_munmap(chunkdirtylist,  prec4); }
        if (dcsflags        != ANYCORE_MAP_FAILED) { ANYCORE_munmap(dcsflags,        prec5); }
        return ANYCORE_ERR_ALLOC_FAILED;
    }

    ANYCORE_memset8(dcsflags, (ccl + 7) >> 3);

    dirtychunks->dirtyCount = 0;

    ttm->transformChunks = transformchunks;
    ttm->dirtyChunks     = dirtychunks;

    ttm->dcsflags = dcsflags;

    ttm->chunkDirtyList  = chunkdirtylist;
    ttm->chunkDirtyCount = 0;

    return ANYCORE_SUCCESS;
}

static inline ANYCORE_RESULT TransformManager_onGrow(ANYCORE* anycore, const uint32_t i) {
    ANYCORE_TransformManager* ttm = &anycore->transformManager;

#if SPACE == SPACE_2D
    #if PRESICION_ == PRESICION_FLOAT
        uint32_t prec = CHUNKSIZE * sizeof(ANYCORE_Transform2Df);
        ANYCORE_Transform2Df* transformsi = ANYCORE_mmap(prec);
    #elif PRESICION_ == PRESICION_DOUBLE
        uint32_t prec = CHUNKSIZE * sizeof(ANYCORE_Transform2Dd);
        ANYCORE_Transform2Dd* transformsi = ANYCORE_mmap(prec);
    #endif
#elif SPACE == SPACE_3D
    #if PRESICION_ == PRESICION_FLOAT
        uint32_t prec = CHUNKSIZE * sizeof(ANYCORE_Transform3Df);
        ANYCORE_Transform3Df* transformsi = ANYCORE_mmap(prec);
    #elif PRESICION_ == PRESICION_DOUBLE
        uint32_t prec = CHUNKSIZE * sizeof(ANYCORE_Transform3Dd);
        ANYCORE_Transform3Dd* transformsi = ANYCORE_mmap(prec);
    #endif
#endif

    uint32_t* dirtiesi = ANYCORE_mmap(0X800 * sizeof(uint32_t));
    uint32_t* createflagsi = ANYCORE_mmap(0X800 * sizeof(uint32_t));
    uint16_t* dirtyindicesi = ANYCORE_mmap(CHUNKSIZE * sizeof(uint16_t));

    if (transformsi == ANYCORE_MAP_FAILED || dirtiesi == ANYCORE_MAP_FAILED || createflagsi == ANYCORE_MAP_FAILED || dirtyindicesi == ANYCORE_MAP_FAILED) {
        if (transformsi   != ANYCORE_MAP_FAILED) { ANYCORE_munmap(transformsi, prec); }
        if (dirtiesi      != ANYCORE_MAP_FAILED) { ANYCORE_munmap(dirtiesi, 0X800 * sizeof(uint32_t)); }
        if (createflagsi  != ANYCORE_MAP_FAILED) { ANYCORE_munmap(createflagsi, 0X800 * sizeof(uint32_t)); }
        if (dirtyindicesi != ANYCORE_MAP_FAILED) { ANYCORE_munmap(dirtyindicesi, CHUNKSIZE * sizeof(uint16_t)); }
        return ANYCORE_ERR_ALLOC_FAILED;
    }

#if SPACE == SPACE_2D
    #if PRESICION_ == PRESICION_FLOAT
        ttm->transformChunks[i] = transformsi;
    #elif PRESICION_ == PRESICION_DOUBLE
        ttm->transformChunks[i] = transformsi;
    #endif
#elif SPACE == SPACE_3D
    #if PRESICION_ == PRESICION_FLOAT
        ttm->transformChunks[i] = transformsi;
    #elif PRESICION_ == PRESICION_DOUBLE
        ttm->transformChunks[i] = transformsi;
    #endif
#endif

    ANYCORE_DirtyChunk* dc = &ttm->dirtyChunks[i];

    dc->dirties     = dirtiesi;
    dc->createFlags = createflagsi;
    dc->dirtyList   = dirtyindicesi;

    return ANYCORE_SUCCESS;
}

static inline void TransformManager_onFree(ANYCORE* anycore) {
    uint32_t ccl = anycore->sceneManager.chunkCountLimit;
    ANYCORE_TransformManager* ttm = &anycore->transformManager;

#if SPACE == SPACE_2D
    #if PRESICION_ == PRESICION_FLOAT
        uint32_t prec1 = CHUNKSIZE * sizeof(ANYCORE_Transform2Df);
    #elif PRESICION_ == PRESICION_DOUBLE
        uint32_t prec1 = CHUNKSIZE * sizeof(ANYCORE_Transform2Dd);
    #endif
#elif SPACE == SPACE_3D
    #if PRESICION_ == PRESICION_FLOAT
        uint32_t prec1 = CHUNKSIZE * sizeof(ANYCORE_Transform3Df);
    #elif PRESICION_ == PRESICION_DOUBLE
        uint32_t prec1 = CHUNKSIZE * sizeof(ANYCORE_Transform3Dd);
    #endif
#endif

    ANYCORE_DirtyChunk* dc = ttm->dirtyChunks;

    for (uint32_t i = 0; i < ccl; i++) {
        ANYCORE_munmap(ttm->transformChunks[i], prec1);
        ANYCORE_munmap(dc[i].dirties, 0X800 * sizeof(uint32_t));
        ANYCORE_munmap(dc[i].createFlags, 0X800 * sizeof(uint32_t));
        ANYCORE_munmap(dc[i].dirtyList, CHUNKSIZE * sizeof(uint16_t));
    }

    ANYCORE_munmap(ttm->transformChunks, ccl * sizeof(*ttm->transformChunks));
    ANYCORE_munmap(ttm->dirtyChunks, ccl * sizeof(ANYCORE_DirtyChunk));
    ANYCORE_munmap(ttm->dcsflags, ((ccl + 7) >> 3) * sizeof(uint8_t));
    ANYCORE_munmap(ttm->chunkDirtyList, ccl * sizeof(uint16_t));
}

#if SPACE == SPACE_2D
#if ANYCORE_ENABLE_SET_POSITION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_setPosition(ANYCORE* anycore, EntityID entityID, const VEC2DTYPE newPosition);
#endif
#if ANYCORE_ENABLE_SET_ROTATION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_setRotation(ANYCORE* anycore, EntityID entityID, const PRESICION newRotation);
#endif
#if ANYCORE_ENABLE_SET_SCALE
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_setScale(ANYCORE* anycore, EntityID entityID, const VEC2DTYPE newScale);
#endif

#if ANYCORE_ENABLE_GET_POSITION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_getPosition(ANYCORE* anycore, EntityID entityID, VEC2DTYPE* outPosition);
#endif
#if ANYCORE_ENABLE_GET_ROTATION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_getRotation(ANYCORE* anycore, EntityID entityID, PRESICION* outRotation);
#endif
#if ANYCORE_ENABLE_GET_SCALE
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_getScale(ANYCORE* anycore, EntityID entityID, VEC2DTYPE* outScale);
#endif

#if ANYCORE_ENABLE_ADD_POSITION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_addPosition(ANYCORE* anycore, EntityID entityID, const VEC2DTYPE value);
#endif
#if ANYCORE_ENABLE_ADD_ROTATION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_addRotation(ANYCORE* anycore, EntityID entityID, const PRESICION value);
#endif
#if ANYCORE_ENABLE_ADD_SCALE
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_addScale(ANYCORE* anycore, EntityID entityID, const VEC2DTYPE value);
#endif

#if ANYCORE_ENABLE_SET_POSITION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_setPositionBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC2DTYPE* newPositions, const uint32_t count);
#endif
#if ANYCORE_ENABLE_SET_ROTATION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_setRotationBulk(ANYCORE* anycore, EntityID* entityIDs, const PRESICION* newRotations, const uint32_t count);
#endif
#if ANYCORE_ENABLE_SET_SCALE_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_setScaleBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC2DTYPE* newScales, const uint32_t count);
#endif

#if ANYCORE_ENABLE_GET_POSITION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_getPositionBulk(ANYCORE* anycore, EntityID* entityIDs, VEC2DTYPE* outPositions, const uint32_t count);
#endif
#if ANYCORE_ENABLE_GET_ROTATION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_getRotationBulk(ANYCORE* anycore, EntityID* entityIDs, PRESICION* outRotations, const uint32_t count);
#endif
#if ANYCORE_ENABLE_GET_SCALE_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_getScaleBulk(ANYCORE* anycore, EntityID* entityIDs, VEC2DTYPE* outScales, const uint32_t count);
#endif

#if ANYCORE_ENABLE_ADD_POSITION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_addPositionBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC2DTYPE* values, const uint32_t count);
#endif
#if ANYCORE_ENABLE_ADD_ROTATION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_addRotationBulk(ANYCORE* anycore, EntityID* entityIDs, const PRESICION* values, const uint32_t count);
#endif
#if ANYCORE_ENABLE_ADD_SCALE_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_addScaleBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC2DTYPE* values, const uint32_t count);
#endif
#elif SPACE == SPACE_3D
#if ANYCORE_ENABLE_SET_POSITION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_setPosition(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE newPosition);
#endif
#if ANYCORE_ENABLE_SET_ROTATION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_setRotation(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE newRotation);
#endif
#if ANYCORE_ENABLE_SET_SCALE
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_setScale(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE newScale);
#endif

#if ANYCORE_ENABLE_GET_POSITION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_getPosition(ANYCORE* anycore, EntityID entityID, VEC3DTYPE* outPosition);
#endif
#if ANYCORE_ENABLE_GET_ROTATION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_getRotation(ANYCORE* anycore, EntityID entityID, VEC3DTYPE* outRotation);
#endif
#if ANYCORE_ENABLE_GET_SCALE
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_getScale(ANYCORE* anycore, EntityID entityID, VEC3DTYPE* outScale);
#endif

#if ANYCORE_ENABLE_ADD_POSITION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_addPosition(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE value);
#endif
#if ANYCORE_ENABLE_ADD_ROTATION
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_addRotation(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE value);
#endif
#if ANYCORE_ENABLE_ADD_SCALE
    ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_Transform_addScale(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE value);
#endif

#if ANYCORE_ENABLE_SET_POSITION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_setPositionBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* newPositions, const uint32_t count);
#endif
#if ANYCORE_ENABLE_SET_ROTATION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_setRotationBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* newRotations, const uint32_t count);
#endif
#if ANYCORE_ENABLE_SET_SCALE_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_setScaleBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* newScales, const uint32_t count);
#endif

#if ANYCORE_ENABLE_GET_POSITION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_getPositionBulk(ANYCORE* anycore, EntityID* entityIDs, VEC3DTYPE* outPositions, const uint32_t count);
#endif
#if ANYCORE_ENABLE_GET_ROTATION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_getRotationBulk(ANYCORE* anycore, EntityID* entityIDs, VEC3DTYPE* outRotations, const uint32_t count);
#endif
#if ANYCORE_ENABLE_GET_SCALE_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_getScaleBulk(ANYCORE* anycore, EntityID* entityIDs, VEC3DTYPE* outScales, const uint32_t count);
#endif

#if ANYCORE_ENABLE_ADD_POSITION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_addPositionBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* values, const uint32_t count);
#endif
#if ANYCORE_ENABLE_ADD_ROTATION_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_addRotationBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* values, const uint32_t count);
#endif
#if ANYCORE_ENABLE_ADD_SCALE_BULK
    ANYCORE_EXPORT uint32_t ANYCORE_Transform_addScaleBulk(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* values, const uint32_t count);
#endif
#endif

static inline void markDirty(ANYCORE_TransformManager* ttm, ANYCORE_DirtyChunk* dc, uint8_t* dcsflags, uint32_t page, uint32_t slot, uint32_t wordIndex, uint32_t bittMask) {
    uint32_t* dirties = dc->dirties;

    uint32_t chunkWord = page >> 3;
    uint32_t chunkBit  = 1u << (page & 7);

    if (!(dirties[wordIndex] & bittMask)) {
        dirties[wordIndex] |= bittMask;

        dc->dirtyList[dc->dirtyCount & CHUNKMASK] = slot;
        dc->dirtyCount++;

        if (!(dcsflags[chunkWord] & chunkBit)) {
            dcsflags[chunkWord] |= chunkBit;

            ttm->chunkDirtyList[ttm->chunkDirtyCount] = page;
            ttm->chunkDirtyCount++;
        }
    }
}

static inline void markCreateFlag(ANYCORE_DirtyChunk* dc, uint32_t wordIndex, uint32_t bittMask, uint32_t createbit) {
    uint32_t* createFlags = dc->createFlags;

    if (createbit) {
        createFlags[wordIndex] |= bittMask;
    }
    else {
        createFlags[wordIndex] &= ~bittMask;
    }
}

static inline void clearDirtyFrame(ANYCORE_TransformManager* ttm, uint32_t chunkCount) {
    ANYCORE_DirtyChunk* dc = ttm->dirtyChunks;

    for (uint32_t page = 0; page < chunkCount; page++) {
        ANYCORE_memset32(dc[page].dirties,     0x800);
        ANYCORE_memset32(dc[page].createFlags, 0x800);
        dc[page].dirtyCount = 0;
    }

    ANYCORE_memset8(ttm->dcsflags, (chunkCount + 7) >> 3);

    ttm->chunkDirtyCount = 0;
}

#endif // ANYCORE_TRANSFORM_MANAGER_UTILS_H