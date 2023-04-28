#include "my_sqlite.h"

#include <algorithm> // for_each
#include <iostream>  // cerr
#include <iterator>
#include <set>
#include <sstream>
#include <stdio.h>
#include <tuple>

#include <sqlite3.h>

using namespace std;

sqlite3* sqliteOpen(const string& dbPath)
{
    sqlite3* db;
    int rc;
    rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return (nullptr);
    }
    else
    {
        rc = sqlite3_threadsafe();
        if (rc == 0)
            fprintf(stderr, "ERROR: sqlite lib compiled not in thread save mode\n");

        //        fprintf(stderr, "Opened database successfully\n");
        return db;
    }
};
int sqliteClose(sqlite3* db)
{
    if (db != nullptr)
        return sqlite3_close(db);
    else
        return false;
};

void sqliteInsertRecordSet(sqlite3* db, const string& tableName, const RecordSet& recordSet,
                           bool isIgnode) // throws LockEror
{
    map<string /*name*/, char /*type*/> fieldsCollection;
    for (auto& record : recordSet)
        for (auto& it : record)
            if (fieldsCollection.count(it.first) == 0)
                fieldsCollection[it.first] = get<0>(it.second);

    auto columnDefinition = [](const string& name, char type) -> string {
        stringstream ss;
        ss << "[" << name << "] ";
        switch (type)
        {
        case 'n':
            ss << "NULL ";
            break;
        case 'i':
            ss << "INTEGER ";
            break;
        case 'r':
            ss << "REAL ";
            break;
        case 't':
            ss << "TEXT ";
            break;
        case 'b':
            ss << "BLOB ";
            break;
        default:
            cerr << "ERROR: sqlite understand only column types: n,i,r,t,b but present:" << type
                 << endl;
            break;
        };
        return ss.str();
    };

    auto _PKFixes = []() -> string {
        return " [id] TEXT NOT NULL , zT INTEGER NOT NULL , ";
    };
    auto computeCreateTable = [&tableName, &fieldsCollection, &columnDefinition,
                               &_PKFixes]() -> string {
        stringstream ss;
        ss << "CREATE TABLE IF NOT EXISTS [" << tableName << "] (";
        ss << _PKFixes(); // only for korni table
        std::set<string> pkS{ "id", "zT" };
        map<string, char> noPkFields;
        for (auto it : fieldsCollection)
            if (pkS.count(it.first) == 0)
                noPkFields[it.first] = it.second;

        for (auto it = noPkFields.begin(); it != noPkFields.end(); ++it)
        {
            if (pkS.count(it->first) > 0)
                continue;
            ss << columnDefinition(it->first, it->second);
            //            if (std::next(it) != noPkFields.end())
            ss << ",";
        }
        ss << "PRIMARY KEY(id,zT)"
           << ");";
        return ss.str();
    };
    //    cout << computeCreateTable();

    auto computeAlterTable = [&tableName, &fieldsCollection, &columnDefinition]() -> vector<string> {
        vector<string> result;
        for (auto it = fieldsCollection.begin(); it != fieldsCollection.end(); ++it)
        {
            stringstream ss;
            if (it->first == "id" || it->first == "zT")
                continue;

            ss << "ALTER TABLE "
               << "[" << tableName << "] ADD COLUMN ";
            ss << columnDefinition(it->first, it->second);
            ss << "; " << endl;
            result.push_back(ss.str());
        }
        return result;
    };
    //    cout << computeAlterTable();

    auto computeInsert = [&recordSet, &tableName,
                          &fieldsCollection]() -> tuple<string, vector<string>, bool> {
        vector<string> valuesToBind;
        stringstream ss;
        ss << "INSERT OR IGNORE INTO "; // REPLACE
        ss << "[" << tableName << "] (";
        //(auto it : fieldsCollection)
        for (auto it = fieldsCollection.begin(); it != fieldsCollection.end(); ++it)
            if (std::next(it) != fieldsCollection.end())
                ss << "[" << it->first << "], ";
            else
                ss << "[" << it->first << "]";
        ss << ")  VALUES ";
        { // do block
            auto final_iter = recordSet.end();
            --final_iter;
            for (auto record = recordSet.begin(); record != recordSet.end(); ++record)
            {
                ss << "(";
                for (auto it = fieldsCollection.begin(); it != fieldsCollection.end(); ++it)
                {
                    valuesToBind.push_back(
                        record->count(it->first) > 0 ? get<1>(record->at(it->first)) : "NULL");
                    ss /*<< string(it->second == 't' ? "\"" : "")*/
                        << "?"
                        //                       << string(it->second == 't' ? "\"" : "")
                        << (std::next(it) != fieldsCollection.end() ? "," : "");
                }

                if (record != final_iter)
                    ss << "),";
                else
                    ss << ")";
            }
        }
        ss << ";";
        return make_tuple(ss.str(), valuesToBind, true);
    };

    auto current = recordSet.begin();
    auto last = recordSet.end();
    auto computeInsertIgnore = [&tableName, &fieldsCollection, &current,
                                &last]() -> tuple<string, vector<string>, bool /*is end*/> {
        vector<string> valuesToBind;
        stringstream ss;
        stringstream ss2;
        auto record = current;
        ss << "INSERT OR IGNORE INTO [" << tableName << "] (";

        for (auto it = fieldsCollection.begin(); it != fieldsCollection.end(); ++it)
        {
            valuesToBind.push_back(record->count(it->first) > 0 ? get<1>(record->at(it->first))
                                                                : "NULL");
            ss << it->first << (std::next(it) != fieldsCollection.end() ? ", " : ") ");
            ss2 << "?" << (std::next(it) != fieldsCollection.end() ? "," : " ");
        }
        ss << " SELECT " << ss2.str();
        //        ss << " FROM [" << tableName << "] ";
        ss << "where NOT EXISTS(SELECT id FROM [" << tableName << "] WHERE id=?);" << endl;
        valuesToBind.push_back(get<1>(record->at("id")));
        ++current;

        return make_tuple(ss.str(), valuesToBind, current == last);
    };

    string isql = get<0>(computeInsert());
    //    cout << isql << endl;

    auto execOneNoPrintError = [&db /*, &tableName*/](const string& sqlText,
                                                      const string& /*errorText*/) {
        int rc = -1;
        //
        sqlite3_stmt* stmt;
        rc = sqlite3_prepare_v2(db, sqlText.c_str(), -1, &stmt, nullptr);
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW && rc != SQLITE_DONE)
        {
            // return record. not only status
            // string errmsg(sqlite3_errmsg(db));
            // cerr << "ERROR: sqlite " << tableName << errorText << " error:" << errmsg << endl;
            sqlite3_finalize(stmt);
        }
        if (rc == SQLITE_DONE)
            sqlite3_finalize(stmt);
        if (rc == SQLITE_BUSY)
            throw LockError();
        if (rc == SQLITE_LOCKED)
        {
            sqlite3_reset(stmt);
            throw LockError();
        }
    };
    execOneNoPrintError(computeCreateTable(), " create table ");
    for (const string& s : computeAlterTable())
        execOneNoPrintError(s, " alter table ");

    auto execInsert =
        [&db](const std::function<tuple<string, vector<string>, bool>()>& computeInsert) {
            // improve perfomabce
            char* sErrMsg = nullptr;
            int rc = 0;
            //            int rc = sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, &sErrMsg);
            //            if (rc)
            //                cerr << string(sErrMsg) << endl;
            //            rc = sqlite3_exec(db, "PRAGMA journal_mode = MEMORY", NULL, NULL, &sErrMsg);
            //            if (rc)
            //                cerr << string(sErrMsg) << endl;

            auto endTransaction = [&]() {
                int rc = sqlite3_exec(db, "END TRANSACTION ;", NULL, NULL, &sErrMsg);
                if (rc)
                    cerr << "ERROR: sqlite code=" << rc << " is a :" << sErrMsg << endl;
                // LockError ? - no i think
            };

            rc = sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &sErrMsg);
            if (rc)
            {
                cerr << "ERROR: sqlite" << string(sErrMsg) << endl;
                //                endTransaction();
                //                throw LockError();
            }

            sqlite3_stmt* stmt;

            bool isNeedStop = true;
            do
            {
                auto ins = computeInsert();

                // TODO if sql is same - not prepare.  if other - finalize
                string sql = get<0>(ins);
                //                cout << sql << endl; // TODO remove
                auto params = get<1>(ins);
                isNeedStop = get<2>(ins);

                rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
                if (rc != SQLITE_OK)
                    cerr << string(sqlite3_errmsg(db)) << endl;
                if (rc == SQLITE_BUSY)
                    throw LockError();
                if (rc == SQLITE_LOCKED)
                    throw LockError(); // not step - same beh as in busy

                string v;
                for (std::size_t index = 0; index < params.size(); ++index)
                {
                    // Using parameters ("?") is not
                    v = params[index];
                    rc = sqlite3_bind_text(stmt, index + 1, v.c_str(), -1, SQLITE_TRANSIENT);
                    if (rc != SQLITE_OK)
                    {                                      // really necessary, but recommended
                        string errmsg(sqlite3_errmsg(db)); // (especially for strings) to avoid
                        sqlite3_finalize(stmt);            // formatting problems and SQL
                        //            throw errmsg;                      // injection attacks.
                        break; // TODO error
                    }
                    if (rc == SQLITE_BUSY || rc == SQLITE_LOCKED)
                        throw LockError();
                }
                // exec insert
                if (rc == SQLITE_OK)
                {
                    rc = sqlite3_step(stmt);

                    if (rc != SQLITE_ROW && rc != SQLITE_DONE)
                    {
                        string errmsg(sqlite3_errmsg(db));
                        if (errmsg.size())
                            cerr << errmsg << endl;

                        int bytes;
                        const unsigned char* text;
                        bytes = sqlite3_column_bytes(stmt, 0);
                        text = sqlite3_column_text(stmt, 0);
                        if (bytes && text)
                            cout << bytes << text << endl;

                        //    cout << "INSERT RESULT " << sqlite3_column_text(stmt, 0) << sqlite3_column_text(stmt, 1)
                        //         << sqlite3_column_int(stmt, 2) << endl;
                    }
                    if (rc == SQLITE_DONE)
                    {
                        //        sqlite3_finalize(stmt);
                        //        throw string("customer not found");
                    }
                    if (rc == SQLITE_BUSY)
                        throw LockError();
                    if (rc == SQLITE_LOCKED)
                    {
                        //                        sqlite3_reset(stmt);
                        throw LockError();
                    }
                }

                sqlite3_reset(stmt);
                sqlite3_clear_bindings(stmt);
                sqlite3_finalize(stmt);
            } while (!isNeedStop);

            endTransaction();
        };
    if (!isIgnode)
        execInsert(computeInsert);
    else
        execInsert(computeInsertIgnore);
}

