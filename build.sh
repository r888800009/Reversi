#!/bin/bash

OS_TYPE=$(uname)
if [[ "$OS_TYPE" == "Darwin" ]]; then
    COMPILER="clang++"
elif [[ "$OS_TYPE" == "Linux" ]]; then
    COMPILER="g++"
else
    echo "Unsupported OS: $OS_TYPE"
    exit 1
fi

$COMPILER main.cpp -o reversi
