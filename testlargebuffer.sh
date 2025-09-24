#!/bin/bash

SERVER=127.0.0.1
PORT=8080
CLIENTS=10
MSG_SIZE=8192  # bigger than your buffer

for i in $(seq 1 $CLIENTS); do
(
    # generate big message of 'A's, leave 1 byte for newline
    head -c $((MSG_SIZE-1)) < /dev/zero | tr '\0' 'A' > msg_$i.txt

    # append newline and client identifier
    printf "\nClient %d says hello\n" "$i" >> msg_$i.txt

    # send to server
    nc -q0 $SERVER $PORT < msg_$i.txt

    # clean up
    rm msg_$i.txt
) &
done

wait
echo "All $CLIENTS clients done."