Record sqliteSelectRecordById(sqlite3* db, const string& table, const string& id)
{
    Record r;
    string sql = "SELECT a.* "
                 "FROM [" +
                 table +
                 "] a "
                 "INNER JOIN ( "
                 "  SELECT id, MAX(zT) zT "
                 "  FROM [" +
                 table +
                 "] "
                 "  GROUP BY id"
                 ") b ON a.id = b.id AND a.zT = b.zT "
                 "WHERE a.id=?; ";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        string errmsg(sqlite3_errmsg(db)); // (especially for strings) to avoid
        cerr << errmsg << endl;
        return r;
    }
    rc = sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK)
    {                                      // really necessary, but recommended
        string errmsg(sqlite3_errmsg(db)); // (especially for strings) to avoid
        sqlite3_finalize(stmt);            // formatting problems and SQL
        return r;
    }
    else
    {
        rc = sqlite3_step(stmt);

        //        cerr << sqlite3_expanded_sql(stmt) << endl; // debug

        if (rc != SQLITE_ROW && rc != SQLITE_DONE)
        {
            // return record. not only status
            string errmsg(sqlite3_errmsg(db));
            cerr << "ERROR: sqlite " << table << " error:" << errmsg << endl;
            //
            sqlite3_finalize(stmt);
        }
        if (rc == SQLITE_ROW)
        {
            //
            int bytes;
            const unsigned char* text;
            for (int i = 0; i < sqlite3_column_count(stmt); ++i)
            {
                int type = sqlite3_column_type(stmt, i);
                bytes = sqlite3_column_bytes(stmt, i);
                text = sqlite3_column_text(stmt, i);
                char t;
                switch (type)
                {
                case SQLITE_TEXT:
                    t = 't';
                    break;
                case SQLITE_INTEGER:
                    t = 'i';
                    break;
                case SQLITE_NULL:
                    t = 'n';
                    break;
                case SQLITE_FLOAT:
                    t = 'r';
                    break;
                case SQLITE_BLOB:
                    t = 'b';
                    break;
                }
                string value((char*) text, bytes);
                tuple<char /* n,i,r,t,b */, string /*val*/> f = make_tuple(t, value);
                const char* n = sqlite3_column_name(stmt, i);
                if (n != nullptr)
                {
                    string name = n;
                    r[name] = f;
                }
            }
            //
            sqlite3_finalize(stmt);
        }
        if (rc == SQLITE_DONE)
            sqlite3_finalize(stmt);
        return r;
    }
};

