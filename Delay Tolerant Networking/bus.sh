#!/bin/bash
source info.sh
if [ "a$1" = "a" ]; then
  node="bus-0"
else
  node="bus-$1"
fi
./bus.out --ip $ip --port $port --map $map --node $node --user $user --pass "$pass" --id $user
