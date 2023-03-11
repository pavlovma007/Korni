#include <string>
#include <functional>

// OK
void /*std::string */ execCommand(const std::string cmd, int& out_exitStatus, std::function<void(char*, int)> handledata = [](char*, int){});


// int exitStatus = 0;
// auto result = execCommand("grep --version", exitStatus);

class Command
{
public:
    int ExitStatus = 0;
    std::string command;
    std::string stdIn;
    std::string stdOut;
    std::string stdErr;

    void execute(const std::function<void()>& callbackBeforeWaitPid = ([]() {}));
    
    // OK
    //void test_Command()
    //{
    //    Command cmd;
    //    cmd.command = "grep diam - ";
    //    cmd.stdIn = R"(
    //     Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy
    //     eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam
    //     voluptua.
    //    )";
    //    cmd.execute();
    //    std::cout << "grep diam -  // stdout=" << cmd.stdOut;
    //}
};
