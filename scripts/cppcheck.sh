#! /usr/bin/env bash

cppcheck src/fifo.c --addon=scripts/misra.json
