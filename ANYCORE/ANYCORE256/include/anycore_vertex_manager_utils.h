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

#ifndef ANYCORE_VERTEX_MANAGER_UTILS_H
#define ANYCORE_VERTEX_MANAGER_UTILS_H

#include "anycore_vertex_manager.h"
#include "anycore_time_manager_utils.h"
#include "anycore_transform_manager_utils.h"
#include "anycore.h"

static inline ANYCORE_RESULT VertexManager_onInit(ANYCORE* anycore, uint32_t stride, uint32_t ccl, uint32_t mccl) {
    ANYCORE_VertexManager* vm = &anycore->vertexManager;

    uint32_t prec1 = ccl  * sizeof(ANYCORE_InstanceChunk);
    uint32_t prec2 = mccl * sizeof(ANYCORE_ModelChunk);
    uint32_t prec3 = mccl * sizeof(ANYCORE_ModelDirtyChunk);

    uint32_t prec4 = mccl * sizeof(uint16_t);
    uint32_t prec5 = ((mccl + 7) >> 3) * sizeof(uint8_t);

    ANYCORE_InstanceChunk*   instances    = ANYCORE_mmap(prec1);
    ANYCORE_ModelChunk*      models       = ANYCORE_mmap(prec2);
    ANYCORE_ModelDirtyChunk* modelDirties = ANYCORE_mmap(prec3);

    uint16_t* mchunkdirtylist = ANYCORE_mmap(prec4);
    uint8_t*  mdcsflags       = ANYCORE_mmap(prec5);


    if (instances        == ANYCORE_MAP_FAILED || models    == ANYCORE_MAP_FAILED || modelDirties == ANYCORE_MAP_FAILED ||
        mchunkdirtylist  == ANYCORE_MAP_FAILED || mdcsflags == ANYCORE_MAP_FAILED) {
        if (instances    != ANYCORE_MAP_FAILED) { ANYCORE_munmap(instances,    prec1); }
        if (models       != ANYCORE_MAP_FAILED) { ANYCORE_munmap(models,       prec2); }
        if (modelDirties != ANYCORE_MAP_FAILED) { ANYCORE_munmap(modelDirties, prec3); }

        if (mchunkdirtylist  != ANYCORE_MAP_FAILED) { ANYCORE_munmap(mchunkdirtylist,  prec4); }
        if (mdcsflags        != ANYCORE_MAP_FAILED) { ANYCORE_munmap(mdcsflags,        prec5); }
        return ANYCORE_ERR_ALLOC_FAILED;
    }

    ANYCORE_memset8(mdcsflags, (ccl + 7) >> 3);

    modelDirties->modelDirtyCount = 0;

    vm->instanceChunks   = instances;
    vm->modelChunks      = models;
    vm->modelDirtyChunks = modelDirties;

    vm->modelCount = 0;
    vm->stride = stride;
    vm->modelChunkCountLimit = mccl;

    vm->mdcsflags = mdcsflags;

    vm->chunkModelDirtyList  = mchunkdirtylist;
    vm->chunkModelDirtyCount = 0;


    return ANYCORE_SUCCESS;
}

static inline ANYCORE_RESULT VertexManager_onGrow(ANYCORE* anycore, const uint32_t i) {
    uint32_t prec = CHUNKSIZE * sizeof(uint32_t);

    uint32_t* instancesi = ANYCORE_mmap(prec);
    if (instancesi == ANYCORE_MAP_FAILED) {
        return ANYCORE_ERR_ALLOC_FAILED;
    }

    anycore->vertexManager.instanceChunks[i].instances = instancesi;
    return ANYCORE_SUCCESS;
}

static inline void VertexManager_onFree(ANYCORE* anycore) {
    ANYCORE_VertexManager* vm = &anycore->vertexManager;
    ANYCORE_SceneManager* sm  = &anycore->sceneManager;
    uint32_t ccl        = sm->chunkCountLimit;
    uint32_t mccl       = vm->modelChunkCountLimit;
    uint32_t chunkcount = sm->chunkcount;

    uint32_t prec1 = CHUNKSIZE * sizeof(uint32_t);
    uint32_t prec2 = CHUNKSIZE * sizeof(ANYCORE_Model);
    uint32_t prec3 = ccl  * sizeof(ANYCORE_InstanceChunk);
    uint32_t prec4 = mccl * sizeof(ANYCORE_ModelChunk);
    uint32_t prec5 = mccl * sizeof(ANYCORE_ModelDirtyChunk);

    for (uint32_t i = 0; i < chunkcount; i++) {
        if (vm->instanceChunks[i].instances) {
            ANYCORE_munmap(vm->instanceChunks[i].instances, prec1);
        }
    }

    uint32_t chunksFull = vm->modelCount >> CHUNKSHIFT;
    uint32_t lastCount  = vm->modelCount &  CHUNKMASK;

    for (uint32_t i = 0; i < chunksFull; i++) {
        ANYCORE_ModelChunk* mc = &vm->modelChunks[i];
        for (uint32_t j = 0; j < CHUNKSIZE; j++) {
            if (mc->models[j].vertices && mc->models[j].vertices) {
                ANYCORE_munmap(mc->models[j].vertices, mc->models[j].vertexCount * vm->stride);
                ANYCORE_munmap(mc->models[j].indices,  mc->models[j].indexCount  * sizeof(uint32_t));
            }
        }
        ANYCORE_munmap(mc->models, prec2);
    }

    if (lastCount > 0) {
        ANYCORE_ModelChunk* mc = &vm->modelChunks[chunksFull];
        for (uint32_t j = 0; j < lastCount; j++) {
            if (mc->models[j].vertices) {
                ANYCORE_munmap(mc->models[j].vertices, mc->models[j].vertexCount * vm->stride);
                ANYCORE_munmap(mc->models[j].indices,  mc->models[j].indexCount  * sizeof(uint32_t));
            }
        }
        ANYCORE_munmap(mc->models, prec2);
    }

    ANYCORE_munmap(vm->instanceChunks,   prec3);
    ANYCORE_munmap(vm->modelChunks,      prec4);
    ANYCORE_munmap(vm->modelDirtyChunks, prec5);

    ANYCORE_munmap(vm->mdcsflags, ((mccl + 7) >> 3) * sizeof(uint8_t));
    ANYCORE_munmap(vm->chunkModelDirtyList, mccl * sizeof(uint16_t));
}

