#!/bin/bash

TIME='date +%s'
THISPROPG=$0

export KEYPATH=~/.config/korni/me.key
export PUBKEY=~/.config/korni/me.pub
export CERTPATH=~/.config/korni/me

export MAGICSTART=MP
export MAGICEND=NS

printUsage(){
	if [ $# -eq 0 ] 
	then
		cat <<USAGEEND
usage: $ ${THISPROPG} <options> command [commandParams ...]
retcode 0 - ok, retcode 1 - error (access etc)

LICENSE: 
   THIS IS a @pavlovma007 utility from korni3 system, all right reserved

options:
---------

commands: 
---------
========================== DATA MODIFICATION ===========================

create <db name> <table name> <json record string>  - write new record,
                                                      return object uuid
set    <db name> <object uuid> <json record string> - patch object props 
                                                      from json dict
delete <db name> <object uuid>                      - remove object

// you can try change any data but "read susbsystem" do not dysplay your 
// modifications if you can't

=========================== DATA SELECTION =============================

select <db name> <sql>          - select data from db

=========================== ACCESS CONTROL =============================

grant <db name> <object uuid> <userid> [S|R|D|A]   - give right to user
                                        S-set|R-Replace|D-delete|A-admin
// by default, only your have acces to your data.
// if your not give it to other - other's data changes can be displayed
// if user is Admin - he can give acces to others.

============================== LOGIN etc ===============================

login <user id>                - use identifier as login
                                 it must the be public key, BitMessageId
                                 
key add mycert.pem mykey.pem	- add new THIS user key / !tip
USAGEEND
		exit 1
	fi
}
#printUsage   ; exit 0

#OPTS 
#parseOpts() {
	#while getopts f:s: flag
	#do
		#case "${flag}" in
			#f) first=${OPTARG};;
			#s) second=${OPTARG};;

		#esac
	#done
	#echo "First: $first";
	#echo "Second: $second";
#}
#parseOpts



# "стать онлайн" - обновить в контейнере файл со своими данными
# online/AAAAB3Nz/aC1yc2EA/AAADAQAB/AAA.....nGTUWON/online.info   /  online.scret
#прописать свои `p2p контакты` это адреса для открытых сервисов p2plib , внешние адреса от stun, прочие адреса ipv6 итид. 
# чтобы получать прямые соединения от других участников
# контент следует подписать 
# то что файл обновлен внутри еще НЕ значит, что данные из него будут сообщены другим - это делает отдельная стевая система. 


# новый контейнер.    генерируем ему навсегда uuid
# 


# таблицы в контейнере. 
# Файлы юзеров могут ??? выглядеть так  для контейнера с УУИД c1e9b221-2292-4963-ad68-fe2ef07d7b78 у юзера  45c8800d889fb48dff5 (каждый ид бьём на 3 части)
# cnt/c1e9b221/2292/4963/ad68/fe2ef07d7b78/user/45c8800d/889fb4/8dff5/c1e9b221-2292-4963-ad68-fe2ef07d7b78--45c8800d889fb48dff5.torrent
# cnt/c1e9b221/2292/4963/ad68/fe2ef07d7b78/user/45c8800d/889fb4/8dff5/c1e9b221-2292-4963-ad68-fe2ef07d7b78--45c8800d889fb48dff5.torrent.sign
# > это торренты всего контейнера от каждого юзера... **это транспорт**!!!
# 
# ./tables/`nodes-2022-05-31.csv`






# путь в контейнере для файла
# ./files/2022/2022-05-22/QmVKD18Y/qaAftH9r/b99nMLKA7rGBWqM8rmD9Q4a6pfjhaL/T-node-GH-uf7d5zbvz7q.csv.gz
# GH-uf7d5zbvz7q  - GeoHash   (uf7d5zbvz7q)
# QmVKD18Y/qaAftH9r/b99nMLKA7rGBWqM8rmD9Q4a6pfjhaL  - испорченный ipfs хеш 
#
# ид объекта
# GFS-QmVKD18YqaAftH9rb99nMLKA7rGBWqM8rmD9Q4a6pfjhaL-T-7bac1d7ca579214232065d8e5fb0c09ae9896282-M-6a7da134e822da8a5e8fd0e70601d5c0-S256-76ac4be60275b9ae272b33bb1f6d896bb82c0b8de3312dfc7d1c0238f815f2ad
#
# таблица 
# * path
# * gfs-hash
# * torrent-hash 
# * sha256 
# * md5 
# * isFile
# * objectType str OR typeuuid 
# 
# files.csv таблица каждая своя у каждого юзера 
filePath(){
	hash = $1;
	
}



keyAdd() {
	# openssl cert
	certPath=$1
	keyPath=$2
	#echo 'pubKeyPath' $pubKeyPath
	if [ -n $certPath ] && [ -n keyPath ]; then 
		# is a self signed cert ? 
		openssl verify -CAfile  $certPath $certPath
		if [ $? = 0 ]; then 
			cp -f $certPath $CERTPATH 
			echo "OK: rewrite pubkey to $CERTPATH"
		else 
			echo "ERROR: cert are bad"
			exit 1
		fi
	else
		echo 'cert file not found'
	fi
	
	#Verifying that a Private Key Matches a Certificate
	(openssl x509 -noout -modulus -in $certPath 2>/dev/null | openssl md5 ;\
	openssl rsa -noout -modulus -in $keyPath 2>/dev/null | openssl md5) | uniq
	if [ $? = 0 ]; then 
		cp -f $keyPath $KEYPATH
		echo "OK: priv key save to $KEYPATH"
		# save pub key 
		openssl rsa -in $KEYPATH -pubout > $PUBKEY 2>/dev/null 
		echo "OK: pub key save to $PUBKEY"
	else
		echo "ERROR: key not match cert"
		exit 1
	fi
}
#keyAdd mycert.pem mykey.pem #$*
#exit 
# openssl req \
#    -x509 -nodes  \
#    -newkey rsa:1024 -keyout mykey.pem -out mycert.pem

test(){
	cat $PUBKEY | 
#	awk '{ if(i++ == 3) {c=$0; ;}   if(i++ == 2) {b=$0; ;}   if(i++ == 1) {a=$0; ;} }; END{ print b;}'
}
test ; exit; 

###################################
# parse command params   https://fedingo.com/how-to-parse-command-line-arguments-in-bash/
i=1;
j=$#;
while [ $i -le $j ] 
do
    # echo "$i: $1";
	case $1 in
	  help | h)
		printUsage; shift 1
		;;
	  tool1 ) 
		# secret internal command 
		tool1 $* ; exit 
		;;
	  debug ) 
		#  TODO 
		debug $* ; exit 
		;;
	  installDepends )
		installDepends; shift 1
		;;

	  key )
		case $2 in 
			add )
				keyAdd $* ; shift 4
				;;
			* ) 
				echo "ERROR when parse params key"
				printUsage; exit 1;
		esac
		;;

	  secret )
		case $2 in 
			add ) 
				secretAdd $3; shift 3
				exit 0
				;;
			* ) 
				echo "ERROR when parse params secret"
				printUsage ; exit 1;
				;;
		esac
		;;

	  select | Select )
		case $2 in 
			"topic" ) 
				selectTopic $3 $4 ; shift 4
				exit 0
				;;
			* ) 
				echo "ERROR when parse params select"
				printUsage ; exit 1;
				;;
		esac
		;;

	  j | J)
		case $2 in 
			new ) 
				result=$(jNew $3 ); 
				f=$(echo $result | awk '{print $1}')
				jid1=$(echo $result | awk '{print $2}')
				parseFile $f
				echo "jid = $jid1"
				exit 0
				;;
			set ) 
				file=$(jSet $3 $4 $5) 
				parseFile $file
				shift 4; 
				exit # ?
				;;
			show ) 
				case $3 in 
					asmap | asMap )
						asMap $4 # jid
						exit 0
						;;
					* )
						;;
				esac
				
				;;
			insert ) 
				jid=$3
				value=$4
				#echo $jid $value
				file=$( jInsert "$jid" "$value" | head -n 1 -q - )
				#echo $file
				parseFile $file
				shift 4;
				exit 0
				;;
				
			* ) 
				echo "ERROR when parse params j|J"
				printUsage ; exit 1
				;;
		esac
		;;
	  file | File | FILE )
		parseFile $2 ; exit 
		;;
	  dbconsole | dbConsole | db_console | db ) 
		dbConsole 
		exit $?
		;; 
	  *)
		echo "ERROR when parse params"
		printUsage ; exit 1;
		;;
	esac
    
    i=$((i + 1));
    #shift 1;