Record sqliteSelectFileByHash(sqlite3* db, const string& hash) // throws LockError
{
    string sql1 = "SELECT a.* FROM [files] a INNER JOIN ( "
                  "SELECT id, MAX(zT) zT FROM [files]  GROUP BY id "
                  ") b ON a.id = b.id AND a.zT = b.zT WHERE a.id=?;";

    string sql2 = "SELECT a.* FROM [files] a INNER JOIN ( "
                  "SELECT id, MAX(zT) zT FROM [files]  GROUP BY id "
                  ") b ON a.id = b.id AND a.zT = b.zT WHERE a.id LIKE ?";

    sqlite3_stmt* stmt = nullptr;
    auto trysql = [db, stmt](const string& sql, const string& param) mutable {
        Record r;

        int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            string errmsg(sqlite3_errmsg(db)); // (especially for strings) to avoid
            cerr << "ERROR in sqliteSelectFileByHash:" << errmsg << endl;
            return make_pair(false, r);
            //return r;
        }
        rc = sqlite3_bind_text(stmt, 1, param.c_str(), -1, SQLITE_TRANSIENT);
        if (rc != SQLITE_OK)
        {                                      // really necessary, but recommended
            string errmsg(sqlite3_errmsg(db)); // (especially for strings) to avoid
            std::cerr << "ERROR sql " << errmsg << endl;
            sqlite3_finalize(stmt); // formatting problems and SQL
            return make_pair(false, r);
            //return r;
        }
        else
        {
            rc = sqlite3_step(stmt);

            //        cerr << sqlite3_expanded_sql(stmt) << endl; // debug

            if (rc != SQLITE_ROW && rc != SQLITE_DONE)
            {
                // return record. not only status
                string errmsg(sqlite3_errmsg(db));
                cerr << "ERROR: sqlite "
                     << "files"
                     << " error:" << errmsg << endl;
                //
                sqlite3_finalize(stmt);
            }

            if (rc == SQLITE_ROW)
            {
                //
                int bytes;
                const unsigned char* text;
                for (int i = 0; i < sqlite3_column_count(stmt); ++i)
                {
                    int type = sqlite3_column_type(stmt, i);
                    bytes = sqlite3_column_bytes(stmt, i);
                    text = sqlite3_column_text(stmt, i);
                    char t;
                    switch (type)
                    {
                    case SQLITE_TEXT:
                        t = 't';
                        break;
                    case SQLITE_INTEGER:
                        t = 'i';
                        break;
                    case SQLITE_NULL:
                        t = 'n';
                        break;
                    case SQLITE_FLOAT:
                        t = 'r';
                        break;
                    case SQLITE_BLOB:
                        t = 'b';
                        break;
                    }
                    string value((char*) text, bytes);
                    tuple<char /* n,i,r,t,b */, string /*val*/> f = make_tuple(t, value);
                    const char* n = sqlite3_column_name(stmt, i);
                    if (n != nullptr)
                    {
                        string name = n;
                        r[name] = f;
                    }
                }
                //
                sqlite3_finalize(stmt);
                return make_pair(true, r);
            }
            else if (rc == SQLITE_DONE)
            {
                sqlite3_finalize(stmt);
                return make_pair(false, r);
            }
            else if (rc == SQLITE_LOCKED || rc == SQLITE_BUSY)
            {
                throw LockError();
                return make_pair(false, r);
            }
        }

        return make_pair(false, r);
    };
    auto r1 = trysql(sql1, hash);
    if (r1.first)
        return r1.second;
    else
    {
        auto r2 = trysql(sql2, "%" + hash + "%");
        return r2.second;
    }
};

