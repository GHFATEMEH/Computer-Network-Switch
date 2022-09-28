#ifndef SWITCH_H
#define SWITCH_H

#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <sys/time.h>
#include <utility>
#include "functions.h"

class Switch{
    public:
        Switch(int _numberOfAvailablePorts, int _switchNumber, int _pipeWriteEnd, int _pipeReadEnd);
        void handleCommand(char* buf);
        std::map<std::string, std::string> getNamedPipes();
        void handleFrame(Frame inputFrame, std::string namedPipe);
        int getPipeReadEnd();
    private:
        int numberOfAvailablePorts;
        int switchNumber;
        int pipeReadEnd;
        int pipeWriteEnd;
        std::map<std::string, std::string> namedPipes;
        std::map<int, std::string> lookupTable;
        void handleCommandConnect(std::vector<std::string> command);
        void handleCommandConnectToSwitch(std::vector<std::string> command);
};

#endif

//namedPipes
// system switch

//lookup
//lookup switch