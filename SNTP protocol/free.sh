#!/bin/bash
source info.sh
./sntp.out --ip $ip --port $port --map $map --user $user --pass "$pass" --free
