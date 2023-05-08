#!/bin/bash -x

ISFORCE=''
#ISFORCE='--no-cache  '

docker build $ISFORCE -t korni3dist . 