// OK
void stringReplaceAll(string& s, const string& search, const string& replace)
{
    for (size_t pos = 0;; pos += replace.length())
    {
        // Locate the substring to replace
        pos = s.find(search, pos);
        if (pos == string::npos)
            break;
        // Replace by erasing and inserting
        s.erase(pos, search.length());
        s.insert(pos, replace);
    }
}
// another  solution   str.replace(str.find(str2),str2.length(),str3);

string recordToString(const Record& r)
{
    stringstream ss;
    ss << "{";
    for (auto it = r.begin(); it != r.end(); ++it)
    {
        string name = it->first;
        char type = get<0>(it->second);
        string v = get<1>(it->second);
        ss << "\"" << name << "\":";
        ss << (type == 't' ? "\"" : "") << v << (type == 't' ? "\"" : "");
        ss << (std::next(it) == r.end() ? "" : ",");
    }
    ss << "}";
    string s = ss.str();
    // на всякий случай
    stringReplaceAll(s, "\n", "");
    return s;
}

// TODO надо использовать тут лямбда калбеки.
bool _sqliteExecStatement(sqlite3* db, const string& sql, sqlite3_callback callback)
{
    char* zErrMsg = 0;
    //            auto callback = ;
    //            Create SQL statement
    //        sql = "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "
    //              "VALUES (1, 'Paul', 32, 'California', 20000.00 ); "
    //              "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "
    //              "VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "
    //              "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)"
    //              "VALUES (3, 'Teddy', 23, 'Norway', 20000.00 );"
    //              "INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)"
    //              "VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 );";

    //             Execute SQL statement
    int rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
    else
    {
        fprintf(stdout, "Records created successfully\n");
        return true;
    }
};

