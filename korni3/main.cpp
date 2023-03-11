#include <algorithm> // std::sort
#include <cstdlib>   // atof
#include <ctime>     // tm time date
#include <map>
#include <pwd.h>
#include <regex>
#include <set>
//#include <signal.h>   // SIGINT
#include <sys/stat.h> // stat fileInfo
#include <time.h>     // timegm

#include <iostream> // stdin stdout stderr open close

// boost
#include <boost/json.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "my_command.h"
#include "my_crypto_utils.h"
#include "my_filesystem.h"
#include "my_geo.h"
#include "my_sha256.h" // used in korniSignText2 // http://www.zedwood.com/article/cpp-sha256-function
#include "my_sqlite.h"

using namespace std;
namespace pt = boost::property_tree;

// Buffer for file read operations. The buffer must be able to accomodate
// the RSA signature in whole (e.g. 4096-bit RSA key produces 512 byte signature)
//#define BUFFER_SIZE 512

namespace
{
// RE MAKE - make several self users with ids for select one or other
constexpr char KORNIPATH[] = "~/.config/korni/";
constexpr char MYKEYPATH[] = "~/.config/korni/MY-KEYS/";
constexpr char LOGINPATH[] = "~/.config/korni/MY-KEYS/selected";
constexpr char KEYPATH[] = "~/.config/korni/MY-KEYS/${nickname}-secret.pem";
constexpr char PUBKEY[] = "~/.config/korni/MY-KEYS/${nickname}-pub.pem";
//constexpr char CERTPATH[] = "~/.config/korni/me";
constexpr char GEOPLACEFILENAME[] = "geoplace";
constexpr char SQLITEDBPATH[] = "~/.config/korni/${CONTAINER}/database.db";

const string FILESTABLENAME = "files";

// changable vars
//string HOMEDIR;
struct passwd* pw;
string KORNIEXEPATH = "";
} // namespace

int systemCurrentEpochTimestamp()
{
    time_t now = time(0);
    tm* gmtm = gmtime(&now);
    //    epoch time
    time_t TIME_STAMP = timegm(gmtm);
    //    cout << TIME_STAMP << endl;
    return TIME_STAMP;
};

// OK
string korniUserId()
{
    string selectedNicknamePath = ::LOGINPATH;
    selectedNicknamePath.replace(selectedNicknamePath.find("~"), 1, ::pw->pw_dir);
    if (filesystemPathFileOrDirOrOther(selectedNicknamePath.c_str()) != 1)
    {
        cerr << "ERROR: not logined. you must to login, use one of your keypair" << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        const char* homedir = ::pw->pw_dir;
        string pubPath(::PUBKEY);
        stringReplaceAll(pubPath, string("~"), string(homedir));
        string nickname = filesystemReadFileIntoString(selectedNicknamePath);
        pubPath.replace(pubPath.find("${nickname}"), 11, nickname);
        stringReplaceAll(pubPath, "\n", "");
        //    cout << pubPath;
        string pubFileContent = filesystemReadFileIntoString(pubPath);
        //    cout << "raw file path = " << pubFileContent << "\n";
        stringReplaceAll(pubFileContent, "\n", "");
        stringReplaceAll(pubFileContent, "-----BEGIN PUBLIC KEY-----", "");
        stringReplaceAll(pubFileContent, "-----END PUBLIC KEY-----", "");
        //    cout << "file path = " << homedir << "/.korni/" << pubFileContent << "\n";

        return pubFileContent;
    }
}

// TODO без аргумента - печатает то , что сейчас установлено
//
void korniLogin(const string& nickname)
{
    string pub = ::PUBKEY;
    // check nickname exist // TODO move check to korniLogin too
    pub.replace(pub.find("~"), 1, ::pw->pw_dir);
    pub.replace(pub.find("${nickname}"), 11, nickname);

    if (filesystemPathFileOrDirOrOther(pub.c_str()) != 1)
    {
        cerr << "ERROR: with this nickname keypair not exists " << endl;
        exit(EXIT_FAILURE);
    }
    else

        if (nickname.empty())
    {
        cout << nickname << "\t" << korniUserId() << endl;
        exit(EXIT_SUCCESS);
    }
    else
    {
        // login
        string loginPath(::LOGINPATH);
        loginPath.replace(loginPath.find("~"), 1, ::pw->pw_dir);
        filesystemWriteStringIntoFile(loginPath, nickname);

        cout << nickname << "\t" << korniUserId() << endl;
        exit(EXIT_SUCCESS);
    }
}

string korniContainerDir(const string& containerName = "")
{
    string target = string(::pw->pw_dir) + "/" + ".config/korni/" + containerName + "/";
    filesystemShureDirecroryExists(target);
    return target;
}

// OK
// impl: fast sign using openssl lib , using current PriveteKey
// return base64(sign(sha256(text), my current private key))
// if error - return empty string
string korniSignText2(const string& text)
{
    string textSha256Hash = sha256(text);

    string selectedNicknamePath = ::LOGINPATH;
    selectedNicknamePath.replace(selectedNicknamePath.find("~"), 1, ::pw->pw_dir);

    const char* homedir = ::pw->pw_dir;
    string path(::KEYPATH);
    stringReplaceAll(path, string("~"), string(homedir));
    string nickname = filesystemReadFileIntoString(selectedNicknamePath);
    path.replace(path.find("${nickname}"), 11, nickname);
    stringReplaceAll(path, "\n", "");

    string privateText = filesystemReadFileIntoString(path);
    if (0 == privateText.length())
    {
        cerr << "ERROR: can not read private key  from " << path << endl;
        return "";
    }

    string result = signMessage(privateText, text);
    result.replace(result.find_last_of("\n"), 1, "");
    return result;

    //    FILE* privkeyFd = fopen(path.c_str(), "r");
    //    RSA* rsa_privatekey = PEM_read_RSAPrivateKey(privkeyFd, NULL, NULL, NULL);
    //    fclose(privkeyFd);
    //    if (rsa_privatekey == 0)
    //    {
    //        cerr << "ERROR: can tor read private key from " << path << endl;
    //        exit(EXIT_FAILURE);
    //    }
    //    // sign
    //    unsigned char signature[200];
    //    bzero(signature, 200);
    //    unsigned int signatureSize = 0;
    //    int signResult = RSA_sign(NID_sha256, (const unsigned char*) textSha256Hash.c_str(),
    //                              textSha256Hash.length(), signature, &signatureSize, rsa_privatekey);
    //    if (signResult == 1)
    //    {
    //        // base64
    //        char* signatureBase64Encoded = base64encode(&signature[0],
    //                                                    signatureSize); //Base-64 encoding.

    //        //  form result
    //        string result;
    //        result.assign(signatureBase64Encoded);
    //        free(signatureBase64Encoded);
    //        return result;
    //    }
    //    else
    //        return "";
}

