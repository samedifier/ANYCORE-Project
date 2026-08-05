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

#ifndef ANYCORE_WRAPPER_H
#define ANYCORE_WRAPPER_H

#include "primitives.h"
#include "configuration.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef _WIN32
    #include <windows.h>
    #define ANYCORE_DLOPEN(path)          (void*)LoadLibraryA(path)
    #define ANYCORE_DLSYM(h, name, field) *(void**)(&(field)) = (void*)GetProcAddress((HMODULE)(h), (name))
    #define ANYCORE_DLCLOSE(h)            FreeLibrary((HMODULE)(h))
#else
    #include <dlfcn.h>
    #define ANYCORE_DLOPEN(path)          dlopen((path), RTLD_NOW)
    #define ANYCORE_DLSYM(h, name, field) *(void**)(&(field)) = dlsym((h), (name))
    #define ANYCORE_DLCLOSE(h)            dlclose(h)
#endif

#if PRESICION_ == PRESICION_FLOAT
    #define VEC2DTYPE Vec2f
    #define VEC3DTYPE Vec3f
#elif PRESICION_ == PRESICION_DOUBLE
    #define VEC2DTYPE Vec2d
    #define VEC3DTYPE Vec3d
#endif

typedef struct ANYCORE ANYCORE;
typedef void* Behaviour;

typedef struct {
    void*    libHandle;
    ANYCORE* core;
} ANYCORE_Instance;

