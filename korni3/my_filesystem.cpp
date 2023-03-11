#include "my_filesystem.h"
#include "my_command.h"
#include "tinydir.h" // https://github.com/cxong/tinydir

#include <fcntl.h> // O_TRUNC
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h> // readlink

// OK
string filesystemReadFileIntoString(const string& path)
{
    ifstream input_file(path);
    if (!input_file.is_open())
    {
        cerr << "Could not open the file - '" << path << "'" << endl;
        exit(EXIT_FAILURE);
    }
    return string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

// OK
void filesystemWriteStringIntoFile(const string& filename, const string data, bool isAppend)
{
    fstream file_out;

    file_out.open(filename, isAppend ? std::ios_base::app : std::ios_base::out);
    if (!file_out.is_open())
    {
        cerr << "ERROR: failed to open " << filename << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        file_out << data << endl;
        //        cout << "Done Writing!" << endl;
    }
}

void filesystemRemoveFile(const string& path)
{
    const int result = remove(path.c_str());
    if (result != 0)
        std::cerr << "file renove error N=" << result << "err str=" << strerror(errno)
                  << " for file=" << path << endl;
    //    else
    //    {
    //        printf("success\n");
    //    };

    //    remove is in the stdio.h file
    //    and strerror is in the string.h
    //    So after your remove function, check to see for what reason it has not been deleted.
    //    The error number is stored in errno variable and strerror can map the error number to a string that tells the reason of failure.
}

void filesystemSymLink(const string& path, const string& linkTarget)
{
    const int result = symlink(linkTarget.c_str(), path.c_str());
    if (result != 0)
        std::cerr << "file ln error N=" << result << "err str=" << strerror(errno)
                  << " for file=" << path << endl;
    //    else
    //    {
    //        printf("success\n");
    //    };

    //    remove is in the stdio.h file
    //    and strerror is in the string.h
    //    So after your remove function, check to see for what reason it has not been deleted.
    //    The error number is stored in errno variable and strerror can map the error number to a string that tells the reason of failure.
}

void filesystemTrancateFile(const string& path)
{
    const int fd = open(path.c_str(), O_TRUNC | O_WRONLY);
    close(fd);
    if (!fd)
        cerr << "ERROR trancate file = " << path << endl;
}

// OK
void filesystemShureDirecroryExists(const string& path)
{
    int pos = path.find_last_of("/");
    string dir = path.substr(0, pos + 1);
    //    cout << dir << endl;
    Command cmd;
    cmd.command = "mkdir -p '" + dir + "'";
    cmd.execute();
    //    cout << cmd.ExitStatus << cmd.stdErr << endl;
}

// OK
string filesystemFindAlternative(const string& dirPath, bool withDirs,
                                 function<void(const string& path)> oneFileCalback)
{
    tinydir_dir dir;
    tinydir_open(&dir, dirPath.c_str());

    stringstream ss;

    while (dir.has_next)
    {
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        if (string(file.name) != "." && string(file.name) != ".." && string(file.name) != "")
        {
            if (file.is_dir)
            {
                if (withDirs)
                    ss << file.path << "/" << endl;
                ss << filesystemFindAlternative(file.path);
            }
            else
            {
                ss << file.path << endl;
                oneFileCalback(string(file.path));
            }
        }
        tinydir_next(&dir);
    }
    tinydir_close(&dir);

    return ss.str();
}

// OK
bool filesystemIsPathExist(const std::string& path, struct stat* optionalResultInfo)
{
    struct stat buffer
    {
    };

    int res = stat(path.c_str(), &buffer);
    if (optionalResultInfo != nullptr)
        *optionalResultInfo = buffer;
    return (!res);
}

// OK
// 1 file; 2 dir; 3  other type; -1 error;
int filesystemPathFileOrDirOrOther(const char* path)
{
    struct stat s;
    if (stat(path, &s) == 0)
    {
        if (s.st_mode & S_IFDIR)
        {
            //it's a directory
            return 2;
        }
        else if (s.st_mode & S_IFREG)
        {
            //it's a file
            return 1;
        }
        else
        {
            //something else
            return 3;
        }
    }
    else
    {
        //error
        return -1;
    }
}
