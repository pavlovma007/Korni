#!/bin/bash -x
	xhost + localhost  2>/dev/null
	xhost +SI:localuser:root  2>/dev/null

	XSOCK=/tmp/.X11-unix
	XAUTH=/tmp/.docker.xauth
	touch $XAUTH
	xauth nlist $DISPLAY | sed -e 's/^..../ffff/' | xauth -f $XAUTH nmerge -


	#XX=" --env=DISPLAY=:0 --env=LANG=ru_RU.utf8 --net host "
	XX="-v $XSOCK:$XSOCK:rw -v $XAUTH:$XAUTH:rw  --device=/dev/dri/card0:/dev/dri/card0  -e DISPLAY=$DISPLAY  -e XAUTHORITY=$XAUTH "
	
	#ALSA from https://joonas.fi/2020/12/audio-in-docker-containers-linux-audio-subsystems-spotifyd/
	# --user $(id -u) 
	ALSA="-v /etc/alsa:/etc/alsa -v /usr/share/alsa:/usr/share/alsa  -v $HOME/.config/pulse:/.config/pulse -v /run/user/$UID/pulse/native:/run/user/$UID/pulse/native --env PULSE_SERVER=unix:/run/user/$UID/pulse/native " 

#	прокид контейнера
	KORNIDATA=" -v $HOME/.config/korni/yt:/root/.config/korni/  -v $HOME/.config/korni/yt:/root/.config/korni/yt"

#export DBUS_SESSION_BUS_ADDRESS=tcp:host=${containerIp},port=6667,family=ipv4
	
	
#if [ -t 1 ] ; then echo terminal; I='-it'; else echo "not a terminal"; I=''; fi

# мы напрямую запустились или из скрипта quick-start ?
if [ $1 != ''] ; then I='-it'; else I='-i'; fi

#  тут я маплю из docker контейнера  контейнер данных tmp в локальную папку tmp
# --gpus all: Expose all GPUs to the container.

docker run $I --rm    $XX -e QT_X11_NO_MITSHM=1  $KORNIDATA $ALSA \
	-v /run/user/1000/bus:/run/user/1000/bus \
	-e DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus \
	korni3dist  

exit 
/mp-qml-project/src/app/run-1.sh


# basical
sudo apt update --fix-missing && apt install -y git gcc g++ make cmake
sudo apt install -y libboost-program-options-dev

# build korni3 
sudo apt install -y sqlite jq coreutils openssl gzip file bash #go-ipfs 
# go + ipfs
#sudo apt install -y golang-go            PROBLEMS WITH IPFS 
1) GET FROM https://go.dev/dl/  GO version 
2) sudo rm -rf /usr/local/go && sudo tar -C /usr/local -xzf go1.20.4.linux-amd64.tar.gz
git clone https://github.com/ipfs/kubo.git && cd kubo && sudo make install && cd .. 
# ssl sqlite3
sudo apt-get install -y libssl-dev libsqlite3-dev
sudo apt install python3-pip && pip install sqlite-utils  
# +? libboost1.74-all-dev
# NEED boost 1.71 ... build it  ?   20.04 has 1.74 ! 
git clone -b main --single-branch https://github.com/pavlovma007/Korni.git && \
	cd Korni/korni3/ && mkdir build  &&   cd build  && \
	cmake  -G "Unix Makefiles" ..   &&   make  &&   \
	cp ./korni3  /usr/local/bin  


# for ubuntu UI yt 
sudo apt install -y qt5-qmake qtbase5-dev       qtmultimedia5-dev qtdeclarative5-dev qtdeclarative5-dev-tools  qtbase5-dev  qt5-qmltooling-plugins qt5-image-formats-plugins qt5-image-formats-plugin-pdf qt5ct qtgstreamer-plugins-qt5 qtkeychain-qt5-dev qtmultimedia5-dev qtpdf5-dev  qtscript5-dev  qttools5-dev-tools qttools5-dev 
sudo apt install qmlscene
git clone -b main --single-branch https://github.com/pavlovma007/mp-qml-project.git  
cd mp-qml-project/  && \
	git submodule update --init --recursive lib/industrial-indicators  && \
	git submodule update --init --recursive lib/industrial-controls && \
	git submodule update --init --recursive lib/qsyncable && \
	mkdir -p build
./scripts/build_qsyncable.sh && \
	./scripts/build_industrila-controls.sh  && \
	./scripts/build_Korni3Plugin.sh	
sudo apt install python3-pip && pip install orjson ujson
sudo cp src/app/yt-backuper/yt-dlp /usr/local/bin/yt-dlp
# run 
cd src/app && ./run-1.sh
