#!/bin/bash 

#Библиотеки нормально устанавливаются в /usr/lib (e.g. на моей системе, /usr/lib/x86_64-linux-gnu/libboost_program_options.so.1.58.0).
#Для того, чтобы скомпилировать код, который собирается линковкой с теми библиотеками, вы нормально используете заголовочные файлы, предоставленные в /usr/include.
#В отличие от многих библиотек, Boost не поставляется с файлами pkg-config, поэтому вам нужно самому добавить флаги линковщика. С обычными правилами Makefile вам понадобится что-то вроде LDLIBS += -lboost_program_options.
#Учтите также, что, хотя пакет libboost-dev предоставляет заголовки для program_options, вам нужно также установить libboost-program-options-dev чтобы получить соответствующую библиотеку.

sudo apt install openssl 
sudo apt install libboost-program-options-dev libboost-dev
sudo apt install libssl-dev

sudo apt install jq coreutils
