#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <sys/stat.h>
#include "System.h"
#include "Switch.h"
#include "functions.h"


using namespace std;

map<int, int[2]> switchesUnNamedPipe;
map<int, int[2]> systemsUnNamedPipe;
int numOfNamedPipe = 0;

void handleConnectCommand(vector<string> input)
{
    numOfNamedPipe++;
    //for writing system in namedpipe
    string namedPipeSystem = "namedPipeSystem" + to_string(numOfNamedPipe);
    const char* newNamedPipeSystem = namedPipeSystem.c_str();
    mkfifo(newNamedPipeSystem, 0666);

    //for writing switch in namedpipe
    string namedPipeSwitch = "namedPipeSwitch" + to_string(numOfNamedPipe);
    const char* newNamedPipeSwitch = namedPipeSwitch.c_str();
    mkfifo(newNamedPipeSwitch, 0666);

    char* toBeWritenSystem = convertStringToCharArr(string("connect") + " " + newNamedPipeSystem + " " + input[2] + " " + newNamedPipeSwitch);
    char* toBeWritenSwitch = convertStringToCharArr(string("connect") + " " + newNamedPipeSystem + " " + input[1] + " " + newNamedPipeSwitch);
    
    writeInUnnamedPipe(toBeWritenSystem, systemsUnNamedPipe[stoi(input[1])]);
    writeInUnnamedPipe(toBeWritenSwitch, switchesUnNamedPipe[stoi(input[2])]);
}

void handleConnectSwitchToSwitch(vector<string> input)
{
    numOfNamedPipe++;
    string namedPipeSwitch1 = "namedPipeSwitch1" + to_string(numOfNamedPipe);
    const char* newNamedPipeSwitch1 = namedPipeSwitch1.c_str();
    mkfifo(newNamedPipeSwitch1, 0666);

    string namedPipeSwitch2 = "namedPipeSwitch2" + to_string(numOfNamedPipe);
    const char* newNamedPipeSwitch2 = namedPipeSwitch2.c_str();
    mkfifo(newNamedPipeSwitch2, 0666);
    
    char* toBeWritenSwitch1 = convertStringToCharArr(string("connect_switch") + " " + newNamedPipeSwitch1 + " " + newNamedPipeSwitch2);
    char* toBeWritenSwitch2 = convertStringToCharArr(string("connect_switch") + " " + newNamedPipeSwitch2 + " " + newNamedPipeSwitch1);

    writeInUnnamedPipe(toBeWritenSwitch1, switchesUnNamedPipe[stoi(input[1])]);
    writeInUnnamedPipe(toBeWritenSwitch2, switchesUnNamedPipe[stoi(input[2])]);
}

void handleSendCommand(vector<string> input)
{
    //send input format: send systemNumber destinationNumber fileName
    char* toBeWritenSystem = convertStringToCharArr(string("send") + " " + input[2] + " " + input[3]);
    writeInUnnamedPipe(toBeWritenSystem, systemsUnNamedPipe[stoi(input[1])]);
}

void handleReceiveCommand(vector<string> input)
{
    //receive input format: receive systemNumber destinationNumber fileName
    char* toBeWritenSystem = convertStringToCharArr(string("receive") + " " + input[2] + " " + input[3]);
    writeInUnnamedPipe(toBeWritenSystem, systemsUnNamedPipe[stoi(input[1])]);
}

void runCommand(vector<string> input)
{
    if(input[0] == "Connect")
        handleConnectCommand(input);
    //Connect_To_Switch sourceSwitch destinationSwitch
    else if(input[0] == "Connect_Switch_To_Switch")
        handleConnectSwitchToSwitch(input);
    else if(input[0] == "send")
        handleSendCommand(input);
    else if(input[0] == "receive")
        handleReceiveCommand(input);
}

int main(){
    string input;

    while(getline(cin, input))
    {
        char** argv;
        vector<string> splittedInput = split(input, ' ');
        if(splittedInput[0] == "MySwitch" or splittedInput[0] == "MySystem"){
            int unnamedPipe[2];
		    pipe(unnamedPipe);
            int pid = fork();
            
            if (pid < 0){
                cout << "fork failed" << "\n";
            }
            else if(pid == 0){
                if(splittedInput[0] == "MySwitch")
                    argv = makeArgv(convertStringToCharArr("./Switch.out"), unnamedPipe, splittedInput);
                else
                    argv = makeArgv(convertStringToCharArr("./System.out"), unnamedPipe, splittedInput);
                execv(argv[0], argv);
            }
            else if (pid > 0){
                if(splittedInput[0] == "MySwitch"){
                    switchesUnNamedPipe[stoi(splittedInput[2])][0] = unnamedPipe[0];
                    switchesUnNamedPipe[stoi(splittedInput[2])][1] = unnamedPipe[1];
                }
                else{
                    systemsUnNamedPipe[stoi(splittedInput[1])][0] = unnamedPipe[0];
                    systemsUnNamedPipe[stoi(splittedInput[1])][1] = unnamedPipe[1];
                }
            }
        }
        else
            runCommand(splittedInput);
    }
}