// OK
// impl: fast verify using openssl lib
// if pubkey=="" then read pubkey from ::PUBKEY, else compude key in place from pubkey
// return "error string message" if error, "" empty string otherwise
string korniVerifySign2(const string& text, const string& signatureBase64, const string& pubkey = "")
{
    string pubkeyText;
    if (pubkey == "")
    {
        string fname = ::PUBKEY;
        fname.replace(fname.find("~"), 1, ::pw->pw_dir);
        pubkeyText = filesystemReadFileIntoString(fname);
    }
    else
        pubkeyText = "-----BEGIN PUBLIC KEY-----\n" + pubkey + "\n-----END PUBLIC KEY-----";

    if (verifySignature(pubkeyText, text, signatureBase64.c_str()))
        return "";
    else
    {
        cerr << getOpenSSLError();
        return "Signature is invalid";
    }
    // OLD code verion

    //    //    static unsigned char signatureBuffer[BUFFER_SIZE];
    //    //    const char* filename = "";
    //    //    const char* sigfile = "";
    //    //    const char* pubkeyfile = "argv[3]";

    //    // TODO use SHA256 class from "sha256.h"

    //    // Calculate SHA256 digest for datafile
    //    //    SHA256SmallImplementation ctx;
    //    //    string digestS = ctx.computeHashFromFile(filename);
    //    string digestS = sha256(text);
    //    //    unsigned char digest[SHA256SmallImplementation::DIGEST_SIZE];

    //    // Read signature from file
    //    //    unsigned signatreBytes = 0;
    //    //    FILE* sign = fopen(sigfile, "r");
    //    //    signatreBytes = fread(sugnatureBuffer, 1, BUFFER_SIZE, sign);
    //    //    fclose(sign);
    //    //
    //    // signature decode
    //    unsigned char* signatureBuffer = (unsigned char*)
    //        base64decode((const void*) signatureBase64.c_str(), signatureBase64.length());
    //    unsigned signatreBytes = strlen((char*) signatureBuffer);

    //    // Verify that calculated digest and signature match
    //    //    FILE* pubkeyFd = fopen(pubkeyfile, "r");
    //    // Read public key from file
    //    //    RSA* rsa_pubkey = PEM_read_RSA_PUBKEY(pubkeyFd, NULL, NULL, NULL);
    //    //
    //    // Read public key from string
    //    BIO* bo = BIO_new(BIO_s_mem());
    //    if (!bo)
    //    {
    //        throw std::runtime_error("Failed to create key BIO");
    //    }
    //    BIO_write(bo, pubkeyText.c_str(), pubkeyText.length());
    //    //    EVP_PKEY* pkey = 0;
    //    //    PEM_read_bio_PrivateKey(bo, &pkey, 0, 0);
    //    RSA* rsa_pubkey = PEM_read_bio_RSA_PUBKEY(bo, nullptr, nullptr, nullptr); // !!!
    //    if (!rsa_pubkey)
    //    {
    //        return "ERROR: can not read public key";
    //    }
    //    BIO_free(bo);

    //    // Decrypt signature (in buffer) and verify it matches
    //    // with the digest calculated from data file.
    //    int result = RSA_verify(NID_sha256, (const unsigned char*) digestS.c_str(),
    //                            SHA256SmallImplementation::DIGEST_SIZE,
    //                            (const unsigned char*) signatureBuffer, signatreBytes, rsa_pubkey);
    //    RSA_free(rsa_pubkey);
    //    free(signatureBuffer);

    //    if (result == 1)
    //    {
    //        return ""; // signature is valid
    //    }
    //    else
    //    {
    //        cerr << getOpenSSLError();
    //        return "Signature is invalid";
    //    }
}

// OK
// place has format  <longitute double>,<latitude double>
void korniSetGeoPlace(const string& place)
{
    // place has format  <longitute double>,<latitude double>
    // compute geohash from it
    // https://github.com/davetroy/geohash-js/blob/master/geohash.js   run node ?
    // save to <korniPath>/geohash
    string left = place.substr(0, place.find(",")), right = place.substr(place.find(",") + 1, -1);
    double l = stod(left), lat = stod(right);
    string geoHash = encodeGeoHash(l, lat);
    //    cerr << geoHash << endl;

    stringstream ss;
    ss << "geohash:" << geoHash << endl;
    ss << "time:" << systemCurrentEpochTimestamp() << endl;
    // todo time as human readable string
    string fileData = ss.str();

    string filepath = KORNIPATH;
    filepath.replace(filepath.find("~"), 1, ::pw->pw_dir);
    filepath += ::GEOPLACEFILENAME;

    filesystemWriteStringIntoFile(filepath, fileData);
}

// OK
bool korniCurrentGeoPlace(string& geohashPlace)
{
    string path = ::KORNIPATH;
    path.replace(path.find("~"), 1, ::pw->pw_dir);
    path += ::GEOPLACEFILENAME;

    string s = filesystemReadFileIntoString(path);
    std::regex rgx = regex("geohash:(.*)");
    std::smatch matches;

    if (std::regex_search(s, matches, rgx))
    {
        //        std::cout << "Match found\n";
        //        for (size_t i = 0; i < matches.size(); ++i)
        //            std::cout << i << ": '" << matches[i].str() << "'\n";
        geohashPlace = matches[1].str();
        return true;
    }
    else
        return false;
}

// OK
string korniPathForTableChanges(const string& userId, const string& containerName,
                                const string& tableName)
{
    // https://www.softwaretestinghelp.com/date-and-time-in-cpp/
    // https://www.tutorialspoint.com/cplusplus/cpp_date_time.htm

    // current date/time based on current system
    time_t now = time(0);
    // convert now to string form
    //    char* dt = ctime(&now);
    // convert now to tm struct for UTC
    tm* gmtm = gmtime(&now);
    //    dt = asctime(gmtm);
    //    cout << "The UTC date and time is:" << dt << endl;

    stringstream sout;
    sout << 1900 + gmtm->tm_year << "-" << 1 + gmtm->tm_mon << "-" << gmtm->tm_mday;
    string dateTooday = sout.str();
    //    cout << "date=" << result << endl;
    // epoch time
    //    time_t TIME_STAMP = timegm(gmtm);
    //    cout << TIME_STAMP << endl;

    string useridHash = sha256(userId);
    vector<string> pathParts;
    auto splitUserIdTo3Level = [&]() {
        // если useridHash кодировать hexdigit (sha hash as string)
        // то `16*16=256`;   `16*16*16=4096`  `4096*16=65536`
        //        **3  символа = нормально , 4 уже много!**
        //        буду кодировать блоками по 3 символа
        pathParts.push_back("U-" + useridHash.substr(0, 3));
        pathParts.push_back(useridHash.substr(2 * 1, 3));
        pathParts.push_back(useridHash.substr(2 * 2, -1 /*3*/));
    };
    splitUserIdTo3Level();

    string curGeoPlace;

    // "/home/mp/.config/korni/test/tables/2022/2022-9-15/U-e1b/b3e/e611c573511be2fb3891d023527d6078e4bfbfad3e126be5f5c9e916fb77/T-node-2022-9-15-GH-uf7d5zbvz7qt.jsonnl.gz"
    string result(korniContainerDir(containerName) + "tables/" + to_string(1900 + gmtm->tm_year) +
                  "/" + dateTooday + "/" + pathParts[0] + "/" + pathParts[1] + "/" + pathParts[2] +
                  "/" + "T--" + tableName + "--D-" + dateTooday + "-" +
                  (korniCurrentGeoPlace(curGeoPlace) ? "GH-" + curGeoPlace : "") + ".jsonnl.gz");
    return result;
};

// OK
string korniPathOfDatabase(const string& containerName)
{
    string path = ::SQLITEDBPATH;
    path.replace(path.find("~"), 1, ::pw->pw_dir);
    path.replace(path.find("${CONTAINER}"), 12, containerName);

    return path;
}

bool is_digits(const std::string& str)
{
    return str.find_first_not_of("0123456789.") == std::string::npos;
}

string ptObjectOrListToString(const boost::property_tree::ptree& pt)
{
    if (pt.empty())
    {
        string s = pt.data(), sl = s;
        transform(s.begin(), s.end(), sl.begin(), ::tolower);
        string result;

        if (sl == "false" || sl == "true")
            result = sl;
        else if (is_digits(s) && s.find(".") == std::string::npos)
            result = s;
        else if (is_digits(s) && s.find(".") != std::string::npos)
            result = s;
        else if (sl == "null")
            result = "null";
        else
        {
            stringReplaceAll(s, "\"", "\\\"");
            result = "\"" + s + "\"";
        }

        return result;
    }
    else
    {
        bool isArray = false;
        for (const auto& it : pt)
        {
            isArray = it.first == "";
            break;
        }
        stringstream ss;
        ss << (isArray ? "[" : "{");
        for (auto it = pt.begin(); it != pt.end(); ++it)
        {
            if (!isArray)
                ss << "\"" << it->first << "\":";
            ss << ptObjectOrListToString(it->second);
            if (std::next(it) != pt.end())
                ss << ",";
        }
        ss << (isArray ? "]" : "}");
        return ss.str();
    }
};

// OK
// TODO иногда вместо чисел при содании таблицы для поля типа epoch int создаётся поле text. почему? см vedeos
// не сначала создавать changeset а потом писать в бд. а сначала проверить в бд id и не создавать change set

