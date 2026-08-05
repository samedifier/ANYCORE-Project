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

#include "anycore_app_manager_utils.h"
#include "anycore_scene_manager_utils.h"
#include "anycore_utils.h"
#include <stdio.h>

void* ANYCORE_moduleWorker(void* arg) {
    Module* m = (Module*)arg;

    if (m->type == MODULE_INDEPENDENT) {
        while (m->running) {
            if (m->loop) { m->loop(); }
        }
        return NULL;
    }

    while (m->running) {
        pthread_mutex_lock(&m->runMutex);
        while (!m->readyToWork && m->running) {
            pthread_cond_wait(&m->runCond, &m->runMutex);
        }

        if (!m->running) {
            pthread_mutex_unlock(&m->runMutex);
            break;
        }

        m->readyToWork = false;
        pthread_mutex_unlock(&m->runMutex);

        if (m->loop) { m->loop(); }

        pthread_mutex_lock(&m->doneMutex);
        m->isDone = true;
        pthread_cond_signal(&m->doneCond);
        pthread_mutex_unlock(&m->doneMutex);
    }

    return NULL;
}

ANYCORE_RESULT initAppManager(ANYCORE* anycore, uint32_t startCapacity, uint32_t loopCapacity, uint32_t moduleCapacity) {
    if (!anycore) { return ANYCORE_ERR_INVALID_ANYCORE; }
    ANYCORE_AppManager* am = &anycore->appManager;

    uint32_t prec1 = startCapacity  * sizeof(void*);
    uint32_t prec2 = loopCapacity   * sizeof(void*);
    uint32_t prec3 = moduleCapacity * sizeof(Module);

    void (**starts)(void*) = ANYCORE_mmap(prec1);
    void (**loops)(void*, const double)  = ANYCORE_mmap(prec2);
    Module* modules        = ANYCORE_mmap(prec3);

    if (starts == ANYCORE_MAP_FAILED || loops == ANYCORE_MAP_FAILED || modules == ANYCORE_MAP_FAILED) {
        if (starts  != ANYCORE_MAP_FAILED) { ANYCORE_munmap(starts,  prec1); }
        if (loops   != ANYCORE_MAP_FAILED) { ANYCORE_munmap(loops,   prec2); }
        if (modules != ANYCORE_MAP_FAILED) { ANYCORE_munmap(modules, prec3); }
        return ANYCORE_ERR_ALLOC_FAILED;
    }

    am->Starts         = starts;
    am->Loops          = loops;
    am->modules        = modules;
    am->startCapacity  = startCapacity;
    am->loopCapacity   = loopCapacity;
    am->moduleCapacity = moduleCapacity;
    am->startCount     = 0;
    am->loopCount      = 0;
    am->moduleCount    = 0;
    am->isAppRunning   = false;

    return ANYCORE_SUCCESS;
}

static void ANYCORE_freeAppManager(ANYCORE* anycore) {
    if (!anycore) { return; }
    ANYCORE_AppManager* am = &anycore->appManager;

    for (int32_t i = (int32_t)am->moduleCount - 1; i >= 0; i--) {
        #if ANYCORE_ENABLE_KILL_MODULE
        ANYCORE_killModule(anycore, (uint32_t)i);
        #endif
    }

    if (am->Starts)  ANYCORE_munmap(am->Starts,  am->startCapacity  * sizeof(void*));
    if (am->Loops)   ANYCORE_munmap(am->Loops,   am->loopCapacity   * sizeof(void*));
    if (am->modules) ANYCORE_munmap(am->modules, am->moduleCapacity * sizeof(Module));

    am->Starts = NULL;
    am->Loops = NULL;
    am->modules = NULL;
    am->startCount = 0;
    am->loopCount = 0;
    am->moduleCount = 0;
}

