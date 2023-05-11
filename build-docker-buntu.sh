#!/bin/bash 
ISFORCE=''
#ISFORCE='--no-cache  '
docker build $ISFORCE  -f Dockerfile.ubuntu.20.04 .  #-t korni3dist-ubun
