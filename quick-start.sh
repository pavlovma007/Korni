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
	

	/usr/bin/sqlite3 /root/.config/korni/tmp/database.db <<EEE
.header on
.mode line
select * from testTable; 
EEE
	# смотрим файлы. 
	find ~/.config/korni/tmp/  -type f

END
)   | ./run.sh fromIn
