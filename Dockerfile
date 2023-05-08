FROM alpine:3.15
# dev libs 
RUN apk update && apk add boost1.77-program_options  boost-dev  openssl-dev sqlite-dev

# Install python/pip
ENV PYTHONUNBUFFERED=1
RUN apk add --update --no-cache python3 && ln -sf python3 /usr/bin/python
RUN python3 -m ensurepip
RUN pip3 install --no-cache --upgrade pip setuptools  

# compile tools 
RUN apk add git openssh-client cmake gcc g++ make  python3-dev && \
	# pip depends for paxos nodes \
	pip3 install pylint flake8 pysqlite3 asyncio pause 

# usable with korni3 for comfort
RUN apk add sqlite jq 

# korni3 run this tools 
RUN apk add coreutils openssl gzip go-ipfs file bash  &&  \
	pip install sqlite-utils  
	#&& \
	# чистим место \
	#pip uninstall -y pip
	

	
# build from source  
RUN git clone -b main --single-branch https://github.com/pavlovma007/Korni.git && \
	cd Korni/korni3/ && mkdir build  &&   cd build  && \
	cmake  -G "Unix Makefiles" ..   &&   make  &&   \
	cp ./korni3  /usr/local/bin  # && 

# create .config structure 
RUN mkdir -p  ~/.config/korni/MY-KEYS/ && chmod -R  go-x-w-r  ~/.config/korni/MY-KEYS/


# UI 
# qmlscene for korni UI 
RUN apk add qt5-qtdeclarative qtchooser qt5-qttools-dev qt5-qtbase-dev qt5-qtdeclarative-dev qt5-qtquickcontrols2-dev qt5-qtsvg-dev  qt5-qtwebview-dev qt5-qtmultimedia-dev
#RUN apk add  # qt5-qtwebengine-dev qt5-qtwebglplugin-dev qt5-qtquick3d-dev  qt5-qtscript-dev qt5-qt3d-dev 

RUN cd / && git clone -b main --single-branch https://github.com/pavlovma007/mp-qml-project.git  && \
	cd mp-qml-project/  && \
	git submodule update --init --recursive lib/industrial-indicators  && \
	git submodule update --init --recursive lib/industrial-controls && \
	git submodule update --init --recursive lib/qsyncable && \
	mkdir -p build && cd scripts && \
	./scripts/build_qsyncable.sh  && \
	./scripts/build_industrila-controls.sh && \
	./scripts/build_Korni3Plugin.sh
	
	
	
	

# remove build tools 
#RUN apk del git cmake gcc g++ make  &&  \
#	# remove dev libs 	\
#	apk del boost1.77-program_options  boost  boost-dev  openssl-dev sqlite-dev  && \
#	rm -rf /usr/include



#ENTRYPOINT ["/bin/bash"]
