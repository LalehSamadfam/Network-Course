#!/bin/bash
source info.sh
./sntp.out --ip $ip --port $port --map $map --node $node1 --user $user --pass "$pass" --id $user
