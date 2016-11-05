#! /bin/bash

export SRC_FILENAME="main.cpp"
export BIN_FILENAME="brainloller"

if [ -f "${BIN_FILENAME}" ]; then
    rm -f ${BIN_FILENAME}
fi
   
g++ -std=c++14 -Wall -Wextra -gdwarf-4 \
    ${SRC_FILENAME} \
    `pkg-config --cflags opencv` \
    `pkg-config --libs opencv` \
    -o ${BIN_FILENAME}

if [ -f "${BIN_FILENAME}" ]; then
    echo "OK!"
    ./${BIN_FILENAME} --help
else
    echo "ERROR!"
fi