// small TODO использовать внутренний нативный , а не внешний, gzip
bool korniCreateRecord(
    const string& dbName, const string& tableName, const string& rawJsonRecord,
    string& dataOut /*если ignore и в бд есть запись - сюда выводится ее json*/,
    bool isIgnore = false /*если запись с таким id (без учёта zT) есть в БД - не вставлять запись*/,
    bool isSkipDbUpdate = false /*просто добавить файл на диск, но не обновлять БД*/)
{
    // check db exists
    // check json contain 'id' field
    // add current user field & timestamp & record sign record
    // gzip record & compute path & append file
    // if ok - insert record to database

    string korniPath = ::KORNIPATH;
    korniPath.replace(korniPath.find("~"), 1, ::pw->pw_dir);
    string containerPath = korniPath + dbName + "/";
    if (!filesystemIsPathExist(containerPath))
    {
        cerr << "ERROR: container path not exists: " << containerPath << endl;
        exit(EXIT_FAILURE);
    }

    Command cmd;

    stringstream ss(rawJsonRecord);

    boost::property_tree::ptree jsontree;
    boost::property_tree::json_parser::read_json(ss, jsontree);

    // get and remove sign from it
    string recordId;
    try
    {
        recordId = jsontree.get<string>("id");
    }
    catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>& e)
    {
        cerr << "ERROR: record must has `id` field. record json=" << rawJsonRecord << endl;
        exit(EXIT_FAILURE);
    };
    if (isIgnore)
    {
        sqlite3* db = sqliteOpen(korniPathOfDatabase(dbName) /*+ "?mode=ro"*/);
        Record r = sqliteSelectRecordById(db, tableName, recordId);
        sqliteClose(db);
        if (r.size() > 0)
        {
            dataOut = recordToString(r);
            return true;
        }
    }

    string userId = korniUserId();
    int timestamp = (int) systemCurrentEpochTimestamp();

    jsontree.put<int>("zT", timestamp);
    jsontree.put<string>("zU", userId);
    ss.str("");
    ss.clear();
    boost::property_tree::json_parser::write_json(ss, jsontree, false);
    string jsonForSign = ss.str();
    jsonForSign.replace(jsonForSign.find_last_of("\n"), 1, "");

    dataOut = jsonForSign;

    string sign = korniSignText2(jsonForSign);
    string outPath = korniPathForTableChanges(korniUserId(), dbName, tableName);
    size_t found = outPath.find_last_of('/');
    if (found != string::npos)
    {
        string dir = outPath.substr(0, found + 1);
        filesystemShureDirecroryExists(dir);
    }

    jsontree.put<string>("zS", sign);
    ss.str("");
    ss.clear();
    boost::property_tree::json_parser::write_json(ss, jsontree, false);
    string jsonForGzip = ss.str();
    cmd.command = " gzip -9  >>  '" + outPath + "'";
    cmd.stdIn = jsonForGzip;
    cmd.execute();

    if (cmd.ExitStatus == 0)
    {
        int result = -1;
        if (!isSkipDbUpdate)
        {
            // OLD
            //            cmd.command = "sqlite-utils insert \"" + korniPathOfDatabase(dbName) + "\"  \"" +
            //                          tableName + "\"  -   --pk=id --pk=zT --ignore --nl   ";
            //            cmd.stdIn = jsonForSign;
            //            cmd.execute();
            //            result = cmd.ExitStatus;

            // save record to database native way
            string dbPath = korniPathOfDatabase(dbName);
            sqlite3* db = sqliteOpen(dbPath);
            if (!db)
            {
                cerr << "ERROR: can not open sqlite database by path" << dbPath << endl;
                exit(1);
            }
            else
            {
                Record r;
                for (auto pos = jsontree.begin(); pos != jsontree.end(); ++pos)
                {
                    if (pos->first != "zS")
                    {
                        string s, sl;
                        if (pos->second.empty())
                            s = pos->second.data();
                        else
                            s = ptObjectOrListToString(pos->second);

                        sl = s;
                        transform(s.begin(), s.end(), sl.begin(), ::tolower);

                        if (sl == "false" || sl == "true")
                            r[pos->first] = make_tuple('i', (sl == "true" ? "1" : "0"));
                        else if (is_digits(s) && s.find(".") == std::string::npos)
                            r[pos->first] = make_tuple('i', s);
                        else if (is_digits(s) && s.find(".") != std::string::npos)
                            r[pos->first] = make_tuple('r', s);
                        else if (sl == "null")
                            r[pos->first] = make_tuple('n', "NULL");
                        else
                            r[pos->first] = make_tuple('t', s);
                    }
                }
                sqliteInsertRecordSet(db, tableName, RecordSet{ r }, isIgnore);
                result = 0;
            }
        }
        return (result == 0);
    }
    else
    {
        cerr << "ERROR: cannot write info to file " << outPath << endl
             << " Error text = " << cmd.stdErr << endl;
        return false;
    }
}

// OK
// --rm , --emptySize --skipMeta (for "already downloaded") option , --skip-db-update
// TODO если ignore - надо лезть в базу и печатать sdout как будто обработано. ЛИБО считать все по этому файлу но не добавлять в БД и диск
// small TODO store to database using native way
// TODO --out-file-id
bool korniStoreFile(
    const string& containerName, const string& fileOrDirPath,
    bool isIgnore = false /*если такой id уже есть не делать*/, bool isNeedRemove = false,
    bool isNeedToEmptyFile = false /*после сохранения контета исходный файл удалить*/,
    bool skipMeta = false /*вставить только см файл, но запись в таблице files не создавать*/,
    bool isSkipDbUpdate = false /*запись на диске создать, но не обновлять БД*/,
    bool isLns = false /* сделать символический линк на файл*/)
{
    // if file or dir
    // iterate file in & compute internal path (or just name)
    // compute hashs
    // put files in container to `files`
    // make "file record" in files table
    // print to STDERR tsv: <korni file id>\t<path>
    // write file record to files table on dost in container dir
    // and to STDOUT print that file Json(nl) records with info
    // you may separate this channels and handle it

    if (!filesystemIsPathExist(fileOrDirPath))
    {
        cerr << "ERROR: file path not exists: " << fileOrDirPath << endl;
        exit(EXIT_FAILURE);
        return false;
    }
    else
    {
        auto handleOneFile = [&containerName, isIgnore](const string& fileInPath, string& outId,
                                                        string& outPath) -> bool {
            Command cmdSha, cmdMd5, cmdTorrent, cmdIpfs;

            cmdSha.command = "sha256sum -b \"" + fileInPath + "\" | awk '{print $1}'";
            cmdMd5.command = "md5sum -b \"" + fileInPath + "\" | awk '{print $1}'";
            //            cmdTorrent.command =
            //                "cd /tmp; out=$(transmission-create '" + fileInPath +
            //                "' | "
            //                "grep 'done' | awk '{print $3}' | sed  's/\"//g'); transmission-show $out  | grep "
            //                "'Hash:' | awk '{print $2}' ; rm $out";
            cmdTorrent.command = "sha1sum -b \"" + fileInPath + "\" | awk '{print $1}'";
            cmdIpfs.command = "ipfs add --only-hash \"" + fileInPath +
                              "\" 2>/dev/null | grep 'added '| awk '{print $2}'";
            cmdSha.execute([&]() {
                cmdTorrent.execute([&]() {
                    cmdIpfs.execute([&]() {
                        cmdMd5.execute();
                    });
                });
            });
            if (cmdIpfs.stdOut.empty() || cmdMd5.stdOut.empty() || cmdSha.stdOut.empty() ||
                cmdTorrent.stdOut.empty())
            {
                cerr << "ERROR: compute file hash's. errors: " << cmdIpfs.stdErr << endl
                     << cmdMd5.stdErr << endl
                     << cmdTorrent.stdErr << endl
                     << cmdSha.stdErr << endl;
                return false;
            }

            // after end "several" external programm
            string gfsHash = cmdIpfs.stdOut, tHash = cmdTorrent.stdOut, md5Hash = cmdMd5.stdOut,
                   s256Hash = cmdSha.stdOut;
            // log errors
            if (cmdTorrent.ExitStatus != 0)
                filesystemWriteStringIntoFile("TORRENT-error.log", cmdTorrent.stdErr, true);

            // GFS-QmVKD18YqaAftH9rb99nMLKA7rGBWqM8rmD9Q4a6pfjhaL-S1-7bac1d7ca579214232065d8e5fb0c09ae9896282-M-6a7da134e822da8a5e8fd0e70601d5c0-S256-76ac4be60275b9ae272b33bb1f6d896bb82c0b8de3312dfc7d1c0238f815f2ad
            string fileNameOut = "GFS-" + gfsHash + "-S1-" + tHash + "-M-" + md5Hash + "-S256-" +
                                 s256Hash;
            stringReplaceAll(gfsHash, "\n", "");
            stringReplaceAll(tHash, "\n", "");
            stringReplaceAll(md5Hash, "\n", "");
            stringReplaceAll(s256Hash, "\n", "");
            stringReplaceAll(fileNameOut, "\n", "");

            outId = fileNameOut;

            // ipfs hash validating is:
            // 1-9A-HJ-NP-Za-km-z
            // /^\/ipfs\/Qm[1-9A-HJ-NP-Za-km-z]{44}(\/.*)?|^\/ipns\/.+/
            // in one symbol mey be combinations 9+8+5+11+11+14=58 (base58 encoding)
            // 58*58=3364 - normal ;  58×58×58=195112 - is a overload.
            // step by two digit

            string p1 = gfsHash.substr(2, 2), p2 = gfsHash.substr(4, 2), p3 = gfsHash.substr(6, 2);
            string outdir = korniContainerDir(containerName) + ::FILESTABLENAME + "/" + p1 + "/" +
                            p2 + "/" + p3 + "/";
            filesystemShureDirecroryExists(outdir);
            string outFile = outdir + fileNameOut;
            outPath = outFile; // export from this lambda

            // check if outFile exists & ignore - skip copy
            if (filesystemIsPathExist(outFile) && isIgnore)
                return true;
            else
            {
                Command cmdCopy;
                cmdCopy.command = "cp -i \"" + fileInPath + "\" \"" + outFile + "\" ";
                cmdCopy.execute();
                if (filesystemPathFileOrDirOrOther(outFile.c_str()) == 1)
                {
                    // OUT STD ERR
                    cerr << fileNameOut << "\t"
                         << "\"" + fileInPath + "\"" << endl;

                    return true;
                }
                else
                    return false;
            }
        };

        auto addFileInfoToFileTable = [&fileOrDirPath, &containerName, &isSkipDbUpdate,
                                       &isIgnore](const string& id, const string& path,
                                                  bool isInDir) {
            struct stat fileInfo;
            if (filesystemIsPathExist(path, &fileInfo))
            {
                string relativePath = path;
                if (isInDir)
                    relativePath.replace(relativePath.find(fileOrDirPath), fileOrDirPath.length(),
                                         "");
                //
                long sizeInBytes = fileInfo.st_size;
                string creationTime(std::ctime(&fileInfo.st_ctime));
                stringReplaceAll(creationTime, "\n", "");
                string lastModTime(std::ctime(&fileInfo.st_mtime));
                stringReplaceAll(lastModTime, "\n", "");
                Command cmd;
                cmd.command = "file \"" + path + "\" ; ";
                cmd.execute();

                boost::property_tree::ptree jsonPt;
                jsonPt.put("id", id);
                jsonPt.put("path", relativePath);
                jsonPt.put("size", sizeInBytes);
                jsonPt.put("creationTime", creationTime);
                jsonPt.put("lastModTime", lastModTime);
                jsonPt.put("fileOut", cmd.ExitStatus == 0 ? cmd.stdOut : "");
                stringstream ss("");
                boost::property_tree::json_parser::write_json(ss, jsonPt, false);
                string json{ ss.str() };
                stringReplaceAll(json, "\n", "");

                string dataOut;
                if (!id.empty() && korniCreateRecord(containerName, ::FILESTABLENAME, json, dataOut,
                                                     isIgnore, isSkipDbUpdate))
                    cout << dataOut << endl;
            }
        };

        string fileId;
        string outFile;
        auto block1 = [&](const string& path) -> bool {
            if (handleOneFile(path, fileId, outFile))
            {
                if (!skipMeta)
                    addFileInfoToFileTable(fileId, fileOrDirPath, false);
                if (isNeedToEmptyFile)
                    filesystemTrancateFile(path);
                if (isNeedRemove)
                    filesystemRemoveFile(path);
                if (isLns)
                {
                    filesystemRemoveFile(path);
                    filesystemSymLink(path, outFile);
                }

                return true;
            }
            else
                return false;
        };

        // is file
        if (filesystemPathFileOrDirOrOther(fileOrDirPath.c_str()) == 1)
            return block1(fileOrDirPath);
        // is dir
        if (filesystemPathFileOrDirOrOther(fileOrDirPath.c_str()) == 2)
        {
            stringstream ss(filesystemFindAlternative(fileOrDirPath));
            string line;
            while (std::getline(ss, line, '\n'))
                block1(line);
            return true; // handle dir
        }
        return false; // is a soket ?
    }
    return false;
};

