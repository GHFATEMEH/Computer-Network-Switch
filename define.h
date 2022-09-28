#ifndef DEFINE_H
#define DEFINE_H

#include <vector>
#include <string>

#define SIZE 1024
#define SIZE_NAMED_PIPE 5000
#define MAX_DATA_SIZE 1500
#define RECEIVE_DATA -1

struct Frame
{
    int destination;
    int source;
    int num;
    int numberOfFrame;
    std::string fileName;
    int dataSize;
    std::vector<char> data;
};

struct ReadInfoFromUnnamedPipe
{
    int valread;
    char* buf;
};

struct ReadInfoFromNamedPipe
{
    int valread;
    Frame frame;
};


#endif