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

#ifndef ANYCORE_APP_MANAGER_UTILS_H
#define ANYCORE_APP_MANAGER_UTILS_H

#include "anycore.h"
#include "anycore_app_manager.h"

ANYCORE_RESULT initAppManager(ANYCORE* anycore, uint32_t startCapacity, uint32_t loopCapacity, uint32_t moduleCapacity);

#if ANYCORE_ENABLE_IMPORT_FUNCTION
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_importFunction(void* libHandle, const char* functionName, void** outFunction);
#endif

#if ANYCORE_ENABLE_LOAD_MODULE
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_loadModule(ANYCORE* anycore, const char* path, ModuleType type, double interval, void** outLibHandle);
#endif

#if ANYCORE_ENABLE_LOAD_BEHAVIOUR
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_loadBehaviour(const char* behaviourLibPath, Behaviour* outBehaviour);
#endif

#if ANYCORE_ENABLE_UNLOAD_BEHAVIOUR
ANYCORE_EXPORT int ANYCORE_unloadBehaviour(void* libHandle);
#endif

ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_pushStart(ANYCORE* anycore, void (*startFn)(void*));
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_pushLoop(ANYCORE* anycore, void (*loopFn)(void*, double));

#if ANYCORE_ENABLE_KILL_MODULE
ANYCORE_EXPORT ANYCORE_RESULT ANYCORE_killModule(ANYCORE* anycore, uint32_t index);
#endif

ANYCORE_EXPORT void ANYCORE_run(ANYCORE* anycore);
ANYCORE_EXPORT void ANYCORE_shutdown(ANYCORE* anycore);

#endif // ANYCORE_APP_MANAGER_UTILS_H
