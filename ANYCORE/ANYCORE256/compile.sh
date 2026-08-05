#!/bin/bash
set -e

CORE_INCLUDE="ANYCORE256/include"
CORE_SRC="ANYCORE256/src"

EXTRA_INCLUDE="ANYCORE256/"

CORE_OUT="./build/anycore256.so"

CC=gcc

mkdir build;

$CC -shared -fPIC \
    $CORE_SRC/anycore.c \
    $CORE_SRC/anycore_scene_manager.c \
    $CORE_SRC/anycore_transform_manager.c \
    $CORE_SRC/anycore_vertex_manager.c \
    $CORE_SRC/anycore_app_manager.c \
    -I$CORE_INCLUDE \
    -I$EXTRA_INCLUDE \
    -Wall -O3 \
    -o $CORE_OUT

echo "done"