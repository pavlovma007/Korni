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

FOR DEVELOPER

wrap <dbfile> <tablename>	- create triggers for sync table. then use monitor

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

sudo apt-get  install inotify-tools sqlite3 
USAGEEND
		exit 1
	fi
}
#printUsage @*


DBFILE="$HOME/.config/korni/db.sqlite"
DB="sqlite3 $DBFILE"
SIGNALFILE=.korniNotify
REPOPATH=~/.config/korni/repo
GIT="git -C $REPOPATH"

# todo add commands
export CERTPATH=~/.config/korni/me
export KEYPATH=~/.config/korni/me.key
export PUBKEY=~/.config/korni/me.pub
export SECRETSPATH=~/.config/korni/secrets.txt


# https://blog.budgetwithbuckets.com/2018/08/27/sqlite-changelog.html
wrapTable1() {
	db=$1
	table=$2
	echo $db $table
	
	(cat <<EEE

/*Change log table*/
CREATE TABLE if not exists change_log  (
    id INTEGER PRIMARY KEY,
    created TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    action TEXT,
    table_name TEXT,
    obj_id INTEGER,
    changes TEXT
);


/*Insert Trigger*/
drop TRIGGER J_track_insert;

CREATE TRIGGER J_track_insert
AFTER INSERT ON J
BEGIN
  INSERT INTO change_log (action, table_name, obj_id, changes) 
    SELECT
    'INSERT', 'J', NEW.id, 'insert into J(id,time,block_id,author,admins,difficult) VALUES (' || COALESCE(NEW.id , '')|| ' , 
			' || COALESCE(NEW.time , '') || ', 
			' || COALESCE(NEW.block_id , '') || ', 
			' || COALESCE(NEW.author , '') || ', 
			' || COALESCE(NEW.admins , '') || ', 
			' || COALESCE(NEW.difficult , '') ||' 
			)' as changes  ;
END ;
-- Update Trigger
drop trigger IF EXISTS ${table}_track_update ;
CREATE TRIGGER if not exists  ${table}_track_update
AFTER UPDATE ON $table
BEGIN
  INSERT INTO change_log (action, table_name, obj_id, changes)
  SELECT
    'UPDATE', '$table', OLD.id, changes
  FROM
    (SELECT
      json_group_object(col, json_array(oldval, newval)) AS changes
    FROM
      (SELECT
        json_extract(value, '\$[0]') as col,
        json_extract(value, '\$[1]') as oldval,
        json_extract(value, '\$[2]') as newval
      FROM
        json_each(
          json_array(
            json_array('id', OLD.id, NEW.id),
            json_array('created', OLD.created, NEW.created),
            json_array('name', OLD.name, NEW.name),
            json_array('age', OLD.age, NEW.age)
          )
        )
      )
    );
END;

/*Delete Trigger*/
drop trigger IF EXISTS ${table}_track_delete ;
CREATE TRIGGER  if not exists ${table}_track_delete
AFTER DELETE ON $table
BEGIN
  INSERT INTO change_log (action, table_name, obj_id, changes)
  SELECT
    'DELETE', '$table', OLD.id, changes
  FROM
    (SELECT
      json_group_object(col, json_array(oldval, newval)) AS changes
    FROM
      (SELECT
        json_extract(value, '\$[0]') as col,
        json_extract(value, '\$[1]') as oldval,
        json_extract(value, '\$[2]') as newval
      FROM
        json_each(
          json_array(
            json_array('id', OLD.id, null),
            json_array('created', OLD.created, null),
            json_array('name', OLD.name, null),
            json_array('age', OLD.age, null)
          )
        )
      )
    );
END;


EEE
)  | $DB


}

wrapTable() {
	db=$1
	table=$2
	#echo $db $table

	echo "/*drop table if exists _changes ;*/   drop TRIGGER IF EXISTS ${table}_track_insert;"  | $DB
	
	echo "
	
	CREATE TABLE if not exists  _changes  (
		tableName	TEXT UNIQUE
	) ;

	CREATE TRIGGER if not exists  ${table}_track_insert
	AFTER INSERT ON ${table}
	BEGIN
	  INSERT OR IGNORE INTO _changes(tableName) VALUES('${table}' ) ;
	END ;


	drop trigger IF EXISTS ${table}_track_update ;
	CREATE TRIGGER if not exists  ${table}_track_update
	AFTER UPDATE ON $table
	BEGIN
	  INSERT OR IGNORE INTO _changes(tableName) VALUES('${table}' ) ;
	END;
	
	drop trigger IF EXISTS ${table}_track_delete ;
	CREATE TRIGGER  if not exists ${table}_track_delete
	AFTER DELETE ON $table
	BEGIN
	  INSERT OR IGNORE INTO _changes(tableName) VALUES('${table}' ) ;
	END;
	
	
	" | $DB ;
}

me() {  #TODO
	# tmp. md5 fingerprint
	#openssl x509 -noout -in mycert.pem -fingerprint 2>/dev/null 
	
	# first line of pubkey
	#head -n 2  ~/.config/korni/me.pub | tail -n 1
	
	# md5 of pubkey
	cat $PUBKEY | openssl md5 -- | cut -d" " -f2 #awk '{print $2}'
}

monitor() {
	#inotifywait -m -q -r --format '%T % e %w%f'  --excludei '/trash/' \
	#	--timefmt '%s' -e modify   -e CLOSE_WRITE \
	#	$DBFILE     | \
	#$DB  ".dump J" > $REPOPATH/J.sql
	
	$GIT config --local --replace-all user.email "humm@n.ism" 
	$GIT config --local --replace-all user.name $(me)
	
	$GIT add $REPOPATH/J.sql
	$GIT commit -m 'table J'
	#$GIT repack #gc
	
	
	# find .git/objects -type f
	# git cat-file -p 89745a608de1604f34d4291b79bae5cad526f20a
	# git verify-pack -v .git/objects/pack/pack-3e9f14792a0b448cdf45e9e3d963224c003008a5.pack
	
	#https://git-scm.com/book/en/v2/Git-Internals-Maintenance-and-Data-Recovery
	#git fsck --full
	#git count-objects -v
	
	# git verify-pack -v .git/objects/pack/pack-29…69.idx \
	# | sort -k 3 -n \
	# | tail -3
	
	# git rev-list --objects --all 
	# git log --oneline --branches -- git.tgz # for one file 

	# TODO CRYPT LINE BY LINE 
}

installDepends() {
	echo 'not implemented'
	
	#sudo apt install liblua5.1-0-dev
	#git clone git@github.com:philanc/luatweetnacl.git
	#cd luatweetnacl
	#LUAINC=/usr/include/lua5.1  make  --trace
	
	mkdir -p ~/.config/korni/repo
	cd  ~/.config/korni/repo ; git init ; 
	
	exit 0
}


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

	  installDepends )
		installDepends; shift 1
		;;

	  wrap ) 
		db=$2
		table=$3
		wrapTable $db $table
		shift 3
		exit 0
		;;
	  monitor )
		monitor
		exit 0
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
