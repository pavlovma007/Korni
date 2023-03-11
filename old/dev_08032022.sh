#!/bin/bash 

<<MULTICOMMANT
  эта версия не доделана но я ее решил переделать. 
  тут блок это json из bin , sign. и это не правильно. 
  + надо его по другому обрабатывать итд
  
MULTICOMMANT

printUsage(){
	if [ $# -eq 0 ] 
	then
		cat <<USAGEEND
usage: korni <options> command [commandParams ...]

options:
---------

commands: 
---------

korni key add mycert.pem mykey.pem
	add new THIS user key 
korni secret add <big-secret-password>
	add secret to known
korni installDepends
	prepare system for working . install some soft from repos 
	create .config folder and structure
korni select topic <topic id>
	select known topic for data work info saved in current dir ./.korniTopic
	
korni J new 
	create new journal object . return jid of new created object
korni J show asList <jid> 
	show object as list 
korni J show asMap <jid>
	show object as map
korni J set <jid> <key> <bin>
	write value to journal
korni J set <jid> <key> 
	write value to journal but read std input bin
korni J permGet <jid>
	read permition of object. 
korni J permAdd <jid> <to> [G|S|D|A]
	grant permition to other key <to> G - get|S - set|D - delete|A - admin 
korni J permRemove <jid> [G|S|D|A]
	remove permition for SELFT for object id

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
export ERRORLOG=error.log

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
	echo $secret >> $SECRETSPATH
	cat $SECRETSPATH | uniq > $SECRETSPATH
}
#secretAdd $(<test.secret)
#exit 

# print to out
me() {  #TODO
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
		# random jid
		#jid=$(uuidgen | tr -d '-' )
		jid=$(openssl rand -base64 48 2>/dev/null )
		author=$(me)
		#echo $jid #$author
		admins=$(me)
		
		JSON_STRING_NO_SIGN=$( jq -n --sort-keys \
			--arg jid "$jid" \
			--arg uJid "$jitFomUser" \
			--arg author "$author" \
			--arg admins "$admins" \
			\
			'{type: "J", jid: $jid, author: $author, admins: [$admins], uJid: $uJid }' )
		
		# sign digest by author 
		sign=$(echo $JSON_STRING_NO_SIGN  | openssl dgst -sha1 \
			-sign $KEYPATH 2>/dev/null  | base64 -w 0 ) #openssl enc -base64 2>/dev/null  
		#echo $sign
		
		#| openssl enc -base64 -d # decode back
		
		# block with sign
		#JSONBLOCK=$(echo $JSON_STRING_NO_SIGN | \
		#	jq --arg sign "$sign" '. +={sign:$sign}')
		#echo $JSONBLOCK >test 
		
		#ЗАЧЕМ???? 
		## crypt it by topic public than by seret and write to log
		#openssl rsautl -encrypt -inkey $TOPICINFO \
		#	-pubin -in <(echo $JSONBLOCK) # -out >()
		
		#echo verify !
		## verify bin data 
		#echo $JSON_STRING_NO_SIGN | openssl dgst -sha1 \
		#	-verify $PUBKEY \
		#	-signature  test --
		
		# crypt by secret
		S=`secret` 
		T=`topic`
		# -a for base64
		CRYPTEDRESULT=$( openssl enc -aes-256-cbc -salt \
			-in <(echo $JSON_STRING_NO_SIGN)\
			-pass pass:$S 2>/dev/null  | base64 -w 0 ) #openssl base64 -e -A 2>/dev/null
		#echo result $CRYPTEDRESULT
		
		outFile=`getOutFile`
		#echo getOutFile $outFile

		# formate json out
		# https://gist.github.com/henridf/704c1c812f04a502c1c26f77a739090b
		binPacket=$( jq -n --sort-keys \
			--arg B "$CRYPTEDRESULT" \
			--arg S "$sign" \
			\
			'{bin: $B, sign: $S}' )
		echo $MAGICSTART$binPacket$MAGICEND >> $outFile
		echo OK: append $outFile
		
		# protobuf encode 
		#(
		#	echo 'packets : ['
		#	echo "{ bin: '$CRYPTEDRESULT', sign: '$sign' } "
		#	echo ']'
		#) |  protoc	--encode=korni.PacketStream 2.proto >> $outFile
		#echo 'cat outfile '
		#cat $outFile
	fi
}
#jNew 'someBlId'
#exit 


# try decript && verify 
tool1() {

	DECRYPTED=''

	tryDecrypt() {
		binBase64=$1
		secrets=$(< $SECRETSPATH)
		for S in $secrets ; do 
			# un base64
			block=$( base64 -d <(echo $binBase64) | \
				# decrypt symmetric \
				openssl enc -d -aes-256-cbc -salt -pass "pass:$S" -- 2>/dev/null )
			DECRYPTED=$block
			[[ $? != 0 ]] &&  exit 
		done
		echo ''
	}

	# bin prod
	read binBase64 
	read sign
	tryDecrypt $binBase64
	#sign='3brmJMeMLj/u1BBxrxR/OrAH7Ns6uYMNgBZ3ryABoH/qDrwgQanAIgsBsf4DQ9ahNfy2rdhQUeV59u3Ei29EVeMXbWU7eaJ5gSUtuJqjFHqIEXsCgZ748mr82z65tP2UHShcdZC5alNBHsgAkO0xHwad0RPrIYtRVZJvkDW3Ivw='
	verifyResult=$(echo $DECRYPTED | openssl dgst -sha1 -verify $PUBKEY -signature <( echo $sign | base64 -d ) -- )
	# out base64 of command json OR empty 
	[[ $? == 0 ]]  \
				&& (echo $DECRYPTED | base64 -w 0)   \
				|| echo '' ;
}


# DEBUG tool1
debug() {
(


	exit 0

}


# todo test 
reIndexJ() {
	# находим jid объекты , которые еще не созданы в J а команда для создания уже есть и создаём
	# переносим из блоков 
	sql="insert into J(block_rowId,id,author,admins) select rowId as block_rowId, json_extract(json , '\$.jid') as id, json_extract(json, '\$.author') as author, json_extract(json, '\$.admins') as admins  from _blocks where json_extract(json, '\$.type') = 'J' and id not in (select id from J);"
	echo $sql | $DB
}


installDepends(){ # todo debug 
	mkdir -p ~/.config/korni/  $WORKDIR
	(cat <<EEE
.tables 
EEE
	) | $DB
	
	#md5sum protobuf-compiler gawk perl 
	sudo apt-get install -y	awk pcregrep openssl jq sqlite3  \
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
	  help_)
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
				secretAdd $*; shift 3
				;;
			* ) 
				echo "ERROR when parse params secret"
				printUsage ; exit 1;
				;;
		esac
		;;

	  "select" )
		case $2 in 
			"topic" ) 
				selectTopic $* ; shift 4
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
				jNew $* ; exit 
				;;
			* ) 
				echo "ERROR when parse params j|J"
				printUsage ; exit 1
				;;
		esac
		;;

	  *)
		echo "ERROR when parse params"
		printUsage ; exit 1;
		;;
	esac
    
    i=$((i + 1));
    #shift 1;
