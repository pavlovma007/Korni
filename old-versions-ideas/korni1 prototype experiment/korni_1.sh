#!/bin/bash

printUsage(){
	if [ $# -eq 0 ] 
	then
		cat <<USAGEEND
usage: korni <options> command [commandParams ...]

options:
---------

commands: 
---------

key add mycert.pem mykey.pem	- add new THIS user key 
secret add <secret-password> 	- add secret to known
select topic <topic id>		- select known topic for data
				work info saved in ./.korniTopic
J new 				- create new journal object . 
				return jid of new created object
J show asList <jid> 		- show object as list
J show asMap [<jid>]		- show object as map
dbconsole|dbConsole|db_console|db - interactive for db stdin
J set <jid> <key> <bin value> - write value to journal
J set <jid> <key> 		- write value to journal 
				but read std input bin	
J permGet <jid>			- read permition of object. 
J permAdd <jid> <to> [S|R|D|A]	- grant permition to other user <to> 
				S-set|R-Replace|D-delete|A - admin 	
J permRemove <jid> [G|S|D|A]	- remove permition for SELFT for jid
file|File|FILE <PATH>		- scan file for operation blocks 	

installDepends
	prepare system for working . install some soft from repos 
	create .config folder and structure

USAGEEND
		exit 1
	fi
}
#printUsage @*


export DB="sqlite3 $HOME/.config/korni/db.sqlite"
export CERTPATH=~/.config/korni/me
export KEYPATH=~/.config/korni/me.key
export PUBKEY=~/.config/korni/me.pub
export SECRETSPATH=~/.config/korni/secrets.txt
export TOPICINFO=./.korniTopic
export WORKDIR=~/.config/korni/work/
export MAGICSTART=MP
export MAGICEND=NS
TIME='date +%s'
# './dev.sh'
THISPROPG=$0

export ERRORLOG=errorKorni.log
#export PROTOPROTO='
#syntax = "proto3";
#package korni;
#message BinPacket {
  #string bin = 1;
  #string sign = 2;
# }
#message PacketStream {
  #repeated BinPacket packets = 1;
#}
#'
#echo $PROTOPROTO


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

secretAdd() {
	secret=$1
	chmod u+rw $SECRETSPATH

	echo $secret >> $SECRETSPATH
	cat $SECRETSPATH | uniq > $SECRETSPATH

	chmod 400 $SECRETSPATH
}
#secretAdd $(<test.secret)
#exit 

# print to out
me() {  #TODO
	# tmp. md5 fingerprint
	#openssl x509 -noout -in mycert.pem -fingerprint 2>/dev/null 
	
	# first line of pubkey
	#head -n 2  ~/.config/korni/me.pub | tail -n 1
	
	# md5 of pubkey
	cat $PUBKEY | openssl md5 -- | cut -d" " -f2 #awk '{print $2}'
}
#echo $(me)

# print selected secret to out
secret() {
	lineNum=2
	s=$(sed "${lineNum}q;d" $TOPICINFO)
	if ! [ -n $s ] ; then 
		echo 'need select topoi first'
		exit 1
	fi
	echo $s
}

# print to stdout
topic(){
	lineNum=1
	s=$(sed "${lineNum}q;d" $TOPICINFO)
	if ! [ -n $s ] ; then 
		echo 'need select topic first'
		exit 1
	fi
	echo $s
}

# TODO ! remove topic key pair
#korni select topic testTopic $(<test.secret) testTopicCert.pem testTopicKey.pem
selectTopic() {
	topicname=$1
	secret=$2
	#cert=$3
	#key=$4
	topicFile=$TOPICINFO
	chmod 600 $topicFile
	echo $topicname > $topicFile
	echo $secret >> $topicFile
	#	cat $cert >> $topicFile
	#	cat $key >> $topicFile
	chmod 400 $topicFile
	#echo TEST
	#cat $topicFile
}
#selectTopic testTopic $(<test.secret) # testTopicCert.pem testTopicKey.pem
#exit 

# print to stdout one line
getOutFile() {
	f=`cat .korniCurrentOutFile`
	if ! [ -f .korniCurrentOutFile ] || [ $? = 0 ] || [ -z $f ] ; then 
		# ищем готовый вариант в папке
		f=$(find $WORKDIR -type f | head -n 50 | shuf -n 1)
	fi 
	if [ -f $f ] ; then 
		# check size and maybe select create new file
		maximumsize=39000000
		actualsize=$(wc -c <"$f")
		if [ $actualsize -ge $maximumsize ]; then
			frel=$(uuidgen | tr -d '-' | sed 's/^\(.\{2\}\)/\1\//'| sed 's/^\(.\{5\}\)/\1\//'| sed 's/^\(.\{8\}\)/\1\//'| sed 's/^\(.\{11\}\)/\1\//'| sed 's/^\(.\{15\}\)/\1\//')
			f="$WORKDIR/$frel"
		fi 
	else 
		# create new 
		frel=$(uuidgen | tr -d '-' | sed 's/^\(.\{2\}\)/\1\//'| sed 's/^\(.\{5\}\)/\1\//'| sed 's/^\(.\{8\}\)/\1\//'| sed 's/^\(.\{11\}\)/\1\//'| sed 's/^\(.\{15\}\)/\1\//')
		f="$WORKDIR/$frel"
	fi
	if ! [ -f $f ] ; then 
		mkdir -v -p $(dirname ${f})
		touch $f
	fi
	echo $f
	# save to corrent state
	echo $f > .korniCurrentOutFile
}
#getOutFile
#exit

# 
jNew() {
	jitFomUser=$1 # optional
	if ! [ -f $TOPICINFO ]; then 
		echo 'ERROR: need first select topic'
		exit 1
	else
		jid=$(openssl rand -base64 24 2>/dev/null )
		author=$(me)
		admins=$(me)
		
		JSON_STRING_NO_SIGN=$( jq -n --sort-keys --compact-output  \
			--arg jid "$jid" \
			--arg uJid "$jitFomUser" \
			--arg time "$($TIME)" \
			--arg author "$author" \
			--arg admins "$admins" \
			\
			'{type: "J", jid: $jid, author: $author, admins: [$admins], uJid: $uJid, time:$time }' )
		
		# sign digest by author 
		blockB64=$(echo $JSON_STRING_NO_SIGN | base64 -w 0)
		signB64=$( echo $blockB64  | openssl dgst -sha1 \
			-sign $KEYPATH 2>/dev/null  | base64 -w 0 ) 
		#echo $blockB64 
		#echo $signB64
		packet="$blockB64 $signB64"
		
		# crypt by secret
		S=$(secret)
		#T=$(topic)
		
		# -a for base64
		CRYPTEDRESULT=$( openssl enc -aes-256-cbc -salt \
			-in <(echo $packet)\
			-pass pass:$S 2>/dev/null  | base64 -w 0 )
		#echo result $MAGICSTART$CRYPTEDRESULT$MAGICEND | cat -n 
		
		outFile=$(getOutFile)
		#echo getOutFile $outFile

		echo $MAGICSTART$CRYPTEDRESULT$MAGICEND >> $outFile
		
		#result out
		echo "$outFile"
		echo $jid
		
		## console diagnostic info
		#echo OK: append  >2
		#echo $jid        >2
	fi
}
#jNew 'someBlId'
#exit 

## read from stdin & decode json & make sql & execute it 
#indexBlocks() {
	#read line
	#json=$(echo $line | awk '{print $1}' | base64 -d)
	#binBase64=$(echo $line | awk '{print $2}' )
	#signBase64=$(echo $line | awk '{print $3}' )
	##echo $json 
	##echo $binBase64
	##echo $signBase64
	
	
	## INSERT OR REPLACE
	#(echo INSERT INTO _blocks\(bin, sign, json\) values \("'"$binBase64"'", "'"$signBase64"'", "'"$json"'" \)\; ) | \
		#$DB  
	##eval $( echo $json | jq -r '. | "author=\"" + .author +"\"" , "jid=\"" + .jid + "\"", "type=\"" + .type + "\"" ')
	##echo $jid , $type , $author 
	##echo $json | jq '.admins | .[]'
	##for a in $(echo $json | jq '.admins | .[] | tostring') ; do 
	##	#echo $a
	##	# WARNING тут почему то есть перенос строк.
	##done 
#}


# try decript && verify 
tool1() {

	DECRYPTED=''
	blockB64=''
	signB64=''
	currentKey=''

	tryDecrypt() {
		#read -r binBase64 
		binBase64=$1

		DECRYPTED='';

		secrets=$(< $SECRETSPATH)
		if [ -z $secrets ]; then 
			echo "ERROR: secrets are empty">2
			exit 1
		else
			for S in $secrets ; do 
				currentKey=$S
				# un base64 #      <(echo $binBase64)
				DECRYPTED=$( base64 -d <<<$binBase64 2>/dev/null | \
					# decrypt symmetric \
					openssl enc -d -aes-256-cbc -salt -pass "pass:$S" -- 2>/dev/null )
				if [ $? = 0 ]; then 
					blockB64=$(echo $DECRYPTED | awk '{print $1}')
					signB64=$(echo $DECRYPTED | awk '{print $2}')
					if [ $blockB64 != '' ] && [ signB64 != '' ]; then 
						verifyResult=$(echo $blockB64  | openssl dgst -sha1 -verify $PUBKEY -signature <( echo $signB64 | base64 -d ) -- )
						if [ $? = 0 ]; then 
							echo $currentKey						# 1
							#echo $blockB64
							base64 -d <<<$blockB64					# 2
							echo $signB64							# 3
							sha256sum <<<$blockB64 | cut -c 1-64 	# 4     id = sha256(base64(blockJSON))
						else
							echo ; echo ; echo ; echo ;
						fi 
					else
						echo ; echo ; echo ; echo ;
					fi 
					break
				else  
					# no result
					echo ; echo ; echo ; echo ;
				fi
			done
		fi
	}
	

	# bin prod
	while  IFS= read line; do 
		# read 2 line
		tryDecrypt $line
		#continue 
	done
	exit 
}


# DEBUG tool1
debug() {
(
	echo "U2FsdGVkX1+e0l7VhoVixfKMOqwMkzYqAT3v30FzRU6QNJ0sKX+eMlwJULyFS/v7MbegXp5MdFAyfqQp3pXLLC7mIbS9EVYVdeixdayfM5V/SvRJNwc3vX0NjgZkS6fHVQmk1OSQAw8C8dOS6aHqEsG7TQVtz2IystYlHsE71di4bU/v/kD1lLaEQA57reA9SQrrJLF6eVPWNqAVqpnr9QppdepkJ0E6ItG0Bp12XHE1PcrVd7GfZSQo2UiB6CNsXk1J7oF671lfbOqP/KX6sqjEe5qyS2eAs+jZobXE+5mMQj93lVRzer4SMGum9R6crWN/GZvHTwfHDvWNwiZsglZXFNIV+n8xCjbTgJv3epB6zooz9fIESuxIBBhA8npojDxAWI4utc0V7rlj6YvCEg== 3brmJMeMLj/u1BBxrxR/OrAH7Ns6uYMNgBZ3ryABoH/qDrwgQanAIgsBsf4DQ9ahNfy2rdhQUeV59u3Ei29EVeMXbWU7eaJ5gSUtuJqjFHqIEXsCgZ748mr82z65tP2UHShcdZC5alNBHsgAkO0xHwad0RPrIYtRVZJvkDW3Ivw="; 
	echo "U2FsdGVkX1+e0l7VhoVixfKMOqwMkzYqAT3v30FzRU6QNJ0sKX+eMlwJULyFS/v7MbegXp5MdFAyfqQp3pXLLC7mIbS9EVYVdeixdayfM5V/SvRJNwc3vX0NjgZkS6fHVQmk1OSQAw8C8dOS6aHqEsG7TQVtz2IystYlHsE71di4bU/v/kD1lLaEQA57reA9SQrrJLF6eVPWNqAVqpnr9QppdepkJ0E6ItG0Bp12XHE1PcrVd7GfZSQo2UiB6CNsXk1J7oF671lfbOqP/KX6sqjEe5qyS2eAs+jZobXE+5mMQj93lVRzer4SMGum9R6crWN/GZvHTwfHDvWNwiZsglZXFNIV+n8xCjbTgJv3epB6zooz9fIESuxIBBhA8npojDxAWI4utc0V7rlj6YvCEg== 3brmJMeMLj/u1BBxrxR/OrAH7Ns6uYMNgBZ3ryABoH/qDrwgQanAIgsBsf4DQ9ahNfy2rdhQUeV59u3Ei29EVeMXbWU7eaJ5gSUtuJqjFHqIEXsCgZ748mr82z65tP2UHShcdZC5alNBHsgAkO0xHwad0RPrIYtRVZJvkDW3Ivw=" ; 
	) | \
		awk -f <(cat <<EEE
		BEGIN {
			# https://www.gnu.org/software/gawk/manual/html_node/Two_002dway-I_002fO.html
			#command = "./dev.tool1"
			command = "./dev.sh tool1"
		}
		
		# awk map program 
		{ 	# each line   {"bin", "sign"} from block in file TODO - just bin
			bin = \$1
			sign = \$2
			# write two lines
			print bin  |& command
			print sign  |& command
			# read one line
			command |& getline decryptedBin
			if(decryptedBin != "") {
				# print 3 Field if in block all ok 
				print decryptedBin, bin, sign
				#system("jq '+={}'   ")
			}
		};
		END {
			close(command)
		}
		
		
	# system("mv -R " \$1 " " \$2) # system call 
EEE
		) -- | indexBlocks 2>>$ERRORLOG

	exit 0

}


#TODO create index
# REINDEX J in DB
reIndexJ() {
	# находим jid объекты , которые еще не созданы в J а команда для создания уже есть и создаём
	sql="insert into J(block_Id,id,author,admins, time) select id as block_Id, json_extract(json , '\$.jid') as jid, json_extract(json, '\$.author') as author, json_extract(json, '\$.admins') as admins, CAST(json_extract(json, '\$.time') as INTEGER) as time  from _blocks where json_extract(json, '\$.type') = 'J' and jid not in (select id from J) and time>0 and time<strftime('%s', 'now');"
	echo $sql | $DB
}

reIndexC() {
	# находим C объекты , которые еще не созданы в C а команда для создания уже есть и создаём
	# если права позволяют! 
	
	sql=$(cat <<EEE
insert into C(block_id, jid, key, time, value, C, author)
select 	id as block_Id
		,json_extract(json, '\$.jid') as i_jid
		,json_extract(json, '\$.key') as i_key
		,CAST(json_extract(json, '\$.time') as INTEGER) as time  
		,json_extract(json, '\$.value') as value
		,json_extract(json, '\$.C') as C
		,json_extract(json, '\$.author') as i_author
from _blocks 
where 	json_extract(json, '\$.type') = 'C' 
		/*and id not in (select id from C)*/ 
		and time > 0 
		and time < strftime('%s', 'now')
		and (
			(C='S' AND 
				/*key is new*/
				(select count() 
				from C
				where 
					jid = i_jid
					and key = i_key
				limit 1) = 0			
			)
			or 
			( C='R' AND
					/*you can rewrite*/
					(SELECT sum(json_quote(json_extract(can, '\$.R'))='1') as summ
					from P  
					where 
						userid = i_author
						and jid = i_jid
					limit 1) > 0		
			)
		);

	
EEE
) ;
	echo $sql | $DB
}


#REINDEX DB
reindexDB() {
	reIndexJ
	reIndexC
}

parseFile() {
	file=$1

	handleOneFile() {
		file=$1
		# get bins 
		# pcregrep -o1 'MP(\.*?\)NS\n'
		# grep "MP.*NS"
		cat $file | awk -f <(cat <<EEE
			BEGIN {
				OFS="\n"; ORS=""
				# https://www.gnu.org/software/gawk/manual/html_node/Two_002dway-I_002fO.html
				command = "$THISPROPG tool1"
				file = "$file"
			}		
			match(\$0, /MP.*?NS$/) { 
				binB64 = substr(\$0, RSTART+2, RLENGTH-4) 
				#print "binB64" , binB64 "\n\n" # -> { "bin": "U2FsdGV...W3Ivw=" }

				# clear var from prev step
				id = ""; secret = ""; blockText = ""; blockB64 = ""; signB64 = "";
				
				# write 1 lines
				print binB64 "\n" |& command
				
				# read 4 line - decripted and verified info 
				command |& getline secret
				command |& getline blockText
				command |& getline signB64
				command |& getline id
				#print "S1:" secret "\n", "BB:" blockText "\n", "S2:" signB64 "\n"
				
				if(secret != "" && blockText != "" && signB64 != "") {
					# print 3 Field if in block all ok 
					#print id, binB64, file, secret, blockText, signB64, "", ""
					# have secret but not using here !  
					print "INSERT INTO _blocks(id, bin, file, sign, json) values( '" id "', '" binB64 "', '" file "', '" signB64 "', '" blockText   "' );\n\n"
				}
			}
			END {
				close(command)
			}
EEE
		) | $DB 2>>$ERRORLOG; 
		
		echo $file
	}
	
	if [ -f $file ]; then 
		handleOneFile $file
	else
		# read files names from stdin
		while true ; do 
			readline file
			handleOneFile $file
		done
	fi 

	echo 'reindexDB'
	reindexDB 2>> $ERRORLOG
}
#parseFile '/home/mp/.config/korni/work/99/f0/cb/d5/7d3/1428f8ac4ab6c6bed9c57'
#exit 

permForMe() {
	jid=$1
	needPerm=$2
}

jSet() {
	jid=$1
	key=$2 ; 
	value=$3;
	#time=$($TIME)
	
	if [ -z "$value" ]; then 
		echo 'read from stdin for "S" not implemented'
		exit 1;
	else
		exists=$( echo "SELECT count() from C where jid='$jid' and key='$key' ;" | $DB )
		me=$(me)
		#echo exists me $exists $me

		makeSBlock() {
			JSON_STRING_NO_SIGN=$( jq -n --sort-keys --compact-output  \
				--arg jid "$jid" \
				--arg time "$($TIME)" \
				--arg C "S" \
				--arg key "$key" \
				--arg value "$value" \
				--arg author "$me" \
				\
				'{type: "C", jid: $jid, C: $C, time:$time, key: $key, value: $value, author: $author}' )
			
			# sign digest by author 
			blockB64=$(echo $JSON_STRING_NO_SIGN | base64 -w 0)
			signB64=$( echo $blockB64  | openssl dgst -sha1 \
				-sign $KEYPATH 2>/dev/null  | base64 -w 0 ) 
			#echo $blockB64 
			#echo $signB64
			packet="$blockB64 $signB64"
			
			# crypt by secret
			S=$(secret)
			#T=$(topic)
			
			# -a for base64
			CRYPTEDRESULT=$( openssl enc -aes-256-cbc -salt \
				-in <(echo $packet)\
				-pass pass:$S 2>/dev/null  | base64 -w 0 )
			
			outFile=$(getOutFile)
			#echo getOutFile $outFile

			echo $MAGICSTART$CRYPTEDRESULT$MAGICEND >> $outFile
			# file name to out
			echo "$outFile"
			echo OK: append 
		}
		
		if  [ -n $exists ] && [ $exists == '1' ] ; then 
			R=$(echo "SELECT json_quote(json_extract(can, '\$.R'))='1'   from P  where userid='$me' and jid='$jid' limit 1;" | $DB)
			# на перезапись ключей в журнале нужны права
			if [ $R = '1' ]; then 
				makeSBlock
			fi 
		else
			G=$(echo "SELECT json_quote(json_extract(can, '\$.S'))='1'   from P  where userid='$me' and jid='$jid' limit 1;" | $DB)
			# на установку новых значений по идее права не нужны - свобода слова, если только не известно, что их отняли
			# TODO difficult 
			if [ -z $G ] || [ $G = '0' ]; then 
				makeSBlock
			fi
		fi
		exit 0
		
	fi 
	
}

# показывает журнал как список
asMap() {
	jid=$1;
	echo 'select * from asmap ;' | $DB -quote
	#echo $sql 

	#if [ -n $jid ]; then 
	#	sql="$sql and C.jid='$jid' "
	#fi

}

dbConsole() {
	$DB -quote
}

jInsert() {
	jid=$1
	value=$2
	jSet "$jid" "$($TIME)" "$value"
}

installDepends(){ # todo debug 
	mkdir -p ~/.config/korni/  $WORKDIR
	(cat <<EEE
	
BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "C" (
	"block_id"	TEXT,
	"jid"	TEXT NOT NULL,
	"key"	TEXT NOT NULL,
	"time"	TEXT NOT NULL,
	"value"	TEXT,
	"C"	TEXT,
	"author"	TEXT,
	FOREIGN KEY("block_id") REFERENCES "_blocks"("id"),
	PRIMARY KEY("jid","key","time"),
	FOREIGN KEY("jid") REFERENCES "J"("id")
);
CREATE TABLE IF NOT EXISTS "P" (
	"jid"	TEXT,
	"userid"	TEXT,
	"can"	TEXT DEFAULT '{"G":1, "A": 0, "S":1, "R":0}',
	"moment"	INTEGER,
	"whoChange"	TEXT,
	FOREIGN KEY("jid") REFERENCES "J"("id"),
	PRIMARY KEY("userid","jid")
);
CREATE TABLE IF NOT EXISTS "J" (
	"id"	TEXT NOT NULL UNIQUE,
	"time"	NUMERIC DEFAULT 0,
	"block_Id"	INTEGER,
	"author"	TEXT NOT NULL,
	"admins"	TEXT,
	"difficult"	INTEGER DEFAULT 0,
	"Field6"	INTEGER,
	FOREIGN KEY("block_Id") REFERENCES "_blocks",
	PRIMARY KEY("id")
);
CREATE TABLE IF NOT EXISTS "_blocks" (
	"id"	TEXT,
	"bin"	TEXT,
	"sign"	TEXT UNIQUE,
	"file"	TEXT,
	"json"	TEXT NOT NULL,
	PRIMARY KEY("id")
);
CREATE TABLE IF NOT EXISTS "__U" (
	"pgp_pub"	TEXT UNIQUE,
	PRIMARY KEY("pgp_pub")
);
CREATE INDEX IF NOT EXISTS "C_time" ON "C" (
	"time"	DESC
);
CREATE INDEX "_blocks_time" ON "_blocks" (
	CAST(json_extract(json, '\$.time') as INTEGER)
);
CREATE INDEX "_blocks_type" ON "_blocks" (
json_extract(json, '\$.type')
);
CREATE INDEX IF NOT EXISTS "C_jid_key" ON "C" (
	"jid"	ASC,
	"key"	ASC
);
CREATE INDEX IF NOT EXISTS "C_C" ON "C" (
	"C"	ASC
);
CREATE INDEX IF NOT EXISTS "P_userid_jid" ON "P" (
	"userid"	ASC,
	"jid"	ASC
);
CREATE VIEW aslist as
select jid, C, value, time, author 
from asmap 
where C != 'D'

--select * from asmap ;
--select json_extract(j, '\$.test') from (select json_extract(json, '\$.value') as j from _blocks WHERE json_valid(j));

--select jid, value , json_type(value, '\$.test') as t
--from asmap 
--WHERE json_valid(value) and t='integer';
CREATE VIEW asmap as 
select C.jid, C.key, C.value, max(C.time) as time, C.C, C.author
from J inner join C on (C.jid = J.id)
GROUP by key 
/*ORDER by key asc, C.time asc*/
/*without delete*/
having C.C != 'D';
COMMIT;




EEE
	) | $DB
	
	#md5sum protobuf-compiler gawk perl 
	sudo apt-get install -y	awk pcregrep grep openssl jq sqlite3  \
							sqlitebrowser
}
#installDepends
#exit 


#OPTS 
parseOpts() {
	while getopts f:s: flag
	do
		case "${flag}" in
			f) first=${OPTARG};;
			s) second=${OPTARG};;

		esac
	done
	echo "First: $first";
	echo "Second: $second";
}
#parseOpts

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

# DEV INFO 
# https://www.simplified.guide/bash/compile-script
#shc -f dev.sh -o dev

# about attack 
# https://stackoverflow.com/a/16056298/2355077
# https://robertheaton.com/2013/07/29/padding-oracle-attack/

#good think 
#https://github.com/FiloSottile/age

#https://gist.github.com/henridf/704c1c812f04a502c1c26f77a739090b

#cat all files
#https://unix.stackexchange.com/a/76881/420369