// OK
string korniGetFilePathInDisk(const string& containerName, const string& korniFileId)
{
    string gfsHash = korniFileId.substr(korniFileId.find("GFS-") + 4, 46 /*magic gfs hash len*/);
    string p1 = gfsHash.substr(2, 2), p2 = gfsHash.substr(4, 2), p3 = gfsHash.substr(6, 2);
    string outdir = korniContainerDir(containerName) + ::FILESTABLENAME + "/" + p1 + "/" + p2 +
                    "/" + p3 + "/";
    string filePath = outdir + korniFileId;
    return filePath;
}

// OK
// impl: but execute `openssl` for key gen
// return: put|write  key,pub files to special dir(korni specific).
bool korniGenKey(const string& nickname = "", int bits = 4096)
{
    // gen key pair using openssls
    // gen using length of key
    // save key to "special place" in KORNIPATH directory

    // why 4096 https://danielpocock.com/rsa-key-sizes-2048-or-4096-bits/

    // 1 private key : openssl genrsa -out testUser-key.pem 4096
    // 2 public key :  openssl rsa -in testUser-key.pem -pubout -out testUser-pub.pem
    // chmod 644 pub ; chmod 600 key

    // test if files are exists : override it or not (need prees y)

    string outPath = ::MYKEYPATH;
    outPath.replace(outPath.find("~"), 1, ::pw->pw_dir);
    filesystemShureDirecroryExists(outPath);

    string keyPath = outPath + nickname + "-secret.pem", pubPath = outPath + nickname + "-pub.pem";

    bool needOverride = true;
    if (filesystemPathFileOrDirOrOther(keyPath.c_str()) == 1 ||
        filesystemPathFileOrDirOrOther(pubPath.c_str()) == 1)
    {
        cout << "WARNING key with this nickname are exists! need override? (y/n)" << endl;
        char c;
        cin >> c;
        needOverride = (c == 'y');
    }

    if (needOverride)
    {
        Command cmd;
        cmd.command = "cd /tmp ; openssl genrsa -out '" + keyPath + "' " + to_string(bits) + ";  " +
                      "openssl rsa -in '" + keyPath + "' -pubout -out '" + pubPath +
                      "'; chmod 644 '" + pubPath + "'; chmod 600 '" + keyPath + "';  ";
        cmd.execute();
        return (cmd.ExitStatus == 0);
    }
    else
        return false;
}

// OK
// read "special directory" with keys
// print to stdout available keys (basicaly key nicknames) in format // TODO
void korniListUserKeys(vector<string>* out = nullptr)
{
    string outPath = ::MYKEYPATH;
    outPath.replace(outPath.find("~"), 1, ::pw->pw_dir);
    filesystemShureDirecroryExists(outPath);

    std::regex rgx = regex("/(.*)-secret.pem");
    std::smatch matches;

    filesystemFindAlternative(outPath, false, [&](const string& path) {
        if (std::regex_search(path, matches, rgx))
        {
            //        std::cout << "Match found\n";
            //        for (size_t i = 0; i < matches.size(); ++i)
            //            std::cout << i << ": '" << matches[i].str() << "'\n";
            string s = matches[1].str();
            s.replace(0, outPath.length() - 1, "");
            cout << s << endl;
            if (out != nullptr)
                out->push_back(s);
        }
    });
}

// OK
void korniVerifyRecordsFromStdIn()
{
    // helper utility
    // read record from stdin
    // from record get userid , sign
    // check sign ,
    // if ok - print to stdout just record data json without sign field.
    // then user can import this checked record to his db using sys pipe channel

    boost::property_tree::ptree jsontree;
    //    string line;
    //    while (cin >> line)
    std::string line;
    while (std::getline(std::cin, line, '\n'))
    {
        try
        {
            stringstream ss(line);
            boost::property_tree::json_parser::read_json(ss, jsontree);
            //write_json(std::cout, p);

            // get and remove sign from it
            string zS, zU, zT;
            try
            {
                zS = jsontree.get<string>("zS");
                zU = jsontree.get<string>("zU");
                zT = jsontree.get<string>("zT");
            }
            catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>& e)
            {
                cerr << "ERROR: record not contain zU & zT fields " << line << endl;
                continue;
            }

            for (auto it = jsontree.begin(); it != jsontree.end(); ++it)
                if (it->first == "zS")
                    jsontree.erase(it);

            string recordJson;
            ss.str("");
            ss.clear();
            boost::property_tree::json_parser::write_json(ss, jsontree, false);
            recordJson = ss.str();
            recordJson.replace(recordJson.find_last_of("\n"), 1, "");
            //        cerr << recordJson << endl;

            string error = korniVerifySign2(recordJson, zS, zU);
            if ("" == error)
                cout << recordJson << endl;
            else
                cerr << error << endl;
        }
        catch (boost::exception& e)
        {
        }
    }
    //    if(cin.bad())
    exit(EXIT_SUCCESS);
}