#if ANYCORE_ENABLE_IMPORT_FUNCTION
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_importFunction(void* libHandle, const char* functionName, void** outFunction) {
    if (!libHandle || !functionName || !outFunction) { return ANYCORE_ERR_NULL_POINTER; }

    *outFunction = dlsym(libHandle, functionName);
    if (!*outFunction) { return ANYCORE_ERR_SYMBOL_IMPORT_FAILED; }
    
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_LOAD_MODULE
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_loadModule(ANYCORE* anycore, const char* path, ModuleType type, double interval, void** outLibHandle) {
    if (!anycore) { return ANYCORE_ERR_INVALID_ANYCORE; }
    ANYCORE_AppManager* am = &anycore->appManager;

    if (am->moduleCount >= am->moduleCapacity) { return ANYCORE_ERR_CAPACITY_REACHED; }

    Module* m = &am->modules[am->moduleCount];

    m->libHandle = dlopen(path, RTLD_NOW);
    if (!m->libHandle) { printf("[ANYCORE] module load error : %s\n", dlerror()); return ANYCORE_ERR_MODULE_LOAD_FAILED; }

    m->type        = type;
    m->running     = true;
    m->readyToWork = false;
    m->isDone      = false;
    m->interval    = interval;
    m->lastTick    = 0;

    pthread_mutex_init(&m->runMutex, NULL);
    pthread_cond_init(&m->runCond,   NULL);
    pthread_mutex_init(&m->doneMutex, NULL);
    pthread_cond_init(&m->doneCond,   NULL);

    ANYCORE_importFunction(m->libHandle, "start",        (void**)&m->start);
    ANYCORE_importFunction(m->libHandle, "loop",         (void**)&m->loop);
    ANYCORE_importFunction(m->libHandle, "injectStream", (void**)&m->injectStream);
    ANYCORE_importFunction(m->libHandle, "clean",        (void**)&m->clean);

    pthread_create(&m->thread, NULL, ANYCORE_moduleWorker, m);

    *outLibHandle = m->libHandle;

    am->moduleCount++;
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_LOAD_BEHAVIOUR
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_loadBehaviour(const char* path, Behaviour* outBehaviour) {
    if (!path || !outBehaviour) { return ANYCORE_ERR_NULL_POINTER; }

    void* handle = dlopen(path, RTLD_NOW);
    if (!handle) { 
        char* error = dlerror();
        fprintf(stderr, "[ANYCORE] behaviour load error: %s\n", error ? error : "Unknown error");
        return ANYCORE_ERR_BEHAVIOUR_LOAD_FAILED; }

    *outBehaviour = (Behaviour)handle;
    return ANYCORE_SUCCESS;
}
#endif

#if ANYCORE_ENABLE_UNLOAD_BEHAVIOUR
ANYCORE_EXPORT int ANYCORE_unloadBehaviour(void* libHandle) {
    if (!libHandle) { return 0; }
    if (dlclose(libHandle) != 0) { return -1; }
    return 0;
}
#endif

ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_pushStart(ANYCORE* anycore, void (*startFn)(void*)) {
    if (!anycore) { return ANYCORE_ERR_INVALID_ANYCORE; }
    ANYCORE_AppManager* am = &anycore->appManager;
    if (am->startCount >= am->startCapacity) { return ANYCORE_ERR_CAPACITY_REACHED; }
    am->Starts[am->startCount++] = startFn;
    return ANYCORE_SUCCESS;
}

ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_pushLoop(ANYCORE* anycore, void (*loopFn)(void*, const double)) {
    if (!anycore) { return ANYCORE_ERR_INVALID_ANYCORE; }
    ANYCORE_AppManager* am = &anycore->appManager;
    if (am->loopCount >= am->loopCapacity) { return ANYCORE_ERR_CAPACITY_REACHED; }
    am->Loops[am->loopCount++] = loopFn;
    return ANYCORE_SUCCESS;
}

ANYCORE_EXPORT void ANYCORE_run(ANYCORE* anycore) {
    if (!anycore) { return; }
    ANYCORE_AppManager* am = &anycore->appManager;

    am->isAppRunning = true;

    VertexManager_UpdateExportStream(anycore, &am->stream);
    for (uint32_t i = 0; i < am->moduleCount; i++) {
        Module* m = &am->modules[i];

        if (m->injectStream) { m->injectStream(&am->stream); }
        if (m->start) { m->start(ANYCORE_BUILD_ID); }
    }

    for (uint32_t i = 0; i < am->startCount; i++) {      
        if (am->Starts[i]) { ((void (*)(ANYCORE*))am->Starts[i])(anycore); }
    }
    
    while (am->isAppRunning) {
        double dt = am->stream.deltaTime;
        for (uint32_t i = 0; i < am->loopCount; i++) {
            if (am->Loops[i]) { ((void (*)(ANYCORE*, const double))am->Loops[i])(anycore, dt); }
        }

        VertexManager_UpdateExportStream(anycore, &am->stream);

        for (uint32_t i = 0; i < am->moduleCount; i++) {
            Module* m = &am->modules[i];

            if (m->injectStream) { m->injectStream(&am->stream); }
            if (m->type == MODULE_INDEPENDENT) { continue; }

            bool shouldRun = (m->type == MODULE_MANDATORY);
            if (m->type == MODULE_TIMED) {
                if (am->stream.elapsed - m->lastTick >= m->interval) {
                    shouldRun    = true;
                    m->lastTick  = am->stream.elapsed;
                }
            }

            if (shouldRun) {
                pthread_mutex_lock(&m->runMutex);
                m->readyToWork = true;
                m->isDone      = false;
                pthread_cond_signal(&m->runCond);
                pthread_mutex_unlock(&m->runMutex);
            } else {
                m->isDone = true;
            }
        }

        for (uint32_t i = 0; i < am->moduleCount; i++) {
            Module* m = &am->modules[i];
            if (m->type == MODULE_INDEPENDENT) { continue; }

            pthread_mutex_lock(&m->doneMutex);
            while (!m->isDone) { pthread_cond_wait(&m->doneCond, &m->doneMutex); }
            pthread_mutex_unlock(&m->doneMutex);
        }

        clearDirtyFrame(&anycore->transformManager,   anycore->sceneManager.chunkcount);
        clearModelDirtyFrame(&anycore->vertexManager, anycore->vertexManager.modelCount >> CHUNKSHIFT);

        anycore->timeManager.frame++;
    }

    for (uint32_t i = 0; i < am->moduleCount; i++) {
        Module* m = &am->modules[i];
        if (m->clean) { m->clean(); }
    }

    ANYCORE_freeAppManager(anycore);
    freeANYCORE(anycore);
}

#if ANYCORE_ENABLE_KILL_MODULE
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_killModule(ANYCORE* anycore, uint32_t index) {
    if (!anycore) { return ANYCORE_ERR_INVALID_ANYCORE; }
    ANYCORE_AppManager* am = &anycore->appManager;
    if (index >= am->moduleCount) { return ANYCORE_ERR_INVALID_INDEX; }

    Module* m = &am->modules[index];

    m->running = false;
    pthread_mutex_lock(&m->runMutex);
    pthread_cond_signal(&m->runCond);
    pthread_mutex_unlock(&m->runMutex);

    pthread_join(m->thread, NULL);

    pthread_mutex_destroy(&m->runMutex);
    pthread_cond_destroy(&m->runCond);
    pthread_mutex_destroy(&m->doneMutex);
    pthread_cond_destroy(&m->doneCond);

    if (m->libHandle) {
        dlclose(m->libHandle);
        m->libHandle = NULL;
    }

    return ANYCORE_SUCCESS;
}
#endif

ANYCORE_EXPORT void ANYCORE_shutdown(ANYCORE* anycore) {
    if (!anycore || !anycore->isinitalized) { return; }
    anycore->appManager.isAppRunning = false;
}