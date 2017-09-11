source info.sh
if [ "a$1" = "a" ]; then
  node="city-0"
else
  node="city-$1"
fi
./city.out --ip $ip --port $port --map $map --node $node --user $user --pass "$pass" --id $user