// OK
// TODO если нет места на диске - не пытаться делать
void korniRepackGzTables(const string containerName, const string& filter = "")
{
    // when user make many changes out gz is a <chunk>+<chunk>+<chunk>+<chunk>+<chunk>
    // user can repack gz to single <chunk> and file size can degrees
    // this function read container gz table changes
    // if <filter> are empty - repack all files
    // if <filter> : repack only files where in path find substring <filter>
    // in pseudocode : zcat <in.gz> | gzip --best > in.gz (rewrite | repack)
    // file size will be smaller
    // to STDOUT print tsv : <percent compress size(100*new/old)>\t<oldsize>\t<newsize>\t<repacked file>
    // to STDERR print (last line): <total saved bytes>
    // to STDERR print (not last line): errors when handle
    // return "" if no error; return eror string when error
    // to

    stringstream ss(filesystemFindAlternative(korniContainerDir(containerName) + "tables/"));
    string line, id;
    char tmpFileName[] = "/tmp/kornitemp.XXXXXX";
    int fd = mkstemp(tmpFileName); // Creates and opens a new temp file r/w.
                                   // Xs are replaced with a unique number.
    if (fd == -1)
        cerr << "ERROR : can not open TMP file in " + string(tmpFileName)
             << endl; // Check we managed to open the file.
    Command cmd;
    int economy = 0;
    struct stat fileInfo;

    while (std::getline(ss, line, '\n'))
    {
        if (line.find(".jsonnl.gz") > 0 && (filter == "" || line.find(filter) != -1))
        {
            // handle
            long oldSize = 0, newSize;
            // /home/mp/.config/korni/test/tables/2022/2022-9-22/U-e1b/b3e/e611c573511be2fb3891d023527d6078e4bfbfad3e126be5f5c9e916fb77/T--files--D-2022-9-22-GH-uf7d5zbvz7qt.jsonnl.gz
            if (!filesystemIsPathExist(line, &fileInfo))
                cerr << "ERROR: can not read file " << line << endl;
            else
            {
                oldSize = fileInfo.st_size;

                // repack
                cmd.command = "cp -f '" + line + "' '" + string(tmpFileName) + "'; zcat '" +
                              tmpFileName + "' | gzip --best >'" + line + "'; ";
                cmd.execute();
                if (cmd.stdErr != "")
                    cerr << "ERROR on file: " << line << " text:" << cmd.stdErr << endl;
                else
                {
                    filesystemIsPathExist(line, &fileInfo);
                    newSize = fileInfo.st_size;
                    //
                    float percent = 100.0 * newSize / oldSize;
                    economy += oldSize - newSize;
                    cout << fixed << setprecision(2) << percent << "\t" << setprecision(0)
                         << oldSize << "\t" << newSize << "\t"
                         << "\'" + line + "\"" << endl;
                }
            }
        }
    }
    // free
    unlink(tmpFileName);
    cerr << "economy-bytes " << economy << endl;
}

// util function
std::regex rgxDate = regex("/tables/\\d+/([\\d-]*)/U-");
string dateTableChanges(const string& path)
{
    std::smatch matches;
    if (!std::regex_search(path, matches, rgxDate))
        return "";
    else
    {
        auto date = matches[1].str();
        return date;
    }
};

// OK
// small TODO test filter by user list
void korniFilterChangeSet(const string containerName,
                          std::function<void(const string& tableName, const string& changeSetPath)>
                              foundedChangeSetHandler,
                          std::function<void(const string& table)> foundtableHandler,
                          const string& fromDate = "", const vector<string>& userList = {},
                          const string& geoHashPoint = "", float geoRadius = 0)
{
    // dev internal utility
    // doc in `korniScanTables` // TODO
    // filter and call <foundedChangeSetHandler> with json-nl records of selected table
    // call <foundtableHandler> when found new table file with criteria

    std::regex rgxGH = regex("-GH-([^\\.]*)");
    std::regex rgxTableName = regex("T--(.*)--D");
    // ./2022/2022-9-15/U-e1b/b3e/e611c573511be2fb3891d023527d6078e4bfbfad3e126be5f5c9e916fb77/T--node--D-2022-9-15-GH-uf7d5zbvz7qt.jsonnl.gz
    std::regex rgxUserHash = regex(
        "\\d{4}-\\d{1,2}-\\d{1,2}/U-(.../.../"
        "............................................................)/");
    auto geoHash = [&](const string& path) -> string {
        std::smatch matches;
        if (!std::regex_search(path, matches, rgxGH))
            return "";
        else
        {
            auto geohashPlace = matches[1].str();
            return geohashPlace;
        }
    };
    auto geoDistance = [&](const string& ghPoint1, const string& ghPoint2) -> float {
        if (ghPoint1 == "" || ghPoint2 == "")
            return NAN;
        double p1x, p1y, p2x, p2y;
        decodeGeoHash(ghPoint1, p1x, p1y);
        decodeGeoHash(ghPoint2, p2x, p2y);

        float distance = calcGPSDistance(p1x, p1y, p2x, p2y);
        return distance;
    };
    auto tableName = [&](const string& path) -> string {
        std::smatch matches;
        if (!std::regex_search(path, matches, rgxTableName))
            return "";
        else
        {
            auto table = matches[1].str();
            return table;
        }
    };
    auto userHashFromCSPath = [&](const string& path) -> string {
        std::smatch matches;
        if (!std::regex_search(path, matches, rgxUserHash))
            return "";
        else
        {
            auto userHash = matches[1].str();
            stringReplaceAll(userHash, "/", "");
            return userHash;
        }
    };
    auto handleFile = [&](const string& path) -> void {
        string table = tableName(path);
        foundtableHandler(table);
        if (table.length() > 0)
            foundedChangeSetHandler(table, path);
    };

    std::map<string, char> userHashMap;
    std::for_each(userList.cbegin(), userList.cend(), [&](const string& u) {
        userHashMap[sha256(u)] = 'y';
    });

    stringstream ss(filesystemFindAlternative(korniContainerDir(containerName) + "tables/"));
    string line;
    while (std::getline(ss, line, '\n'))
    {
        // lexicographical compare are working here
        if (fromDate.length() > 0 && dateTableChanges(line) <= fromDate)
            continue; // skip
        // distance check
        float distance = -1.0;
        if (geoHashPoint.length() > 0 && (distance = geoDistance(geoHash(line), geoHashPoint)) &&
            !isnan(distance) && distance > geoRadius)
            continue; // skip if big distance

        if (userList.size() > 0)
        {
            string userHash = userHashFromCSPath(line);
            if (userHashMap.count(userHash))
                continue;
        }

        handleFile(line);
    }
}