int _sqliteDisableTransactions(sqlite3* db)
{
    return _sqliteExecStatement(db, "PRAGMA synchronous = OFF");
};

// TODO DEPRECATED
bool _sqliteCreateTable(sqlite3* db, map<string /*collumn*/, string /*type*/>& columns)
{
    char* zErrMsg = 0;
    /* Create SQL statement */
    char sql[] = "CREATE TABLE COMPANY("
                 "id TEXT PRIMARY KEY     NOT NULL,"
                 "zT LONG PRIMARY KEY     NOT NULL,"
                 //"AGE            INT     NOT NULL,"
                 //"ADDRESS        CHAR(50),"
                 //"SALARY         REAL"
                 ");";

    //    struct ct
    //    {
    //        ct(){};
    //        int actor(void*, int argc, char** argv, char** azColName)
    //        {
    //            for (int i = 0; i < argc; i++)
    //            {
    //                printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    //            }
    //            printf("\n");
    //            return 0;
    //        };
    //    } callback;
    //    sqlite3_callback cb = &callback.actor;

    // TODO callbeck
    return _sqliteExecStatement(db, sql);

    //    /* Execute SQL statement */
    //    int rc = sqlite3_exec(db, sql, , 0, &zErrMsg);
    //    if (rc != SQLITE_OK)
    //    {
    //        fprintf(stderr, "SQL error: %s\n", zErrMsg);
    //        sqlite3_free(zErrMsg);
    //        return false;
    //    }
    //    else
    //    {
    //        fprintf(stdout, "Table created successfully\n");
    //        return true;
    //    }
};

// TODO переместить в отдельные файлы + разбить на функции
// TODO + написать расширения этих функций для korni (insert many jsonnl and ignore pk error + auto alter column + auto create table + auto create trigers)
int _sqliteInsertRecords(const char* /*dbPath*/)
{
    //    sqlite3* db;
    //    char* zErrMsg = 0;
    return 0;
};
