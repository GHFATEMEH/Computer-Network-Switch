#include <signal.h>
#include "Switch.h"

using namespace std;

Switch::Switch(int _numberOfAvailablePorts, int _switchNumber, int _pipeWriteEnd, int _pipeReadEnd){
    numberOfAvailablePorts = _numberOfAvailablePorts;
    switchNumber = _switchNumber;
    pipeReadEnd = _pipeReadEnd;
    pipeWriteEnd = _pipeWriteEnd;
    cerr << "New switch make\n";
    cerr << "numberOfAvailablePorts: " << numberOfAvailablePorts << "\n";
    cerr << "switchNumber: " << switchNumber << "\n";
    cerr << "pipeReadEnd: " << pipeReadEnd << "\n";
    cerr << "pipeWriteEnd: " << pipeWriteEnd << "\n";
}

void Switch::handleCommandConnect(vector<string> command)
{
    if(numberOfAvailablePorts > 0)
    {
        namedPipes[command[1]] = command[3];
        lookupTable[stoi(command[2])] = command[3];
        numberOfAvailablePorts--;
        cerr << "connect in switch" << "\n";
        cerr << "namedPipes " << command[1] << "\n";
        cerr << "lookupTable " << command[2] << "\n";
        cerr << "numberOfAvailablePorts " << numberOfAvailablePorts << "\n";
    }
    else
        cout << "connection failed!" << endl;
}

void Switch::handleCommandConnectToSwitch(vector<string> command)
{
    if(numberOfAvailablePorts > 0){
        namedPipes[command[2]] = command[1];
        numberOfAvailablePorts--;
    }
    else
        cout << "connection switch to switch failed!" << endl;
}

void Switch::handleCommand(char* buf)
{
    vector<string> command = split(string(buf), ' ');
    if(command[0] == "connect")
        handleCommandConnect(command);
    else
        handleCommandConnectToSwitch(command);
}

std::map<std::string, std::string> Switch::getNamedPipes()
{
    return namedPipes;
}

void Switch::handleFrame(Frame inputFrame, string namedPipeSwitch)
{
    lookupTable[inputFrame.source] = namedPipeSwitch;
    if(lookupTable.find(inputFrame.destination) == lookupTable.end()){
        map<string, string>::iterator it;
        for (it = namedPipes.begin(); it != namedPipes.end(); it++)
            if(it->second != namedPipeSwitch)
                writeInNamedPipe(convertStringToCharArr(it->second), inputFrame);
    }
    else
    {
        cerr << "inputFrame.destination: " << inputFrame.destination << endl;
        cerr << "lookupTable[inputFrame.destination]: " << lookupTable[inputFrame.destination] << endl;
        cerr << "inputFrame.Namefile: " << inputFrame.fileName << endl;
        writeInNamedPipe(convertStringToCharArr(lookupTable[inputFrame.destination]), inputFrame);
    }
}

int Switch::getPipeReadEnd(){
    return pipeReadEnd;
}
       

int main(int argc, char** argv){
    Switch* mySwitch = new Switch(atoi(argv[3]), atoi(argv[4]), atoi(argv[2]), atoi(argv[1]));
    fd_set readfd;
    int maxFd;
    map<string, int> namedPipeFds;
    while(1){
        FD_ZERO(&readfd);
        FD_SET(mySwitch->getPipeReadEnd(), &readfd);
        maxFd = mySwitch->getPipeReadEnd();
        map<string, string> namedPipes = mySwitch->getNamedPipes();
        map<string, string>::iterator it;
        for (it = namedPipes.begin(); it != namedPipes.end(); it++)
        {
            int namedPipe = open(convertStringToCharArr(it->first), O_RDONLY | O_NONBLOCK);
            namedPipeFds[it->first] = namedPipe;
            FD_SET(namedPipe, &readfd);
            maxFd = max(maxFd, namedPipe);
        }

        select(maxFd + 1, &readfd, NULL, NULL, NULL);

        if(FD_ISSET(mySwitch->getPipeReadEnd(), &readfd)){
            ReadInfoFromUnnamedPipe readInfoFromUn = readFromUnnamedPipe(atoi(argv[2]), atoi(argv[1]));
            if(readInfoFromUn.valread > 0){
                mySwitch->handleCommand(readInfoFromUn.buf);
            }
        }

        for (it = namedPipes.begin(); it != namedPipes.end(); it++)
        {
            if(FD_ISSET(namedPipeFds[it->first], &readfd)){
                ReadInfoFromNamedPipe readInfoFromN = readFromNamedpipe(namedPipeFds[it->first]);
                if(readInfoFromN.valread > 0)
                    mySwitch->handleFrame(readInfoFromN.frame, it->second);
            }
        }
    }
}