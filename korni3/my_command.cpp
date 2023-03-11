#include "my_command.h"

#include <cstring>
#include <iostream> // stdin stdout stderr open close
#include <sys/wait.h>
#include <unistd.h>

// OK
void /*std::string */ execCommand(const std::string cmd, int& out_exitStatus,
                                  std::function<void(char*, int)> handledata)
{
    out_exitStatus = 0;
    auto pPipe = ::popen(cmd.c_str(), "r");
    if (pPipe == nullptr)
    {
        throw std::runtime_error("Cannot open pipe");
    }

    std::array<char, 256> buffer;

    //    std::string result;

    while (not std::feof(pPipe))
    {
        auto bytes = std::fread(buffer.data(), 1, buffer.size(), pPipe);
        //        result.append(buffer.data(), bytes);
        handledata(buffer.data(), bytes);
    }

    auto rc = ::pclose(pPipe);

    if (WIFEXITED(rc))
    {
        out_exitStatus = WEXITSTATUS(rc);
    }

    //    return result;
}
// int exitStatus = 0;
// auto result = execCommand("grep --version", exitStatus);

void Command::execute(const std::function<void()>& callbackBeforeWaitPid)
{
    const int READ_END = 0;
    const int WRITE_END = 1;

    int infd[2] = { 0, 0 };
    int outfd[2] = { 0, 0 };
    int errfd[2] = { 0, 0 };

    auto cleanup = [&]() {
        ::close(infd[READ_END]);
        ::close(infd[WRITE_END]);

        ::close(outfd[READ_END]);
        ::close(outfd[WRITE_END]);

        ::close(errfd[READ_END]);
        ::close(errfd[WRITE_END]);
    };

    auto rc = ::pipe(infd);
    if (rc < 0)
    {
        throw std::runtime_error(std::strerror(errno));
    }

    rc = ::pipe(outfd);
    if (rc < 0)
    {
        ::close(infd[READ_END]);
        ::close(infd[WRITE_END]);
        throw std::runtime_error(std::strerror(errno));
    }

    rc = ::pipe(errfd);
    if (rc < 0)
    {
        ::close(infd[READ_END]);
        ::close(infd[WRITE_END]);

        ::close(outfd[READ_END]);
        ::close(outfd[WRITE_END]);
        throw std::runtime_error(std::strerror(errno));
    }

    auto pid = fork();
    if (pid > 0) // PARENT
    {
        ::close(infd[READ_END]);   // Parent does not read from stdin
        ::close(outfd[WRITE_END]); // Parent does not write to stdout
        ::close(errfd[WRITE_END]); // Parent does not write to stderr

        if (::write(infd[WRITE_END], stdIn.data(), stdIn.size()) < 0)
        {
            throw std::runtime_error(std::strerror(errno));
        }
        ::close(infd[WRITE_END]); // Done writing
    }
    else if (pid == 0) // CHILD
    {
        ::dup2(infd[READ_END], STDIN_FILENO);
        ::dup2(outfd[WRITE_END], STDOUT_FILENO);
        ::dup2(errfd[WRITE_END], STDERR_FILENO);

        ::close(infd[WRITE_END]); // Child does not write to stdin
        ::close(outfd[READ_END]); // Child does not read from stdout
        ::close(errfd[READ_END]); // Child does not read from stderr

        ::execl("/bin/bash", "bash", "-c", command.c_str(), nullptr);
        ::exit(EXIT_SUCCESS);
    }

    // PARENT
    if (pid < 0)
    {
        cleanup();
        throw std::runtime_error("Failed to fork");
    }

    callbackBeforeWaitPid();

    int status = 0;
    ::waitpid(pid, &status, 0);

    std::array<char, 256> buffer;

    ssize_t bytes = 0;
    do
    {
        bytes = ::read(outfd[READ_END], buffer.data(), buffer.size());
        stdOut.append(buffer.data(), bytes);
    } while (bytes > 0);

    do
    {
        bytes = ::read(errfd[READ_END], buffer.data(), buffer.size());
        stdErr.append(buffer.data(), bytes);
    } while (bytes > 0);

    if (WIFEXITED(status))
    {
        ExitStatus = WEXITSTATUS(status);
    }

    cleanup();
}
