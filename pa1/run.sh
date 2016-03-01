#!/bin/bash

PORT="30001"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export PATH="$PATH:$DIR/fakefs"

make

echo "Running server."
./server/server $PORT &

sleep 1

echo "Starting client."
./client/client localhost $PORT
