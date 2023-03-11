#include <string>
#include <functional>
using namespace std;

// OK
string filesystemReadFileIntoString(const string& path);

// OK
void filesystemWriteStringIntoFile(const string& filename, const string data, bool isAppend=false);

void filesystemRemoveFile(const string& path);

void filesystemSymLink(const string& path, const string& linkTarget);

void filesystemTrancateFile(const string& path);

// OK
void filesystemShureDirecroryExists(const std::string& s);

// OK
bool filesystemIsPathExist(const std::string& path, struct stat* optionalResultInfo = nullptr);

// OK
string filesystemFindAlternative(const string& dirPath, bool withDirs = false, function<void(const string& path)> oneFileCalback = [](const string&){});

// OK
// 1 file; 2 dir; 3  other type; -1 error;
int filesystemPathFileOrDirOrOther(const char* path);
