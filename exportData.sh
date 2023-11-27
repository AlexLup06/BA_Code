#!/bin/bash

mkdir $root/${1}
opp_scavetool export -f 'module=~Network? AND type=~vector' -o $root/$1/client.json -F JSON $root/CDNsimulator/simulations/results/flash_s${2}net?cp${3}rid?.vec
opp_scavetool export -f 'module=~**.surrogateServer[*].app[0] AND type=~vector' -o $root/$1/surrogate$i.json -F JSON $root/CDNsimulator/simulations/results/flash_s${2}net?cp${3}rid?.vec
opp_scavetool export -f 'module=~**.originServer.app[0] AND type=~vector' -o $root/${1}/origin.json -F JSON $root/CDNsimulator/simulations/results/flash_s${2}net?cp${3}rid?.vec