#! /usr/bin/env bash

cppcheck src/fifo.c --inline-suppr --addon=scripts/misra.json --template "{file}:{line}: {severity} ({id}): {message}"
