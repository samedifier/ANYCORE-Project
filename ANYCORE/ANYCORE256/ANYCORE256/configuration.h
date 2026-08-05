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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "primitives.h"

#define OFF 0
#define ON  1

#define SPACE_2D         2
#define SPACE_3D         3
#define PRESICION_FLOAT  4
#define PRESICION_DOUBLE 8

///

#define ANYCORE_ENABLE_GET_BUILD_ID                      ON

#define ANYCORE_ENABLE_LOAD_MODULE                       ON
#define ANYCORE_ENABLE_LOAD_BEHAVIOUR                    ON
#define ANYCORE_ENABLE_UNLOAD_BEHAVIOUR                  ON
#define ANYCORE_ENABLE_IMPORT_FUNCTION                   ON
#define ANYCORE_ENABLE_KILL_MODULE                       ON

#define ANYCORE_ENABLE_IS_ENTITY_ALIVE                   ON
#define ANYCORE_ENABLE_IS_ENTITY_LOCKED                  ON

#define ANYCORE_ENABLE_CREATE_ENTITY                     ON
#define ANYCORE_ENABLE_RESTORE_ENTITY                    ON
#define ANYCORE_ENABLE_DESTROY_ENTITY                    ON

#define ANYCORE_ENABLE_LOCK_ENTITY                       ON
#define ANYCORE_ENABLE_UNLOCK_ENTITY                     ON

#define ANYCORE_ENABLE_GET_ACTIVE_CHUNK_COUNT            ON
#define ANYCORE_ENABLE_GET_LAST_CHUNK_VALID_ENTITY_COUNT ON
#define ANYCORE_ENABLE_FREE_LAST_CHUNK                   ON
#define ANYCORE_ENABLE_ALLOC_NEW_CHUNK                   ON

#define ANYCORE_ENABLE_CREATE_ENTITY_BULK                ON
#define ANYCORE_ENABLE_RESTORE_ENTITY_BULK               ON
#define ANYCORE_ENABLE_DESTROY_ENTITY_BULK               ON
#define ANYCORE_ENABLE_LOCK_ENTITY_BULK                  ON
#define ANYCORE_ENABLE_UNLOCK_ENTITY_BULK                ON

#define ANYCORE_ENABLE_DESTROY_ALL_ENTITIES              ON
#define ANYCORE_ENABLE_LOCK_ALL_ENTITIES                 ON
#define ANYCORE_ENABLE_UNLOCK_ALL_ENTITIES               ON

#define ANYCORE_ENABLE_ADD_MODEL                         ON
#define ANYCORE_ENABLE_SET_MODEL                         ON
#define ANYCORE_ENABLE_UPDATE_MODEL                      ON
#define ANYCORE_ENABLE_REMOVE_MODEL                      ON

#define ANYCORE_ENABLE_SET_POSITION                      ON
#define ANYCORE_ENABLE_SET_ROTATION                      ON
#define ANYCORE_ENABLE_SET_SCALE                         ON
#define ANYCORE_ENABLE_GET_POSITION                      ON
#define ANYCORE_ENABLE_GET_ROTATION                      ON
#define ANYCORE_ENABLE_GET_SCALE                         ON
#define ANYCORE_ENABLE_ADD_POSITION                      ON
#define ANYCORE_ENABLE_ADD_ROTATION                      ON
#define ANYCORE_ENABLE_ADD_SCALE                         ON

#define ANYCORE_ENABLE_SET_POSITION_BULK                 ON
#define ANYCORE_ENABLE_SET_ROTATION_BULK                 ON
#define ANYCORE_ENABLE_SET_SCALE_BULK                    ON
#define ANYCORE_ENABLE_GET_POSITION_BULK                 ON
#define ANYCORE_ENABLE_GET_ROTATION_BULK                 ON
#define ANYCORE_ENABLE_GET_SCALE_BULK                    ON
#define ANYCORE_ENABLE_ADD_POSITION_BULK                 ON
#define ANYCORE_ENABLE_ADD_ROTATION_BULK                 ON
#define ANYCORE_ENABLE_ADD_SCALE_BULK                    ON

///

#define PRESICION_ PRESICION_FLOAT
#define SPACE      SPACE_3D

#if PRESICION_ == PRESICION_FLOAT
    #define PRESICION float
#elif PRESICION_ == PRESICION_DOUBLE
    #define PRESICION double
#endif

#define ANYCORE_EXPORT __attribute__((visibility("default")))

static inline void ANYCORE_memset32(uint32_t* dst, size_t count) {
    while (count--) { *dst++ = 0; }
}

static inline void ANYCORE_memset8(uint8_t* dst, size_t count) {
    while (count--) { *dst++ = 0; }
}

#include <sys/mman.h>
#define ANYCORE_mmap(size) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)
#define ANYCORE_munmap(ptr, size) munmap(ptr, size)
#define ANYCORE_MAP_FAILED MAP_FAILED

#define ANYCORE_VERSION 0x01

#if SPACE == SPACE_3D
    #define AC_DIM_VAL 0x03
#elif SPACE == SPACE_2D
    #define AC_DIM_VAL 0x02
#endif

#if PRESICION_ == PRESICION_FLOAT
    #define AC_PREC_VAL 0x04
#elif PRESICION_ == PRESICION_DOUBLE
    #define AC_PREC_VAL 0x08
#endif

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define AC_ENDIAN_VAL 0x01
#else
    #define AC_ENDIAN_VAL 0x00
#endif

#define ANYCORE_BUILD_ID ((uint32_t)((AC_ENDIAN_VAL << 24) | (AC_PREC_VAL << 16) | (AC_DIM_VAL << 8) | ANYCORE_VERSION))

#endif // CONFIGURATION_H