// OK
// TODO файлы которые просканированы писать в снапшот журналы, чтобы можно было откатится на предыдущий.
// small TODO оптимизация. использовать нативный импорт в БД - написать свой, быстрый.
void korniScanTables(const string& containerName, const string& fromDate = "",
                     const vector<string>& userList = {}, const string& geoHashPoint = "",
                     float geoRadius = 0)
{
    // function write table updates to /sqlite/ db
    // and filter files changes
    // if <userList> are empty: handl all available users , else: only from list (updates from friends?)
    // if <fromDate> are empty: handle all dates, else: new records from that date in past (format: 2001-09-11)
    // if <geoHashPoint> are empty: handle all geo positions, else: only filechanges in <geoRadius > from point <geoHashPoint>
    // before handle: verify sign in records
    // for hadnle import use the external tool: `pip3 install sqlite-utils` OR 'pip3 install spyql'
    //
    // to STDOUT print:
    // to STDERR print:
    //
    // pseudocode replace : korniFilterChangeSet(<user conditions>) -> paths | <split by tables; set table=...> | xargs zcat | korni verify | sqlite-utils <db> import <table> --pk=id --nl
    // INFO: execute system sqlite-utils (python utility)

    string databasePath = korniPathOfDatabase(containerName);
    if (filesystemPathFileOrDirOrOther(databasePath.c_str()) != 1)
    {
        // need create database, it not exists
        // TODO move this to separete function
        Command cmd;
        cmd.command = "sqlite-utils create-database \"" + databasePath + "\"";
        cmd.execute();
        if (cmd.ExitStatus)
        {
            cerr << "ERROR: database not exists and creation error. path=" << databasePath
                 << " ; creation error: " << cmd.stdOut << cmd.stdErr << endl;
            exit(EXIT_FAILURE);
        }
    }

    map<string /*table*/, vector<string /*path*/>> filesByTable;

    long pathCountForImportTotal = 0;
    korniFilterChangeSet(
        containerName,
        [&](const string& tableName, const string& changeSetPath) {
            string d = dateTableChanges(changeSetPath);
            filesByTable[(string) tableName].push_back(changeSetPath);
            ++pathCountForImportTotal;
        },
        [&](const string& table) {
            auto res = filesByTable.count(table);
            if (res == 0)
                filesByTable[table] = vector<string>{};
        },
        fromDate, userList, geoHashPoint, geoRadius);

    int handledCounter = 0;
    string cmd;
    for (auto iter = filesByTable.begin(); iter != filesByTable.end(); ++iter)
    {
        auto table = iter->first;
        auto paths = filesByTable[table];
        std::sort(paths.begin(), paths.end(), [](const string& a, const string& b) -> bool {
            string da = dateTableChanges(a);
            string db = dateTableChanges(b);
            return da < db;
        });
        for (string path : paths)
        {
            cmd = "zcat \"" + path + "\" |  \"" + ::KORNIEXEPATH +
                  "\" verify 2>/dev/null  |  sqlite-utils  insert  \"" + databasePath + "\"   \"" +
                  table + "\" -  --pk=id --pk=zT --ignore  --nl --alter ";
            int status;
            execCommand(cmd, status, [](char* dta, int size) {
                cerr << string(dta, size);
            });
            ++handledCounter;

            cout << (status == 0 ? " OK \t" : "FAIL\t")
                 << to_string(handledCounter) + "/" + to_string(pathCountForImportTotal) + "\t"
                 << path << endl;
        }
    }
    exit(EXIT_SUCCESS);
}

// OK
// выводит на STDOUT данные в формате фильтрации `sqlite-utils rows`
// small TODO использовать нативную фильтрацию.
void korniTableRows(const string containerName, const string& table, const vector<string>& params)
{
    Command cmd;
    cmd.command = "sqlite-utils rows \"" + korniPathOfDatabase(containerName) + "\"  \"" + table +
                  "\"  ";
    for (auto item : params)
        cmd.command += "'" + item + "'  ";
    cmd.execute();
    cout << cmd.stdOut << endl; // TODO ограниченный объём вывода - зависнет.
    cerr << cmd.stdErr << endl;
    exit(EXIT_SUCCESS);
}

// TODO пока не  надо
// void korniSql(const string containerName, const string sql){};

// OK
void korniDBToStdOut(const string& containerName)
{
    cout << korniPathOfDatabase(containerName) << endl;
}

