#!/bin/bash

##multiply clients simultanously

for i in $(seq 1 15); do
    echo "Client $i says hello" | nc -q 0 127.0.0.1 8080 &
done