typedef struct {
#if ANYCORE_ENABLE_GET_BUILD_ID
    /* Returns the build ID of the ANYCORE instance. */
    uint32_t       (*getBuildID) (ANYCORE* anycore);
#endif

#if ANYCORE_ENABLE_LOAD_MODULE
    /* Loads an ANYCORE module from a shared library. */
    ANYCORE_RESULT (*loadModule)      (ANYCORE* anycore, const char* modulePath, ModuleType moduleType, double interval, void* outLibHandle);
#endif
#if ANYCORE_ENABLE_LOAD_BEHAVIOUR
    /* Loads a behaviour from a shared library. */
    ANYCORE_RESULT (*loadBehaviour)   (const char* behaviourPath, Behaviour* outBehaviour);
#endif
#if ANYCORE_ENABLE_UNLOAD_BEHAVIOUR
    /* Unloads a previously loaded behaviour library. */
    int            (*unloadBehaviour) (void* libHandle);
#endif
#if ANYCORE_ENABLE_IMPORT_FUNCTION
    /* Imports a function symbol from a loaded shared library. */
    ANYCORE_RESULT (*importFunction)  (void* libHandle, const char* functionName, void** outFunction);
#endif

    /* Registers a function to be executed when ANYCORE starts. */
    ANYCORE_RESULT (*pushStart)       (ANYCORE* anycore, void (*startFn)(ANYCORE*));

    /* Registers a function to be executed during the ANYCORE loop. */
    ANYCORE_RESULT (*pushLoop)        (ANYCORE* anycore, void (*loopFn)(ANYCORE*));

    /* Starts the ANYCORE execution loop. */
    void           (*run)             (ANYCORE* anycore);

#if ANYCORE_ENABLE_KILL_MODULE
    /* Stops and removes a loaded module. */
    ANYCORE_RESULT (*killModule)      (ANYCORE* anycore, uint32_t module);
#endif

    /* Shuts down ANYCORE and releases its resources. */
    void           (*ANYCOREshutdown) (ANYCORE* anycore);

#if ANYCORE_ENABLE_IS_ENTITY_ALIVE
    /* Checks whether an entity is currently alive. */
    bool           (*isEntityAlive)          (ANYCORE* anycore, const EntityID);
#endif
#if ANYCORE_ENABLE_IS_ENTITY_LOCKED
    /* Checks whether an entity is currently locked. */
    bool           (*isEntityLocked)         (ANYCORE* anycore, const EntityID);
#endif

#if ANYCORE_ENABLE_CREATE_ENTITY
    /* Creates a new entity from a model. */
    ANYCORE_RESULT (*createEntity)           (ANYCORE* anycore, const ModelID, EntityID*);
#endif
#if ANYCORE_ENABLE_RESTORE_ENTITY
    /* Restores a previously destroyed entity. */
    ANYCORE_RESULT (*restoreEntity)          (ANYCORE* anycore, EntityID*);
#endif
#if ANYCORE_ENABLE_DESTROY_ENTITY
    /* Destroys an entity. */
    ANYCORE_RESULT (*destroyEntity)          (ANYCORE* anycore, const EntityID);
#endif
#if ANYCORE_ENABLE_LOCK_ENTITY
    /* Locks an entity. */
    ANYCORE_RESULT (*lockEntity)             (ANYCORE* anycore, const EntityID);
#endif
#if ANYCORE_ENABLE_UNLOCK_ENTITY
    /* Unlocks an entity. */
    ANYCORE_RESULT (*unlockEntity)           (ANYCORE* anycore, const EntityID);
#endif

#if ANYCORE_ENABLE_GET_ACTIVE_CHUNK_COUNT
    /* Returns the number of active chunks. */
    uint32_t       (*getActiveChunkCount)    (ANYCORE* anycore, uint32_t);
#endif
#if ANYCORE_ENABLE_GET_LAST_CHUNK_VALID_ENTITY_COUNT
    /* Returns the number of valid entities in the last chunk. */
    uint32_t       (*getLastChunkValidEntityCount)(ANYCORE* anycore);
#endif
#if ANYCORE_ENABLE_FREE_LAST_CHUNK
    /* Frees the last chunk. */
    ANYCORE_RESULT (*freeLastChunk)          (ANYCORE* anycore);
#endif
#if ANYCORE_ENABLE_ALLOC_NEW_CHUNK
    /* Allocates a new chunk. */
    ANYCORE_RESULT (*allocNewChunk)          (ANYCORE* anycore);
#endif

#if ANYCORE_ENABLE_CREATE_ENTITY_BULK
    /* Creates multiple entities from a model in a single operation. */
    ANYCORE_RESULT (*createEntityBulk)       (ANYCORE* anycore, const ModelID, EntityID* outEntityIDs, uint32_t count);
#endif
#if ANYCORE_ENABLE_RESTORE_ENTITY_BULK
    /* Restores multiple previously destroyed entities in a single operation. */
    ANYCORE_RESULT (*restoreEntityBulk)      (ANYCORE* anycore, EntityID* entityIDs, uint32_t count);
#endif
#if ANYCORE_ENABLE_DESTROY_ENTITY_BULK
    /* Destroys multiple entities in a single operation. */
    ANYCORE_RESULT (*destroyEntityBulk)      (ANYCORE* anycore, EntityID* entityIDs, uint32_t count);
#endif
#if ANYCORE_ENABLE_LOCK_ENTITY_BULK
    /* Locks multiple entities in a single operation. */
    ANYCORE_RESULT (*lockEntityBulk)         (ANYCORE* anycore, EntityID* entityIDs, uint32_t count);
#endif
#if ANYCORE_ENABLE_UNLOCK_ENTITY_BULK
    /* Unlocks multiple entities in a single operation. */
    ANYCORE_RESULT (*unlockEntityBulk)       (ANYCORE* anycore, EntityID* entityIDs, uint32_t count);
#endif

#if ANYCORE_ENABLE_DESTROY_ALL_ENTITIES
    /* Destroys all entities in the scene. */
    uint32_t (*destroyAllEntities)(ANYCORE* anycore);
#endif
#if ANYCORE_ENABLE_LOCK_ALL_ENTITIES
    /* Locks all entities in the scene. */
    uint32_t (*lockAllEntities)(ANYCORE* anycore);
#endif
#if ANYCORE_ENABLE_UNLOCK_ALL_ENTITIES
    /* Unlocks all entities in the scene. */
    uint32_t (*unlockAllEntities)(ANYCORE* anycore);
#endif
    
#if ANYCORE_ENABLE_ADD_MODEL
    /* Adds a new model to ANYCORE. */
    ANYCORE_RESULT (*addModel)   (ANYCORE* anycore, void* vertices, const uint32_t vertexCount, uint32_t* indices, const uint32_t indexCount, ModelID* outModelID);
#endif
#if ANYCORE_ENABLE_SET_MODEL
    /* Assigns a model to an entity. */
    ANYCORE_RESULT (*setModel)   (ANYCORE* anycore, const EntityID entityID, const ModelID modelID);
#endif
#if ANYCORE_ENABLE_UPDATE_MODEL
    /* Replaces the data of an existing model. */
    ANYCORE_RESULT (*updateModel)(ANYCORE* anycore, const ModelID modelID, const void* newVertices, const uint32_t newVertexCount, const uint32_t* newIndices, const uint32_t newIndexCount);
#endif
#if ANYCORE_ENABLE_REMOVE_MODEL
    /* Removes a model from ANYCORE. */
    ANYCORE_RESULT (*removeModel) (ANYCORE* anycore, ModelID modelID);
#endif

#if SPACE == SPACE_2D

#if ANYCORE_ENABLE_SET_POSITION
    /* Sets the position of an entity. */
    ANYCORE_RESULT (*setPosition)(ANYCORE* anycore, EntityID entityID, const VEC2DTYPE newPosition);
#endif
#if ANYCORE_ENABLE_SET_ROTATION
    /* Sets the rotation of an entity. */
    ANYCORE_RESULT (*setRotation)(ANYCORE* anycore, EntityID entityID, const PRESICION newRotation);
#endif
#if ANYCORE_ENABLE_SET_SCALE
    /* Sets the scale of an entity. */
    ANYCORE_RESULT (*setScale)   (ANYCORE* anycore, EntityID entityID, const VEC2DTYPE newScale);
#endif
#if ANYCORE_ENABLE_GET_POSITION
    /* Gets the position of an entity. */
    ANYCORE_RESULT (*getPosition)(ANYCORE* anycore, EntityID entityID, VEC2DTYPE* outPosition);
#endif
#if ANYCORE_ENABLE_GET_ROTATION
    /* Gets the rotation of an entity. */
    ANYCORE_RESULT (*getRotation)(ANYCORE* anycore, EntityID entityID, PRESICION* outRotation);
#endif
#if ANYCORE_ENABLE_GET_SCALE
    /* Gets the scale of an entity. */
    ANYCORE_RESULT (*getScale)   (ANYCORE* anycore, EntityID entityID, VEC2DTYPE* outScale);
#endif
#if ANYCORE_ENABLE_ADD_POSITION
    /* Adds a value to the position of an entity. */
    ANYCORE_RESULT (*addPosition)(ANYCORE* anycore, EntityID entityID, const VEC2DTYPE value);
#endif
#if ANYCORE_ENABLE_ADD_ROTATION
    /* Adds a value to the rotation of an entity. */
    ANYCORE_RESULT (*addRotation)(ANYCORE* anycore, EntityID entityID, const PRESICION value);
#endif
#if ANYCORE_ENABLE_ADD_SCALE
    /* Adds a value to the scale of an entity. */
    ANYCORE_RESULT (*addScale)   (ANYCORE* anycore, EntityID entityID, const VEC2DTYPE value);
#endif

#if ANYCORE_ENABLE_SET_POSITION_BULK
    /* Sets the positions of multiple entities in a single operation. */
    uint32_t (*setPositionBulk)(ANYCORE* anycore, EntityID* entityIDs, const VEC2DTYPE* newPositions, const uint32_t count);
#endif
#if ANYCORE_ENABLE_SET_ROTATION_BULK
    /* Sets the rotations of multiple entities in a single operation. */
    uint32_t (*setRotationBulk)(ANYCORE* anycore, EntityID* entityIDs, const PRESICION* newRotations, const uint32_t count);
#endif
#if ANYCORE_ENABLE_SET_SCALE_BULK
    /* Sets the scales of multiple entities in a single operation. */
    uint32_t (*setScaleBulk)   (ANYCORE* anycore, EntityID* entityIDs, const VEC2DTYPE* newScales, const uint32_t count);
#endif
#if ANYCORE_ENABLE_GET_POSITION_BULK
    /* Gets the positions of multiple entities in a single operation. */
    uint32_t (*getPositionBulk)(ANYCORE* anycore, EntityID* entityIDs, VEC2DTYPE* outPositions, const uint32_t count);
#endif
#if ANYCORE_ENABLE_GET_ROTATION_BULK
    /* Gets the rotations of multiple entities in a single operation. */
    uint32_t (*getRotationBulk)(ANYCORE* anycore, EntityID* entityIDs, PRESICION* outRotations, const uint32_t count);
#endif
#if ANYCORE_ENABLE_GET_SCALE_BULK
    /* Gets the scales of multiple entities in a single operation. */
    uint32_t (*getScaleBulk)   (ANYCORE* anycore, EntityID* entityIDs, VEC2DTYPE* outScales, const uint32_t count);
#endif
#if ANYCORE_ENABLE_ADD_POSITION_BULK
    /* Adds values to the positions of multiple entities in a single operation. */
    uint32_t (*addPositionBulk)(ANYCORE* anycore, EntityID* entityIDs, const VEC2DTYPE* values, const uint32_t count);
#endif
#if ANYCORE_ENABLE_ADD_ROTATION_BULK
    /* Adds values to the rotations of multiple entities in a single operation. */
    uint32_t (*addRotationBulk)(ANYCORE* anycore, EntityID* entityIDs, const PRESICION* values, const uint32_t count);
#endif
#if ANYCORE_ENABLE_ADD_SCALE_BULK
    /* Adds values to the scales of multiple entities in a single operation. */
    uint32_t (*addScaleBulk)   (ANYCORE* anycore, EntityID* entityIDs, const VEC2DTYPE* values, const uint32_t count);
#endif

#elif SPACE == SPACE_3D

#if ANYCORE_ENABLE_SET_POSITION
    /* Sets the position of an entity. */
    ANYCORE_RESULT (*setPosition)(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE newPosition);
#endif
#if ANYCORE_ENABLE_SET_ROTATION
    /* Sets the rotation of an entity. */
    ANYCORE_RESULT (*setRotation)(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE newRotation);
#endif
#if ANYCORE_ENABLE_SET_SCALE
    /* Sets the scale of an entity. */
    ANYCORE_RESULT (*setScale)   (ANYCORE* anycore, EntityID entityID, const VEC3DTYPE newScale);
#endif
#if ANYCORE_ENABLE_GET_POSITION
    /* Gets the position of an entity. */
    ANYCORE_RESULT (*getPosition)(ANYCORE* anycore, EntityID entityID, VEC3DTYPE* outPosition);
#endif
#if ANYCORE_ENABLE_GET_ROTATION
    /* Gets the rotation of an entity. */
    ANYCORE_RESULT (*getRotation)(ANYCORE* anycore, EntityID entityID, VEC3DTYPE* outRotation);
#endif
#if ANYCORE_ENABLE_GET_SCALE
    /* Gets the scale of an entity. */
    ANYCORE_RESULT (*getScale)   (ANYCORE* anycore, EntityID entityID, VEC3DTYPE* outScale);
#endif
#if ANYCORE_ENABLE_ADD_POSITION
    /* Adds a value to the position of an entity. */
    ANYCORE_RESULT (*addPosition)(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE value);
#endif
#if ANYCORE_ENABLE_ADD_ROTATION
    /* Adds a value to the rotation of an entity. */
    ANYCORE_RESULT (*addRotation)(ANYCORE* anycore, EntityID entityID, const VEC3DTYPE value);
#endif
#if ANYCORE_ENABLE_ADD_SCALE
    /* Adds a value to the scale of an entity. */
    ANYCORE_RESULT (*addScale)   (ANYCORE* anycore, EntityID entityID, const VEC3DTYPE value);
#endif

#if ANYCORE_ENABLE_SET_POSITION_BULK
    /* Sets the positions of multiple entities in a single operation. */
    uint32_t (*setPositionBulk)(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* newPositions, const uint32_t count);
#endif
#if ANYCORE_ENABLE_SET_ROTATION_BULK
    /* Sets the rotations of multiple entities in a single operation. */
    uint32_t (*setRotationBulk)(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* newRotations, const uint32_t count);
#endif
#if ANYCORE_ENABLE_SET_SCALE_BULK
    /* Sets the scales of multiple entities in a single operation. */
    uint32_t (*setScaleBulk)   (ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* newScales, const uint32_t count);
#endif
#if ANYCORE_ENABLE_GET_POSITION_BULK
    /* Gets the positions of multiple entities in a single operation. */
    uint32_t (*getPositionBulk)(ANYCORE* anycore, EntityID* entityIDs, VEC3DTYPE* outPositions, const uint32_t count);
#endif
#if ANYCORE_ENABLE_GET_ROTATION_BULK
    /* Gets the rotations of multiple entities in a single operation. */
    uint32_t (*getRotationBulk)(ANYCORE* anycore, EntityID* entityIDs, VEC3DTYPE* outRotations, const uint32_t count);
#endif
#if ANYCORE_ENABLE_GET_SCALE_BULK
    /* Gets the scales of multiple entities in a single operation. */
    uint32_t (*getScaleBulk)   (ANYCORE* anycore, EntityID* entityIDs, VEC3DTYPE* outScales, const uint32_t count);
#endif
#if ANYCORE_ENABLE_ADD_POSITION_BULK
    /* Adds values to the positions of multiple entities in a single operation. */
    uint32_t (*addPositionBulk)(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* values, const uint32_t count);
#endif
#if ANYCORE_ENABLE_ADD_ROTATION_BULK
    /* Adds values to the rotations of multiple entities in a single operation. */
    uint32_t (*addRotationBulk)(ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* values, const uint32_t count);
#endif
#if ANYCORE_ENABLE_ADD_SCALE_BULK
    /* Adds values to the scales of multiple entities in a single operation. */
    uint32_t (*addScaleBulk)   (ANYCORE* anycore, EntityID* entityIDs, const VEC3DTYPE* values, const uint32_t count);
#endif
#endif
} ANYCORE_Wrapper;