void parseCommandLine(int argc, char** argv)
{
    // genkey <nickname> [<bits; 4096 by default>]
    if ((argc == 3 || argc == 4) && strcmp(argv[1], "genkey") == 0)
    {
        string nickname = argv[2];
        string bits;
        if (argc == 3)
        {
            if (korniGenKey(nickname))
                exit(EXIT_SUCCESS);
            else
                exit(EXIT_FAILURE);
        }
        else
        {
            bits = argv[3];
            if (korniGenKey(nickname, stoi(bits)))
                exit(EXIT_SUCCESS);
            else
                exit(EXIT_FAILURE);
        };
    }
    // test OK
    //    cerr << korniGenKey("testUser", 4096) << endl;

    // utility helper `find` иногда виснет. эта утилита для итерирования содержимого папки
    // FIND <dir>
    //    if (argc == 3 && (strcmp(argv[1], "FIND") == 0))
    //    {
    //        string path = argv[2];
    //        filesystemFindAlternative(path, [](const string& path) {
    //            cout << path << endl;
    //        });
    //    }

    // command: korni3 keys
    // print to stdout keys nicknames
    if (argc == 2 && strcmp(argv[1], "keys") == 0)
    {
        korniListUserKeys();
        exit(EXIT_SUCCESS);
    }
    // test
    //    korniListUserKeys();

    // login <nickname>
    // login # when no nickname - just print current selected user info (nick + publickey or id) // TODO
    // select surrent nickname "mask" of user; And crypto key
    // if success print: <usernick> <userid=publickkey>
    if (argc == 3 && (strcmp(argv[1], "login") == 0))
    {
        string nickname = argv[2];
        korniLogin(nickname);
    }

    // create|insert <dbName> <tableName> [<jsonRecord>] [--skip-db-update] [--ignore]
    // create|insert <dbName> <tableName> [--skip-db-update] # json read from stdin
    if ((argc >= 4) && (strcmp(argv[1], "create") == 0 || strcmp(argv[1], "insert") == 0))
    { // create record
        string dbName = argv[2];
        string tableName = argv[3];
        bool isIgnore = false, isSkipDbUpdate = false;
        int optCount = 0;
        for (int i = 4; i < argc; ++i)
        {
            if (string(argv[i]) == "--ignore")
            {
                isIgnore = true;
                ++optCount;
            }
            if (string(argv[i]) == "--skip-db-update")
            {
                isSkipDbUpdate = true;
                ++optCount;
            }
        }
        string notUsed;
        if (argc - optCount == 5)
        {
            string recordJson = argv[4];
            if (korniCreateRecord(dbName, tableName, recordJson, notUsed, isIgnore, isSkipDbUpdate))
                exit(EXIT_SUCCESS);
            else
                exit(EXIT_FAILURE);
        }
        else if (argc - optCount == 4)
        {
            string recordJson;
            while (std::getline(std::cin, recordJson, '\n'))
                korniCreateRecord(dbName, tableName, recordJson, notUsed, isIgnore, isSkipDbUpdate);
            exit(EXIT_SUCCESS);
        }
    }

    // repack <containerName> <file-filter>
    //
    if (argc == 4 && strcmp(argv[1], "repack") == 0)
    {
        string container = argv[2];
        string filter = argv[3];
        korniRepackGzTables(container, filter);
        exit(EXIT_SUCCESS);
    }

    // file <dbName> <fileOrDirPath> [--skipmeta] [--skipdbupdate] [--ignore] [--rm] [--empty] [--lns]
    if (argc >= 3 && argc <= 8 && strcmp(argv[1], "file") == 0)
    { // load file to system
        string dbName = argv[2];

        typedef set<string> parsedSingleParameterResultType;
        auto parseSingleOptions =
            [&](vector<string> optionNames) -> parsedSingleParameterResultType {
            parsedSingleParameterResultType result;
            for (int i = 0; i < argc; ++i)
            {
                string option(argv[i]);
                std::for_each(optionNames.begin(), optionNames.end(), [&](const string& item) {
                    if (option.find(item) == 0)
                        result.insert(item);
                });
            };
            return result;
        };
        auto parsed = parseSingleOptions(
            vector<string>{ string("--skipmeta"), string("--skipdbupdate"), string("--ignore"),
                            string("--rm"), string("--empty"), string("--lns") });

        if (argc - parsed.size() == 4)
        {
            // path in arg
            string fileOrDirPath = argv[3];
            cout << (bool) parsed.count("--lns") << endl;
            korniStoreFile(dbName, fileOrDirPath, parsed.count("--ignore"), parsed.count("--rm"),
                           parsed.count("--empty"), parsed.count("--skipmeta"),
                           parsed.count("--skipdbupdate"), parsed.count("--lns"));
            exit(EXIT_SUCCESS);
        }
        else
        {
            // path in stdin
            //   /media/mp/ac22fd5c-3daf-4f5a-a4bf-65b346984f64/BACKUP/KORNI-DATA/yt-korni-WfoAvg6ooB/КАНАЛУ ГОД! Отчет о проделанной работе и почему канал называется 'МЕХАМОЗГ' [EvOSqaXSy9A].mp4
            string fileOrDirPath;
            while (std::getline(std::cin, fileOrDirPath, '\n'))
                korniStoreFile(dbName, fileOrDirPath, parsed.count("--ignore"),
                               parsed.count("--rm"), parsed.count("--empty"),
                               parsed.count("--skipmeta"), parsed.count("--skipdbupdate"),
                               parsed.count("--lns"));
            exit(EXIT_SUCCESS);
        }
    }
    // test add file directory regular
    //    korniStoreFile("test", "/home/mp/Qt/QtCreator-v8.0.1/bin/qt.conf"); // test
    // test list directory
    //    string out = filesystemFindAlternative("/home/mp/Qt/QtCreator-v8.0.1/share");
    //    cout << out << endl;
    // test add files directory
    //    korniStoreFile("test", "/home/mp/Qt/QtCreator-v8.0.1/share"); // test

    // setgeo <lat like 58.05139>,<lat like 38.83802>
    // setgeo <lon like 58.05139> <lon like 38.83802>
    // INFO : set current user current/new geoplace.
    if ((argc == 3 || argc == 4) && strcmp(argv[1], "setgeo") == 0)
    {
        string geoplace;
        if (argc == 3)
            geoplace = argv[2];
        else if (argc == 4)
            geoplace = string(argv[2]) + "," + string(argv[3]);
        korniSetGeoPlace(geoplace);
        exit(EXIT_SUCCESS);
    }
    // test OK
    //    korniSetGeoPlace("58.05139,38.83802");

    // GEOHASH <latitude like 58.05139> < longitude like 38.83802>
    // GEOHASH <latitude like 58.05139>,< longitude like 38.83802>
    // INFO helper utility
    // print to stdout geohash string of position
    if ((argc == 4 || argc == 3) && strcmp(argv[1], "GEOHASH") == 0)
    {
        if (argc == 4)
        {
            double latitude = atof(argv[2]), longitude = atof(argv[3]);
            cout << encodeGeoHash(latitude, longitude);
            exit(EXIT_SUCCESS);
        }
        else if (argc == 3)
        {
            string coord = string(argv[2]), p1, p2;
            p1 = coord.substr(0, coord.find(","));
            p2 = coord.substr(coord.find(",") + 1, -1);
            double latitude = stof(p1), longitude = stof(p2);
            cout << encodeGeoHash(latitude, longitude);
            exit(EXIT_SUCCESS);
        }
        else
            exit(EXIT_FAILURE);
    }
    // test ok
    //    cout << "heohash of 58.05139/38.83802 = " << encodeGeoHash(58.05139, 38.83802)
    //         << " and mus be uf7d5zbvz7q" << endl;
    // test ok
    //    double olat, olong;
    //    decodeGeoHash("uf7d5zbvz7q", olat, olong);
    //    cout << "test decodeGeoHash(\"uf7d5zbvz7q\")   latlon bust be 58.05139/38.83802. so it "
    //            "computed as : "
    //         << olat << "/" << olong << endl;

    // verify
    // INFO: verify record from changelist files
    // record readed from stdin ; successfuly verified records go to stdout ; error print to stderr
    if (argc == 2 && strcmp(argv[1], "verify") == 0)
    {
        korniVerifyRecordsFromStdIn();
    }
    // test
    // test data 1(fail): {"fam":"pavlov","id":"uuid-text","name":"misha","zS":"SlrUcXRNI+","zT":1663235861,"zU":"MIG"}
    // test data 2(fail): {"fam":"pavlov","id":"uuid-text","name":"misha","zT":"1663235861","zU":"MIG"}
    // test data 3(ok  ): {"fam":"pavlov","id":"uuid-text","name":"misha","zS":"SlrUcXRNI+LMBeW3JuWENGllK31Fj6CDheL3I0osN5wENREfE95f/7rJNG66FlCqWs2WQdL0W9w1NtjJKU1IPzdWmTdB5of234SleK3qrAyJmh0dxDWC/rztyxr4cQpfotR6nBeq8pn3Q6AXkmeRasG/K6ZXzClkeAmCfF62iek=","zT":1663235861,"zU":"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDOrZEaOJqsWOPryE1H3dJhqaAG6eeo+6fGfl9FiEMjDPfWiaMypGfFF4uKMMKQrU1G94BIDP8nJ6bJgmsTWoTeIOrx5p9eixKVQwTmfkps3EiUv/iEVASIJaGIuSutmNJ69uNIaU2IjLdObcmuOIs20UdLvkVTicA+f+yzWvqgAQIDAQAB"}
    //    korniVerifyRecordsFromStdIn();

    // TODO test
    // scan <containerName> [ filter condition options ]
    // --geo-r <radius>     //  in circel with radius around current place
    // --date <start date>  //  fresher than the date
    // --u <u1> --u <user2> --u <u3> ..... // list of target users
    // TODO --table <table>  // scan only the table
    if (argc >= 3 && strcmp(argv[1], "scan") == 0)
    {
        string containerName(argv[2]);

        typedef multimap<string /*optionName*/, string /*value*/> parsedParameterResultType;
        auto parseOptions = [&](vector<string> optionNames) -> parsedParameterResultType {
            parsedParameterResultType result;
            for (int i = 0; i < argc; ++i)
            {
                string option(argv[i]);
                std::for_each(optionNames.begin(), optionNames.end(), [&](const string& item) {
                    if (option.find(item) == 0)
                        result.insert(std::pair<string, string>(option, argv[++i]));
                });
            };
            return result;
        };
        auto parsed = parseOptions(
            vector<string>{ string("--u"), string("--date"), string("--geo-r") });

        string dateFilter, geoHashFilter;
        float radiusFilter = 0.0;
        vector<string> userListFilter;
        if (parsed.count("--date") > 0)
            dateFilter = parsed.find("--date")->second;
        if (parsed.count("--geo-r") > 0)
        {
            radiusFilter = stof(parsed.find("--geo-r")->second);
            //
            string path(::KORNIPATH);
            path += ::GEOPLACEFILENAME;
            path.replace(path.find("~"), 1, ::pw->pw_dir);
            //
            geoHashFilter = filesystemReadFileIntoString(path);
            size_t p1 = geoHashFilter.find("geohash") + 8, p2 = geoHashFilter.find("\n");
            geoHashFilter = geoHashFilter.substr(p1, p2 - p1);
            stringReplaceAll(geoHashFilter, "\n", "");
        }
        if (parsed.count("--u") > 0)
        {
            for (auto&& it : parsed)
            {
                if (it.first == "--u")
                    userListFilter.push_back(it.second);
            };
        }
        korniScanTables(containerName, dateFilter, userListFilter, geoHashFilter, radiusFilter);
    }

    // rows <containrName> <table> [options]
    // options are: --csv --table -c <column> -c <column2> --where <condition> ...
    // $ korni3 rows test testTable --csv --table  -c id -c zT -c name --where 'id = 1'
    if (argc >= 4 && strcmp(argv[1], "rows") == 0)
    {
        string containerName(argv[2]);
        string table(argv[3]);
        vector<string> params;
        for (int i = 4; i < argc; ++i)
            params.push_back(string(argv[i]));
        korniTableRows(containerName, table, params);
    }

    // db <containerName>
    // утилита печатает на std out путь к файлу БД для прямой работы с базой
    // надо для запросов чтения  через `sqlite3`
    // используется например так:
    // $ sqlite3 `korni3 db test` 'select * from ttt limit 5;'
    // db <containerName>
    if (argc == 3 && strcmp(argv[1], "db") == 0)
    {
        string containerName(argv[2]);
        korniDBToStdOut(containerName);
        exit(EXIT_SUCCESS);
    }

    // File Hash To Path
    // fhtp <containerName> <file hash>
    //
    // full id example : GFS-Qmc2XXEz3JwPj1gKV1pXP7EfccDdinkmGwFeFZ1tWGEk3c-S1-9fd27f480268020f4c59f8ebc059dad7de0120ce-M-c9798284ae72e973cd7a0ebaf7adb146-S256-23af490d33a9d732f48811ab21f2bfad4405b40d3ced353dbaf7f0940a5e4e1b
    // Qmc2XXEz3JwPj1gKV1pXP7EfccDdinkmGwFeFZ1tWGEk3c
    // 9fd27f480268020f4c59f8ebc059dad7de0120ce
    // c9798284ae72e973cd7a0ebaf7adb146
    // 23af490d33a9d732f48811ab21f2bfad4405b40d3ced353dbaf7f0940a5e4e1b
    // RETURN file path
    // TODO по частичному хешу
    if (argc == 4 && strcmp(argv[1], "fhtp") == 0)
    {
        string containerName(argv[2]);
        string hash(argv[3]);
        sqlite3* db = sqliteOpen(korniPathOfDatabase(containerName));

        Record r = sqliteSelectFileByHash(db, hash);
        string korniFileId;
        for (auto f : r)
        {
            if (f.first == "id")
                korniFileId = get<1>(f.second);
        }
        cerr << korniFileId << endl;
        string path = korniGetFilePathInDisk(containerName, korniFileId);
        if (path.length() > 0)
        {
            cout << path << endl;
            exit(EXIT_SUCCESS);
        }
        else
        {
            cerr << "not found";
            exit(EXIT_FAILURE);
        }

        //        boost::property_tree::ptree jsontree;
        //        for (auto f : r)
        //        {
        //            // TODO if else if if else if .... type
        //            jsontree.put<string>(f.first, get<1>(f.second));
        //        }
        //        stringstream ss;
        //        ss.str("");
        //        ss.clear();
        //        boost::property_tree::json_parser::write_json(ss, jsontree, false);
        //        string jsonForSign = ss.str();
        //        jsonForSign.replace(jsonForSign.find_last_of("\n"), 1, "");
        //        cout << jsonForSign << endl;
        //        if (r.size() > 0)
        //            exit(EXIT_SUCCESS);
        //        else
        //            exit(EXIT_FAILURE);
    }

    //    // TODO remove
    //    if (/*argc == 3 && */ strcmp(argv[1], "test") == 0)
    //    {
    //        sqlite3* db = sqliteOpen(korniPathOfDatabase("test"));
    //        sqliteInsertRecordSet(db, "test2",
    //                              RecordSet{
    //                                  Record{ { string("id"), make_tuple('i', string("100")) },
    //                                          { string("zT"), make_tuple('i', string("101")) },
    //                                          { string("f1"), make_tuple('t', string("f1-value")) } },
    //                                  Record{
    //                                      { string("id"), make_tuple('i', string("202")) },
    //                                      { string("zT"), make_tuple('i', string("303")) },
    //                                      { string("f2"), make_tuple('t', string("f2-value2")) },
    //                                      { string("f1"), make_tuple('t', string("f1-value3")) } } },
    //                              true);
    //        exit(0);
    //    }

    // print help
    {
        cout << "# genkey <nickname> [<bits; 4096 by default>]" << endl;
        cout << "$ korni3 genkey demo" << endl;
        cout << "                                                       generate keypairs" << endl;
        cout << "" << endl;
        cout << "# print to stdout keys nicknames" << endl;
        cout << "$ korni3 keys" << endl;
        cout << "testUser" << endl;
        cout << "demo" << endl;
        cout << "                                                           list keypairs" << endl;
        cout << "" << endl;
        cout << "# korni3 login <nickname>" << endl;
        cout << "$ korni3 login demo" << endl;
        cout << "                                           use one of keypair as current" << endl;
        cout << "" << endl;
        cout << "# create|insert <dbName> <tableName> [<jsonRecord>]" << endl;
        cout << "# create|insert <dbName> <tableName> # json read from stdin" << endl;
        cout << "$ korni3 insert test testTable '{\"id\":1, \"name\":\"korni demo tutorial\"}'"
             << endl;
        cout << "                                     " << endl;
        cout << "                                     " << endl;
        cout << "$ korni3 rows test testTable --table -c id -c zT -c name --where 'id=1'" << endl;
        cout << "  id          zT  name" << endl;
        cout << "----  ----------  ---------------------" << endl;
        cout << "   1  1664036153  korni demo tutorial" << endl;
        cout << "                                          list table rows with condition" << endl;
        cout << "$ korni3 repack <dbName> <filter>" << endl;
        cout << "                  repack offten table change chansets for compactisation" << endl;
        cout << "" << endl;
        cout << "# korni3 file <dbName> <fileOrDirPath> [--ignore] [--rm] [--empty] [--lns]"
             << endl;
        cout << "$ korni3 file test /home/mp/Qt/QtCreator-v8.0.1/bin/qt.conf" << endl;
        cout << "$ korni3 file test /home/mp/Qt/QtCreator-v8.0.1/bin/qt.conf 2>/dev/null " << endl;
        cout << "                                  add file or dir to data container test" << endl;
        cout << "                            it print to stderr and stdout in diff format" << endl;
        cout << "" << endl;
        cout << "$ korni3 insert-files images *.gif" << endl;
        cout << "                        another way to store files - in table change set" << endl;
        cout << "" << endl;
        cout << "# setgeo <lat like 58.05139>,<lat like 38.83802>" << endl;
        cout << "# setgeo <lon like 58.05139> <lon like 38.83802>" << endl;
        cout << "$ korni3 58.05139 38.83802" << endl;
        cout << "                                   set current user current/new geoplace" << endl;
        cout << "" << endl;
        cout << "# scan <containerName> [<filter option> см. documentation]" << endl;
        cout << "$ korni3 scan test # [options]" << endl;
        cout << "                                просканировать журнал, чтобы обновить БД" << endl;
        cout << "                фильтрация изменений по дате, гео позиции, пользователям" << endl;
        cout << "    --date <2022-09-29> : newer the date" << endl;
        cout << "    --geo-r <10000> : in circle with radius 10000 m around current place" << endl;
        cout << "    --u <u pubkey> --u <u pubkey> : scan updates only for this user" << endl;
        cout << "        " << endl;
        cout << "# db <containerName>" << endl;
        cout << "$ sqlite3  `korni3 db test` '.tables' " << endl;
        cout << "        return to stdout path to database sqlite file for use in scripts" << endl;
    }
}

