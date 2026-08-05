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

#ifndef ANYCORE_H
#define ANYCORE_H

#include "primitives.h"

#include "anycore_time_manager.h"
#include "anycore_scene_manager.h"
#include "anycore_transform_manager.h"
#include "anycore_vertex_manager.h"
#include "anycore_app_manager.h"

typedef struct {
    bool isinitalized;

    ANYCORE_TransformManager transformManager;
    ANYCORE_SceneManager sceneManager;
    ANYCORE_TimeManager timeManager;
    ANYCORE_VertexManager vertexManager;
    ANYCORE_AppManager appManager;
} ANYCORE;

#endif // ANYCORE_H