done

exit 

# DEV INFO 
# https://www.simplified.guide/bash/compile-script
shc -f dev.sh -o dev

# about attack 
# https://stackoverflow.com/a/16056298/2355077
# https://robertheaton.com/2013/07/29/padding-oracle-attack/

#good think 
#https://github.com/FiloSottile/age

https://gist.github.com/henridf/704c1c812f04a502c1c26f77a739090b

cat all files
https://unix.stackexchange.com/a/76881/420369




# deprecated 
# protobuf to json deserialize
# https://stackoverflow.com/questions/65521958/how-to-deserialize-binary-protobuf-to-json-with-bash-script
parseInFile() {
	# infile with special json blocks
	file=$1
	
	# METHOD 1
	# TODO check file size < 50 МБ
#	jsonGetter=cat <<EEE
#	perl -e "my $file_content = do{local(@ARGV,$/)='/dev/stdin';<>}; \
#	@matches = $file_content =~ m/MP({.*?})NS/gi; \
#	print(@matches))   "
#EEE
	##echo 'cat $file | $jsonGetter'
	#	cat $file | perl -e 'my $file_content = do{local(@ARGV,$/)="/dev/stdin";<>};  #@matches = $file_content =~ m/MP(\{.*?\})NS/gi; print(@matches)'  \ 
	## jq filter 
	#	jq '.bin, .sign'
	## readed !
	
	# METHOD2 может он быстрее ? - тестировать
	# grep -R 'MP{.*}NS' ~/.config/korni/work/ | wc -n 
		cat $file | pcregrep -o1 'MP(\{.*?\})NS'
	
	
	# todo filter y date .... may be 
	
	#index blocks if not present 
	
	
	
	
	
	
	
}
#parseInFile '/home/mp/.config/korni/work/99/f0/cb/d5/7d3/1428f8ac4ab6c6bed9c57'
#exit