int test()
{
    // TODO now its sha256 hash
    string userId{ "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDOrZEaOJqsWOPryE1H3dJhqaAG" };
    //    korniLogin(userId);

    // test OK
    //    string baseText = "misha pavlov";
    //    string singature = korniSignText(baseText);
    //    //    cout << "DEBUG signature" << endl << singature << endl;
    //    cout << "test check signature " << (korniVerifySign(baseText, singature) ? "OK" : "FAIL")
    //         << endl;

    //    string thisProprammPath = argv[0];
    //    cout << argc << argv[1] << "|" << argv[2] << endl;

    // test OK
    //    string outGH = "---";
    //    cerr << (bool) getCurrentPlace(outGH) << "  " << outGH << endl;

    // test
    //    cerr << korniPathForTableChanges(korniUserId(), "test", "node") << endl;

    // test
    //    korniCreateRecord("test", "node",
    //                      "{\"id\":\"uuid-text\", \"name\": \"misha\", \"fam\":\"pavlov\"}");

    //    // test sign2
    //    string text = "2022.09.16\n";
    //    auto sign = korniSignText2(text);
    //    cout << korniVerifySign2(text, sign) << endl;
    //    cout << korniVerifySign2(text, sign, korniUserId()) << endl;

    //    // test
    //    cout << korniGetFilePathInDisk(
    //                "test",
    //                "GFS-QmSyr7J3qTb3iWtRu8pWmuuw1qSVMRs63PGnyHxgsHpa3U-T-"
    //                "3b44512a3a990bc9d23123636c02b44dd1aa07dc-M-c66d7fc0fe68fe63e1d42c231804fa00-S256-"
    //                "21c76a1b4f5447ee464bab983c804f75d57c6c2a3db776a38a8ce1d53dfb54c4")
    //         << endl;

    //    //test
    //    korniRepackGzTables("test");

    ////test
    //    korniFilterChangeSet(
    //        "test",
    //        [](const string& tablename, const string& path) {
    //            cout << table << "\t" << path << endl;
    //        },
    //        [](const string& table) {
    //            cout << table << endl;
    //        },
    //        "", vector<string>{}, "", 0);
    //// date ok
    //    korniFilterChangeSet(
    //        "test",
    //        [](const string& tablename, const string& path) {
    //            cout << tablename << "\t" << path << endl;
    //        },
    //        [](const string& table) {
    //            cout << table << endl;
    //        },
    //        "2022-09-22", vector<string>{}, "", 0);
    //// geo 1 fail
    //    korniFilterChangeSet(
    //        "test",
    //        [](const string& tablename, const string& path) {
    //            cout << tablename << "\t" << path << endl;
    //        },
    //        [](const string& table) {
    //            cout << table << endl;
    //        },
    //        "", vector<string>{}, "uf7d60uqvu2h", 10);
    //// geo 2 ok
    //    korniFilterChangeSet(
    //        "test",
    //        [](const string& tablename, const string& path) {
    //            cout << tablename << "\t" << path << endl;
    //        },
    //        [](const string& table) {
    //            cout << table << endl;
    //        },
    //        "", vector<string>{}, "uf7d60uqvu2h", 10000);

    // test
    //    korniScanTables("test");

    // test sqlte3
    //sqliteInsertRecords(korniPathOfDatabase("test").c_str());

    return 0;
}

int main(int argc, char** argv)
{
    ::pw = getpwuid(getuid());
    ::KORNIEXEPATH = argv[0];
    //    if (signal(SIGINT, &sighandler) == SIG_ERR)
    //    {
    //        fprintf(stderr, "Could not set signal handler\n");
    //        return EXIT_FAILURE;
    //    }
    parseCommandLine(argc, argv);
    exit(EXIT_SUCCESS);

    test();
    return 0;
}
