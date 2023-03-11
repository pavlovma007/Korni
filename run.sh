#!/bin/bash -x

#if [ -t 1 ] ; then echo terminal; I='-it'; else echo "not a terminal"; I=''; fi

# мы напрямую запустились или из скрипта quick-start ?
if [ $1 != ''] ; then I='-it'; else I='-i'; fi

#  тут я маплю из docker контейнера  контейнер данных tmp в локальную папку tmp
docker build -t korni3dist . && \
	docker run $I --rm -v `pwd`/tmp:/root/.config/korni/tmp/    korni3dist   bash -x
