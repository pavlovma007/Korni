BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "_blocks" (
	"bin"	TEXT,
	"sign"	TEXT UNIQUE,
	"file"	TEXT,
	"topic"	TEXT,
	"json"	TEXT NOT NULL,
	PRIMARY KEY("bin")
);
CREATE TABLE IF NOT EXISTS "P" (
	"jid"	TEXT,
	"object_rowid"	INTEGER,
	"userid"	TEXT,
	"can"	TEXT,
	"moment"	INTEGER
);
CREATE TABLE IF NOT EXISTS "K" (
	"object_rowid"	INTEGER,
	"jid"	TEXT,
	"key"	TEXT,
	"time"	TEXT,
	"value"	TEXT
);
CREATE TABLE IF NOT EXISTS "J" (
	"id"	TEXT NOT NULL UNIQUE,
	"object_rowid"	INTEGER,
	"author"	TEXT NOT NULL,
	"admins"	TEXT,
	"difficult"	INTEGER,
	"Field6"	INTEGER,
	PRIMARY KEY("id")
);
CREATE TABLE IF NOT EXISTS "_objects" (
	"block_rowid"	INTEGER,
	"type"	TEXT,
	"json"	TEXT
);
CREATE TABLE IF NOT EXISTS "__U" (
	"pgp_pub"	TEXT UNIQUE,
	PRIMARY KEY("pgp_pub")
);
COMMIT;