static inline bool ANYCORE_load(ANYCORE_Instance* out, const char* path,
    uint32_t chunkCountLimit, uint32_t modelChunkCountLimit, uint32_t startCapacity,
    uint32_t loopCapacity, uint32_t moduleCapacity, uint32_t stride) {
    if (!out || !path) { return false; }

    out->libHandle = ANYCORE_DLOPEN(path);
    if (!out->libHandle) return false;

    ANYCORE_RESULT (*init)(ANYCORE**, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) = NULL;
    ANYCORE_DLSYM(out->libHandle, "initANYCORE", init);
    if (!init) { ANYCORE_DLCLOSE(out->libHandle); return false; }

    ANYCORE_RESULT r = init(&out->core, chunkCountLimit, modelChunkCountLimit, startCapacity, loopCapacity, moduleCapacity, stride);
    if (r != ANYCORE_SUCCESS) {
        ANYCORE_DLCLOSE(out->libHandle);
        return false;
    }

    return true;
}

static inline void ANYCORE_unload(ANYCORE_Instance* inst) {
    if (!inst) { return; }
    if (inst->libHandle) { ANYCORE_DLCLOSE(inst->libHandle); }
    inst->core      = NULL;
    inst->libHandle = NULL;
}

static inline bool ANYCORE_Wrapper_init(ANYCORE_Wrapper* w, void* libHandle) {
    if (!w || !libHandle) { return false; }

#if ANYCORE_ENABLE_GET_BUILD_ID
    ANYCORE_DLSYM(libHandle, "getANYCOREBuildID", w->getBuildID);
#endif

#if ANYCORE_ENABLE_LOAD_MODULE
    ANYCORE_DLSYM(libHandle, "ANYCORE_loadModule",      w->loadModule);
#endif
#if ANYCORE_ENABLE_LOAD_BEHAVIOUR
    ANYCORE_DLSYM(libHandle, "ANYCORE_loadBehaviour",   w->loadBehaviour);
#endif
#if ANYCORE_ENABLE_UNLOAD_BEHAVIOUR
    ANYCORE_DLSYM(libHandle, "ANYCORE_unloadBehaviour", w->unloadBehaviour);
#endif
#if ANYCORE_ENABLE_IMPORT_FUNCTION
    ANYCORE_DLSYM(libHandle, "ANYCORE_importFunction",  w->importFunction);
#endif
    ANYCORE_DLSYM(libHandle, "ANYCORE_pushStart",       w->pushStart);
    ANYCORE_DLSYM(libHandle, "ANYCORE_pushLoop",        w->pushLoop);
    ANYCORE_DLSYM(libHandle, "ANYCORE_run",             w->run);
#if ANYCORE_ENABLE_KILL_MODULE
    ANYCORE_DLSYM(libHandle, "ANYCORE_killModule",      w->killModule);
#endif
    ANYCORE_DLSYM(libHandle, "ANYCORE_shutdown",        w->ANYCOREshutdown);

#if ANYCORE_ENABLE_IS_ENTITY_ALIVE
    ANYCORE_DLSYM(libHandle, "ANYCORE_isEntityAlive",           w->isEntityAlive);
#endif
#if ANYCORE_ENABLE_IS_ENTITY_LOCKED
    ANYCORE_DLSYM(libHandle, "ANYCORE_isEntityLocked",          w->isEntityLocked);
#endif
#if ANYCORE_ENABLE_CREATE_ENTITY
    ANYCORE_DLSYM(libHandle, "ANYCORE_createEntity",            w->createEntity);
#endif
#if ANYCORE_ENABLE_RESTORE_ENTITY
    ANYCORE_DLSYM(libHandle, "ANYCORE_restoreEntity",           w->restoreEntity);
#endif
#if ANYCORE_ENABLE_DESTROY_ENTITY
    ANYCORE_DLSYM(libHandle, "ANYCORE_destroyEntity",           w->destroyEntity);
#endif
#if ANYCORE_ENABLE_LOCK_ENTITY
    ANYCORE_DLSYM(libHandle, "ANYCORE_lockEntity",              w->lockEntity);
#endif
#if ANYCORE_ENABLE_UNLOCK_ENTITY
    ANYCORE_DLSYM(libHandle, "ANYCORE_unlockEntity",            w->unlockEntity);
#endif
#if ANYCORE_ENABLE_GET_ACTIVE_CHUNK_COUNT
    ANYCORE_DLSYM(libHandle, "ANYCORE_getActiveChunkCount",     w->getActiveChunkCount);
#endif
#if ANYCORE_ENABLE_GET_LAST_CHUNK_VALID_ENTITY_COUNT
    ANYCORE_DLSYM(libHandle, "ANYCORE_getLastChunkValidEntityCount", w->getLastChunkValidEntityCount);
#endif
#if ANYCORE_ENABLE_FREE_LAST_CHUNK
    ANYCORE_DLSYM(libHandle, "ANYCORE_freeLastChunk",           w->freeLastChunk);
#endif
#if ANYCORE_ENABLE_ALLOC_NEW_CHUNK
    ANYCORE_DLSYM(libHandle, "ANYCORE_allocNewChunk",           w->allocNewChunk);
#endif

#if ANYCORE_ENABLE_CREATE_ENTITY_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_createEntityBulk",        w->createEntityBulk);
#endif
#if ANYCORE_ENABLE_RESTORE_ENTITY_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_restoreEntityBulk",       w->restoreEntityBulk);
#endif
#if ANYCORE_ENABLE_DESTROY_ENTITY_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_destroyEntityBulk",       w->destroyEntityBulk);
#endif
#if ANYCORE_ENABLE_LOCK_ENTITY_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_lockEntityBulk",          w->lockEntityBulk);
#endif
#if ANYCORE_ENABLE_UNLOCK_ENTITY_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_unlockEntityBulk",        w->unlockEntityBulk);
#endif

#if ANYCORE_ENABLE_DESTROY_ALL_ENTITIES
    ANYCORE_DLSYM(libHandle, "ANYCORE_destroyAllEntities",      w->destroyAllEntities);
#endif
#if ANYCORE_ENABLE_LOCK_ALL_ENTITIES
    ANYCORE_DLSYM(libHandle, "ANYCORE_lockAllEntities",         w->lockAllEntities);
#endif
#if ANYCORE_ENABLE_UNLOCK_ALL_ENTITIES
    ANYCORE_DLSYM(libHandle, "ANYCORE_unlockAllEntities",       w->unlockAllEntities);
#endif

#if ANYCORE_ENABLE_ADD_MODEL
    ANYCORE_DLSYM(libHandle, "ANYCORE_VertexManager_addModel",    w->addModel);
#endif
#if ANYCORE_ENABLE_SET_MODEL
    ANYCORE_DLSYM(libHandle, "ANYCORE_VertexManager_setModel",    w->setModel);
#endif
#if ANYCORE_ENABLE_UPDATE_MODEL
    ANYCORE_DLSYM(libHandle, "ANYCORE_VertexManager_updateModel", w->updateModel);
#endif
#if ANYCORE_ENABLE_REMOVE_MODEL
    ANYCORE_DLSYM(libHandle, "ANYCORE_VertexManager_removeModel", w->removeModel);
#endif

#if ANYCORE_ENABLE_SET_POSITION
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_setPosition", w->setPosition);
#endif
#if ANYCORE_ENABLE_SET_ROTATION
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_setRotation", w->setRotation);
#endif
#if ANYCORE_ENABLE_SET_SCALE
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_setScale",    w->setScale);
#endif
#if ANYCORE_ENABLE_GET_POSITION
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_getPosition", w->getPosition);
#endif
#if ANYCORE_ENABLE_GET_ROTATION
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_getRotation", w->getRotation);
#endif
#if ANYCORE_ENABLE_GET_SCALE
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_getScale",    w->getScale);
#endif
#if ANYCORE_ENABLE_ADD_POSITION
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_addPosition", w->addPosition);
#endif
#if ANYCORE_ENABLE_ADD_ROTATION
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_addRotation", w->addRotation);
#endif
#if ANYCORE_ENABLE_ADD_SCALE
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_addScale",    w->addScale);
#endif

#if ANYCORE_ENABLE_SET_POSITION_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_setPositionBulk", w->setPositionBulk);
#endif
#if ANYCORE_ENABLE_SET_ROTATION_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_setRotationBulk", w->setRotationBulk);
#endif
#if ANYCORE_ENABLE_SET_SCALE_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_setScaleBulk",    w->setScaleBulk);
#endif
#if ANYCORE_ENABLE_GET_POSITION_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_getPositionBulk", w->getPositionBulk);
#endif
#if ANYCORE_ENABLE_GET_ROTATION_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_getRotationBulk", w->getRotationBulk);
#endif
#if ANYCORE_ENABLE_GET_SCALE_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_getScaleBulk",    w->getScaleBulk);
#endif
#if ANYCORE_ENABLE_ADD_POSITION_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_addPositionBulk", w->addPositionBulk);
#endif
#if ANYCORE_ENABLE_ADD_ROTATION_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_addRotationBulk", w->addRotationBulk);
#endif
#if ANYCORE_ENABLE_ADD_SCALE_BULK
    ANYCORE_DLSYM(libHandle, "ANYCORE_Transform_addScaleBulk",    w->addScaleBulk);
#endif

    return true;
}

static inline void ANYCORE_injectWrapper(ANYCORE_Wrapper* w, Behaviour bh) {
    void (*injectANYCOREWrapper)(void*) = NULL;
    ANYCORE_DLSYM(bh, "injectANYCOREWrapper", injectANYCOREWrapper);
    if (injectANYCOREWrapper) { injectANYCOREWrapper(w); }
}

#endif // ANYCORE_WRAPPER_H