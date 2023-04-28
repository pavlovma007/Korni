#pragma once

#include <sqlite3.h>

#include <vector>
#include <functional>
#include <string>
#include <map>

using namespace std;

class LockError : exception {};

// util
void stringReplaceAll(string& s, const string& search, const string& replace);

//   https://www.sqlite.org/datatype3.html
// n NULL. The value is a NULL value.
// i INTEGER. The value is a signed integer, stored in 0, 1, 2, 3, 4, 6, or 8 bytes depending on the magnitude of the value.
// r REAL. The value is a floating point value, stored as an 8-byte IEEE floating point number.
// t TEXT. The value is a text string, stored using the database encoding (UTF-8, UTF-16BE or UTF-16LE).
// b BLOB. The value is a blob of data, stored exactly as it was input.
typedef tuple<char /* n,i,r,t,b */, string /*val*/> Field;

typedef map<string /*name*/, Field> Record;
typedef vector<Record> RecordSet;

sqlite3* sqliteOpen(const string& dbPath);
int sqliteClose(sqlite3* db);
// TODO test , handle errors ...
void sqliteInsertRecordSet(sqlite3* db, const string& tableName, const RecordSet& recordSet,  bool isIgnode=false); // // throws LockEror

// simple INSERT
    //INSERT OR IGNORE INTO "test2" ("id", "zT", "f1", "f2")
    //select '200', '206', 'NULL', 'f2-value2'
    //-- from test2
    //-- where NOT id in (select id from test2);
// INSERT with ignore = just append line
    //INSERT OR IGNORE INTO "test2" ("id", "zT", "f1", "f2")
    //select '200', '206', 'NULL', 'f2-value2'
    // from test2
    // where NOT EXISTS(select id from test2 where id=200); -- beter

// возвращает последнюю по времени запись с таким идентификатором. юзера не учитывает  - просто последняя по времени
Record sqliteSelectRecordById(sqlite3* db, const string& table, const string& id);

Record sqliteSelectFileByHash(sqlite3* db, const string& hash); // throws LockError

string recordToString(const Record& r);


// TODO отказаться от этих функций.
// TODO надо использовать тут лямбда калбеки.
typedef int (*sqlite3_callback)(void*,  /* Data provided in the 4th argument of sqlite3_exec() */
                                int,    /* The number of columns in row */
                                char**, /* An array of strings representing fields in the row */
                                char**  /* An array of strings representing column names */
);
// TODO проба лямбда типа
typedef function<int(void*,  /* Data provided in the 4th argument of sqlite3_exec() */
               int,    /* The number of columns in row */
               char**, /* An array of strings representing fields in the row */
               char**  /* An array of strings representing column names */
)>   functional_sqlite3_callback;




bool _sqliteExecStatement(
    sqlite3* db, const string& sql,
    sqlite3_callback callback = [](void* , int argc, char** argv, char** azColName) -> int {
        int i;
        for (i = 0; i < argc; i++)
        {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
    });

int _sqliteDisableTransactions(sqlite3* db);

// TODO
bool _sqliteCreateTable(map<string /*collumn*/, string /*type*/>& collumns);

// TODO переместить в отдельные файлы + разбить на функции
// TODO + написать расширения этих функций для korni (insert many jsonnl and ignore pk error + auto alter column + auto create table + auto create trigers)
int _sqliteInsertRecords(const char* dbPath);
