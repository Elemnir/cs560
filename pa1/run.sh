#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export PATH="$PATH:$DIR/fakefs"

make

echo "Running server."
./server/server 30000 &

sleep 1

echo "Starting client."
./client/client 30000
