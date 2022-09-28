#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include "define.h"

std::vector<std::string> split(std::string line, char delimiter);
char* convertStringToCharArr(std::string str);
char** makeArgv(char* input, int unnamedPipe[], std::vector<std::string> splittedInput);
void writeInUnnamedPipe(char* toBeWriten, int unnamedPipe[]);
ReadInfoFromUnnamedPipe readFromUnnamedPipe(int writeEnd, int readEnd);
ReadInfoFromNamedPipe readFromNamedpipe(int namedPipe);
void writeInNamedPipe(const char* namedPipeName, Frame frame);
void writeDataInFile(std::string fileName, std::vector<char> data);
std::string convertFrameToString(Frame frame);
Frame convertCharArrToFrame(char* input);
std::string convertCharArrToString(std::vector<char> v);

#endif