done



end() {
	exit 
}

exit 



################## сниппеты для с++ программы ##################


# SIGN 
echo 'misha pavlov' | openssl dgst --sha256 --sign ~/.config/korni/me.key  | (base64 -w 0 ; echo ;)
akhz/m290is+F4SluUUs4sIz2M+e4OuKDNzSGz+FNr8Q8cajrLj6IZPNVw5CVRImgrWOFBgSg/A8aIRcNd4BvCRAH+xL+0uDPCfVjR5Wkm+Lc/m8ICQtcr1hwJBTab8MjoGc2MXanD6Ddl7L35xOeSr5DwV5Plw1woBUJ9l2Mt0=


# инфо: посчитать хеши  сертификатов для проверки подписей
for file in *; do ln -s "$file" "$(openssl x509 -hash -noout -in "$file")".0; done


# VERIFY 
echo 'misha pavlov' | openssl dgst -sha1 -verify '/home/mp/.config/korni/me.pub' -signature '/tmp/mytemp.odWaMY' --

# JQ jsonNL ... `-c` key is what i need


# сложить json через конвеййер 
echo '{"a":1, "b":"c"}' | jq -c '.d=2|.e=3'
# или сложить с новым объектом 
echo '{"a":1, "b":"c"}' | jq -c '. + {d:2,e:3}'

