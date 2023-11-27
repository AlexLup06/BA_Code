#!/bin/bash

mkdir $root/${1}
opp_scavetool export -f 'module=~Network? AND type=~vector' -o $root/$1/client.json -F JSON $root/${3}/flash_s${3}net?cp${2}rid?.vec
opp_scavetool export -f 'module=~**.surrogateServer[*].app[0] AND type=~vector' -o $root/$1/surrogate$i.json -F JSON $root/${3}/flash_s${3}net?cp${2}rid?.vec
opp_scavetool export -f 'module=~**.originServer.app[0] AND type=~vector' -o $root/${1}/origin.json -F JSON $root/${3}/flash_s${3}net?cp${2}rid?.vec