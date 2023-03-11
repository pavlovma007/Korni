#!/bin/bash 

mkdir -p tmp
(cat  <<END  

	mkdir -p ~/.config/korni/tmp/
	#/root/.config/korni/geoplace
	korni3 setgeo 10 10
	cat ~/.config/korni/geoplace
	
	korni3 genkey demo
	korni3 keys
	korni3 login demo
	korni3 create  tmp testTable '{"id":123}'

	# now it no need 
	#korni3 scan tmp

	korni3 db tmp # it out to std:
	#/root/.config/korni/tmp/database.db

	sqlite3 `korni3 db tmp` <<EEE
		select * from testTable; 
EEE
	# смотрим файлы. 
	find ~/.config/korni/tmp/

END
)   | ./run.sh fromIn