# удалить из json поле = оставить только другое
echo '{"b":"c", "a":1}' | jq -c '. |  { a }  '
# удалить можно и так `del( .field2 )`
jq -c --arg val foo '.key5[] |= ( del( .field2 ) | .field3 = $val )'

# сортировка полей в json (для подписи и проверки подписи)
https://stackoverflow.com/questions/30331504/how-to-sort-a-json-file-by-keys-and-values-of-those-keys-in-jq
sort by key 
	jq -c --sort-keys
	echo '{"b":"c", "a":1, "d":2}' | jq -c --sort-keys '. |  { b, a }  '
	или просто указывать поля в нужном порядке
	echo '{"b":"c", "a":1, "d":2}' | jq -c '. |  { a, b }  '
sort_by
	jq -c '.components.rows|=sort_by(.id)|.components.rows[].properties|=sort_by(.name)'   file.json > out.json
	





params:
	create test node '{"fam":"pavlov","id":"wcdsa", "name":"misha"}'	
in json: 
	{"fam":"pavlov","id":"wcdsa","name":"misha"}
for sign : 
	"{"fam":"pavlov","id":"wcdsa","name":"misha","zT":"1663347867","zU":"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDOrZEaOJqsWOPryE1H3dJhqaAG6eeo+6fGfl9FiEMjDPfWiaMypGfFF4uKMMKQrU1G94BIDP8nJ6bJgmsTWoTeIOrx5p9eixKVQwTmfkps3EiUv\/iEVASIJaGIuSutmNJ69uNIaU2IjLdObcmuOIs20UdLvkVTicA+f+yzWvqgAQIDAQAB"}
result : 
	$ zcat ~/.config/korni/test/tables/2022/2022-9-16/U-e1b/b3e/e611c573511be2fb3891d023527d6078e4bfbfad3e126be5f5c9e916fb77/T--node--D-2022-9-16-GH-uf7d5zbvz7qt.csv.gz 
	{"fam":"pavlov","id":"wcdsa","name":"misha","zT":"1663348218","zU":"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDOrZEaOJqsWOPryE1H3dJhqaAG6eeo+6fGfl9FiEMjDPfWiaMypGfFF4uKMMKQrU1G94BIDP8nJ6bJgmsTWoTeIOrx5p9eixKVQwTmfkps3EiUv\/iEVASIJaGIuSutmNJ69uNIaU2IjLdObcmuOIs20UdLvkVTicA+f+yzWvqgAQIDAQAB","zS":"U1D9AveQng2ctxoYm0ZTdyypELf\/qrzbtDvjrMkw6iQIr50oYdbpVpbX5IJf\/v5e\ni7\/DyN6tu+Q7indVbWPtXDHae0FpSCpiT6G8ZhpCQCU1AMqUxkfH52P+Ro624WR2\nf6XwbsLgKsC08Bx0TAJ7EipVGWkyZuaLtKRIc4asamM="}"
	