#if ANYCORE_ENABLE_ADD_MODEL
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_VertexManager_addModel(ANYCORE* anycore, void* vertices, const uint32_t vertexCount, uint32_t* indices, const uint32_t indexCount, ModelID* outModelID);
#endif

#if ANYCORE_ENABLE_SET_MODEL
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_VertexManager_setModel(ANYCORE* anycore, const EntityID entityID, const ModelID modelID);
#endif

#if ANYCORE_ENABLE_UPDATE_MODEL
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_VertexManager_updateModel(ANYCORE* anycore, const ModelID modelID, const void* newVertices, const uint32_t newVertexCount, const uint32_t* newIndices, const uint32_t newIndexCount);
#endif

#if ANYCORE_ENABLE_REMOVE_MODEL
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_VertexManager_removeModel(ANYCORE* anycore, const ModelID modelID);
#endif

static inline void VertexManager_UpdateExportStream(ANYCORE* anycore, ANYCORE_Stream* out) {
    ANYCORE_TimeManager*      im = &anycore->timeManager;
    ANYCORE_SceneManager*     sm = &anycore->sceneManager;
    ANYCORE_TransformManager* tm = &anycore->transformManager;
    ANYCORE_VertexManager*    vm = &anycore->vertexManager;

    ANYCORE_TimeManager_Update(im);

    out->frame                = im->frame;
    out->elapsed              = im->elapsed;
    out->deltaTime            = im->deltaTime;

    out->transformChunks      = (void*)tm->transformChunks;

    out->instanceChunks       = vm->instanceChunks;
    out->modelChunks          = vm->modelChunks;

    out->modelCount           = vm->modelCount;
    out->stride               = vm->stride;

    out->sceneChunks          = sm->sceneChunks;

    out->dirtyChunks          = tm->dirtyChunks;
    out->modelDirtyChunks     = vm->modelDirtyChunks;

    out->chunkDirtyList       = tm->chunkDirtyList;
    out->chunkDirtyCount      = tm->chunkDirtyCount;

    out->chunkModelDirtyList  = vm->chunkModelDirtyList;
    out->chunkModelDirtyCount = vm->chunkModelDirtyCount;

    out->activeEntityCount    = sm->dsize;
    out->chunkCount           = sm->chunkcount;
    out->chunkLimit           = sm->chunkCountLimit;
    out->modelChunkLimit      = vm->modelChunkCountLimit;
}

static inline void markModelDirty(ANYCORE_VertexManager* vm, uint32_t page, uint32_t slot, uint32_t wordIndex, uint32_t bittMask) {    
    ANYCORE_ModelDirtyChunk* mdc = &vm->modelDirtyChunks[page];
    uint32_t* dirties = mdc->modelDirties;

    if (!(dirties[wordIndex] & bittMask)) {
        dirties[wordIndex] |= bittMask;

        mdc->modelDirtyList[mdc->modelDirtyCount & CHUNKMASK] = slot;
        mdc->modelDirtyCount++;

        uint32_t chunkWord = page >> 3;
        uint32_t chunkBit  = 1u << (page & 7);

        uint8_t* dcsflags = vm->mdcsflags;

        if (!(dcsflags[chunkWord] & chunkBit)) {
            dcsflags[chunkWord] |= chunkBit;

            vm->chunkModelDirtyList[vm->chunkModelDirtyCount] = page;
            vm->chunkModelDirtyCount++;
        }
    }
}

static inline void clearModelDirtyFrame(ANYCORE_VertexManager* vm, uint32_t chunkCount) {
    ANYCORE_ModelDirtyChunk* mdc = vm->modelDirtyChunks;

    for (uint32_t page = 0; page < chunkCount; page++) {
        ANYCORE_memset32(mdc[page].modelDirties, 0x800);
        mdc[page].modelDirtyCount = 0;
    }

    ANYCORE_memset8(vm->mdcsflags, (chunkCount + 7) >> 3);

    vm->chunkModelDirtyCount = 0;
}

#endif // ANYCORE_VERTEX_MANAGER_UTILS_H