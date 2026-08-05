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

#include "anycore_vertex_manager_utils.h"
#include <string.h>

#if ANYCORE_ENABLE_ADD_MODEL
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_VertexManager_addModel(ANYCORE* anycore, void* vertices, const uint32_t vertexCount, uint32_t* indices, const uint32_t indexCount, ModelID* outModelID) {
    ANYCORE_VertexManager* vm = &anycore->vertexManager;

    uint32_t modelCount = vm->modelCount;
    uint32_t chunk = modelCount >> CHUNKSHIFT;

    if (chunk >= vm->modelChunkCountLimit) { return ANYCORE_ERR_CHUNK_LIMIT; }
    uint32_t slot  = modelCount &  CHUNKMASK;

    uint32_t prec1 = CHUNKSIZE * sizeof(ANYCORE_Model);
    uint32_t prec2 = 0X800     * sizeof(uint32_t);
    uint32_t prec3 = CHUNKSIZE * sizeof(uint16_t);

    if (slot == 0) {
        ANYCORE_Model* models    = ANYCORE_mmap(prec1);
        uint32_t* modelDirties   = ANYCORE_mmap(prec2);
        uint16_t* modelDirtyList = ANYCORE_mmap(prec3);

        if (models == ANYCORE_MAP_FAILED || modelDirties == ANYCORE_MAP_FAILED || modelDirtyList == ANYCORE_MAP_FAILED) {
            if (models         != ANYCORE_MAP_FAILED) { ANYCORE_munmap(models,         prec1); }
            if (modelDirties   != ANYCORE_MAP_FAILED) { ANYCORE_munmap(modelDirties,   prec2); }
            if (modelDirtyList != ANYCORE_MAP_FAILED) { ANYCORE_munmap(modelDirtyList, prec3); }
            return ANYCORE_ERR_ALLOC_FAILED; 
        }

        vm->modelChunks[chunk].models = models;

        ANYCORE_ModelDirtyChunk* modeldirtychunk = &vm->modelDirtyChunks[chunk];

        modeldirtychunk->modelDirties    = modelDirties;
        modeldirtychunk->modelDirtyList  = modelDirtyList;
        modeldirtychunk->modelDirtyCount = 0;
    }

    ANYCORE_Model* m = &vm->modelChunks[chunk].models[slot];

    m->vertices    = vertices;
    m->vertexCount = vertexCount;
    m->indices     = indices;
    m->indexCount  = indexCount;

    if (outModelID) { *outModelID = modelCount; }

    markModelDirty(vm, chunk, slot, slot >> 5, 1u << (slot & 31));

    vm->modelCount++;
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_SET_MODEL
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_VertexManager_setModel(ANYCORE* anycore, const EntityID entityID, const ModelID modelID) {
    ANYCORE_SceneManager* sm = &anycore->sceneManager;

    uint32_t gslot = entityID.slot;
    uint32_t chunk = gslot >> CHUNKSHIFT;
    uint32_t slot  = gslot &  CHUNKMASK;
    uint32_t mask  = 1u << (slot & 31);
    uint32_t prec1 = slot >> 5;

    if (chunk >= sm->chunkcount) { return ANYCORE_ERR_ENTITY_DEAD; }
    ANYCORE_SceneChunk* sc = &sm->sceneChunks[chunk];

    if (!((sc->validFlags[prec1] & mask) &&
    (sc->generations[slot] == entityID.generation))) { return ANYCORE_ERR_ENTITY_DEAD; }

    anycore->vertexManager.instanceChunks[chunk].instances[slot] = modelID;

    ANYCORE_TransformManager* ttm = &anycore->transformManager;
    ANYCORE_DirtyChunk* dc = ttm->dirtyChunks;
    markDirty(ttm, dc, ttm->dcsflags, chunk, slot, prec1, mask);
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_UPDATE_MODEL
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_VertexManager_updateModel(ANYCORE* anycore, const ModelID modelID, const void* newVertices, const uint32_t newVertexCount, const uint32_t* newIndices, const uint32_t newIndexCount) {
    ANYCORE_VertexManager* vm = &anycore->vertexManager;

    if (modelID >= vm->modelCount) { return ANYCORE_ERR_INVALID_MODEL_ID; }

    uint32_t chunk = modelID >> CHUNKSHIFT;
    uint32_t slot  = modelID &  CHUNKMASK;

    ANYCORE_Model* m = &vm->modelChunks[chunk].models[slot];
    if (!m->vertices) { return ANYCORE_ERR_MODEL_NULL; }

    m->vertexCount = newVertexCount;
    m->indexCount  = newIndexCount;

    memcpy(m->vertices, newVertices, newVertexCount * vm->stride);
    if (newIndices && m->indices) {
        memcpy(m->indices, newIndices, newIndexCount * sizeof(uint32_t));
    }

    uint32_t mask  = 1u << (slot & 31);
    uint32_t windx = slot >> 5;

    markModelDirty(vm, chunk, slot, windx, mask);
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_REMOVE_MODEL
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_VertexManager_removeModel(ANYCORE* anycore, const ModelID modelID) {
    ANYCORE_VertexManager* vm = &anycore->vertexManager;

    if (modelID >= vm->modelCount) { return ANYCORE_ERR_INVALID_MODEL_ID; }

    uint32_t chunk = modelID >> CHUNKSHIFT;
    uint32_t slot  = modelID &  CHUNKMASK;

    ANYCORE_Model* m = &vm->modelChunks[chunk].models[slot];
    if (!m->vertices) { return ANYCORE_ERR_MODEL_NULL; }

    m->vertices     = NULL;
    m->vertexCount  = 0;

    uint32_t mask  = 1u << (slot & 31);
    uint32_t windx = slot >> 5;

    markModelDirty(vm, chunk, slot, windx, mask);
    return ANYCORE_SUCCESS;
}
#endif