file path in container example 
	/home/mp/.config/korni/test/files/Qme6DcxwC2SABnt/tae5ndAPzNyk4G2/KHmxFas7NqFHPFhy/GFS-Qme6DcxwC2SABnttae5ndAPzNyk4G2KHmxFas7NqFHPFhy-T-bd3541672cd9a9c83d7ac8c6f74a7f6fe272f734-M-8a5ff4aeb511b1cfae11787da6e28a99-S256-7b3a3f93a63341651d7ef129ddca235f33cf061e434cd866e3d1695cb4cee402


$ zcat  /home/mp/.config/korni/test/tables/2022/2022-9-15/U-e1b/b3e/e611c573511be2fb3891d023527d6078e4bfbfad3e126be5f5c9e916fb77/T--node--D-2022-9-15-GH-uf7d5zbvz7qt.csv.gz | cat -n
     1	{"fam":"pavlov","id":"uuid-text","name":"misha","zS":"SlrUcXRNI+LMBeW3JuWENGllK31Fj6CDheL3I0osN5wENREfE95f/7rJNG66FlCqWs2WQdL0W9w1NtjJKU1IPzdWmTdB5of234SleK3qrAyJmh0dxDWC/rztyxr4cQpfotR6nBeq8pn3Q6AXkmeRasG/K6ZXzClkeAmCfF62iek=","zT":1663235861,"zU":"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDOrZEaOJqsWOPryE1H3dJhqaAG6eeo+6fGfl9FiEMjDPfWiaMypGfFF4uKMMKQrU1G94BIDP8nJ6bJgmsTWoTeIOrx5p9eixKVQwTmfkps3EiUv/iEVASIJaGIuSutmNJ69uNIaU2IjLdObcmuOIs20UdLvkVTicA+f+yzWvqgAQIDAQAB"}
     
тут размер json 468 символов     
     
размер сжатый  407 байт, если json объект будет большой - сжаьте должно луше себя проявить

$ ll   /home/mp/.config/korni/test/tables/2022/2022-9-15/U-e1b/b3e/e611c573511be2fb3891d023527d6078e4bfbfad3e126be5f5c9e916fb77/T--node--D-2022-9-15-GH-uf7d5zbvz7qt.csv.gz 
-rw-rw-r-- 1 mp mp 407 сен 15 12:58 /home/mp/.config/korni/test/tables/2022/2022-9-15/U-e1b/b3e/e611c573511be2fb3891d023527d6078e4bfbfad3e126be5f5c9e916fb77/T--node--D-2022-9-15-GH-uf7d5zbvz7qt.csv.gz


принято решение за userid брать сам "публичный ключ" например в base64. чтобы сразу же им и подпись проверять.  не хеш и не имя а сам публичный ключ, но чем он длиннее тем длиннее подпись... 
много бит - надежнее, мало бит - короче записи в файлах - меньше мета занимают. 




{"fam":"pavlov","id":"uuid-text","name":"misha","zS":"SlrUcXRNI+","zT":1663235861,"zU":"MIG"}


"{
    "fam": "pavlov",
    "id": "uuid-text",
    "name": "misha",
    "zT": "1663235861",
    "zU": "MIG"
}
"
{"fam":"pavlov","id":"uuid-text","name":"misha","zT":"1663235861","zU":"MIG"}


openssl dgst -sha256 -verify '/tmp/kornitemp.bp9oVH' -signature '/tmp/kornitemp.pcxXJH' --


retcode 0 - is bad 

