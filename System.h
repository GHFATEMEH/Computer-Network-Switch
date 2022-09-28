#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <fstream>
#include <signal.h>
#include <sstream>
#include <vector>
#include <iostream>
#include "functions.h"

class System{
    public:
        System(int _systemNumber, int _pipeReadEnd, int _pipeWriteEnd);
        int getWriteEnd();
        int getReadEnd();
        char* getNamedPipeSwitch();
        void handleCommand(char* buf);
        void handleFrame(Frame inputFrame);
        void setNamePipeFd(int fd);
    private:
        int systemNumber;
        int switchNumberConnected;
        int pipeReadEnd;
        int pipeWriteEnd;
        std::string namedPipeSystem;
        std::string namedPipeSwitch;
        int namedPipeSwitchFd;
        void handleCommandConnect(std::vector<std::string> command);
        void handleCommandSend(std::vector<std::string> command);
        void handleCommandReceive(std::vector<std::string> command);
        std::vector<Frame> makeFrames(std::vector<std::string> command);
        std::vector<char> readFromFile(std::string fileName);
        Frame makeSingleFrame(std::vector<std::string> command, int num, std::vector<char> dataChunk, int numOfNeededFrames);
        void margeFramesData(std::vector<Frame> frames);

};

#endif