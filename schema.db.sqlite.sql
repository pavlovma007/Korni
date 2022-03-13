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
	CAST(json_extract(json, '$.time') as INTEGER)
);
CREATE INDEX "_blocks_type" ON "_blocks" (
json_extract(json, '$.type')
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
--select json_extract(j, '$.test') from (select json_extract(json, '$.value') as j from _blocks WHERE json_valid(j));

--select jid, value , json_type(value, '$.test') as t
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