sign2
	text "2022.09.16"
	digest "b37cb7666754f51880db2d9a821de1fbe7f29e38e69cd7342ea4c87617a4e052"
	signature size  128
	signature base64 "xOgOK07JlwK3WeA4zDpND99KY0aXX2SzNU9loGzfumtDKqB4i+yU3hRDg1s99NnzUjNCS1W88thqJ2iTYdJ+X7kBE/+3czwdE5P7uEXHDB1gn1S1Iq97xaRDhamjL8x8WiAKxaxIYPH6/tCi/x3GNaG3+wgeETEPx4ZaOd3Muis="
	sign base64 size 172
verify 




выбрать последнюю версию по времени

	SELECT a.id, a.zT, a.*
	FROM testTable a
	INNER JOIN (
		SELECT id, MAX(zT) zT
		FROM testTable
		GROUP BY id
	) b ON a.id = b.id AND a.zT = b.zT ;
	
	
то же но он ЮЗЕРА - ХОЗЯИНА

	SELECT a.id, a.zT, a.*
	FROM testTable a
	INNER JOIN (
		SELECT id, MAX(zT) zT
		FROM testTable
		WHERE zU="MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAvCEIchAnnO5hIdiuFt/+op8uBbRdh67lPjkLU6Mj00Q+DoFuOWBb3pRfLXdd6xEg+VUa7liBunrm99a5og/DYp98p6+Lh68yCSzLzDm9bPkvVuRsjVjm4Fngc6/sKK+dC04KtnDDoQ5J+rIAHaX2WrgyTwu1Hc+M0ENeAazSXwCNJMc+Fgutj0ofaobRlelvx/FndZKQUFWkbVBf6LY08kSut7sPadBrKMbf1Oyg1k5FUTSbgOBvUmyIx+qOfINCv/TsJxLCSzyCZXRJFJ40EGz1bO2XbJRf8EIWwNYk6VnKZ70dJqClBBAWxVM1w7k5t8aDyBTsEic4uLZLyIN5t9ou9klWW467pTUwhkHJbD2xG9bB/YFLOQY9bQxOx6CRbw4ZegLhk+M3AjG3P20/ouv7psYz9o08pfgpvlkaICGklvWnCBAB4AC1FI/e83j6JVqrFTn+ZJCVxED5OaiKqioaK2bWu7QHgo/7UgQZpKwB7qRadzg1mQ7k1Vd0wy8iBnQajc1akH4RiK/IwMqsI+Z9JlP4vWLHWPdUQzPzghKsTj2HbBBzBhvrrMV5szYuxQj/vVYtEcNPapigWinhwdoZCFNGfXHDeKt/iph1GYqOD0krFz96K2glphFfa74CkaOjTXUnn2f4lI/Pz82GwnqFG1/8W0LXKO4RcgxZYvUCAwEAAQ=="
		GROUP BY id
	) b ON a.id = b.id AND a.zT = b.zT ;





FLUENT ffmped pip 
https://stackoverflow.com/questions/64723759/ffmpeg-command-line-code-translate-to-fluent-ffmpeg

	var args = [
		'-ss', '00:00:3',
		'-thread_queue_size', '4096' ,'-f' , 'avfoundation', '-framerate', '25' ,'-i' ,'1', 
		'-thread_queue_size', '4096' ,'-f' , 'avfoundation', '-framerate', '25' ,'-video_size', '320x240' ,'-i' ,'0:0' ,
		'-c:v', 'libx264', '-crf', '2', '-preset', 'ultrafast',
		'-filter_complex', 'overlay=main_w-overlay_w-10:main_h-overlay_h-10' ,
		'-r', '25', 'Dektop record'+Date().toString()+'.mp4'
	];
	command = spawn(ffmpegPath, args);
	





# bash regexp
mystring1='<link rel="self" href="/api/clouds/1/instances/1BBBBBB"/> dsf <link rel="self" href="/api/clouds/1/instances/2AAAAAAA"/>'
regex='/instances/([A-Z0-9]+).*/instances/([A-Z0-9]+)'
[[ $mystring1 =~ $regex ]]
$ echo ${BASH_REMATCH[1]}
1BBBBBB
$ echo ${BASH_REMATCH[2]}
2AAAAAAA
