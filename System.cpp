#include "System.h"

using namespace std;

System::System(int _systemNumber, int _pipeReadEnd, int _pipeWriteEnd){
    systemNumber = _systemNumber;
    pipeReadEnd = _pipeReadEnd;
    pipeWriteEnd = _pipeWriteEnd;
    cerr << "New system make\n";
    cerr << "systemNumber: " << systemNumber << "\n";
    cerr << "pipeReadEnd: " << pipeReadEnd << "\n";
    cerr << "pipeWriteEnd: " << pipeWriteEnd << "\n";
}

int System::getWriteEnd()
{
    return pipeWriteEnd;
}

int System::getReadEnd()
{
    return pipeReadEnd;
}

char* System::getNamedPipeSwitch()
{
    return convertStringToCharArr(namedPipeSwitch);
}

void System::handleCommandConnect(vector<string> command)
{
    switchNumberConnected = stoi(command[2]);
    namedPipeSystem = command[1];
    namedPipeSwitch = command[3];
    cerr << "switchNumberConnected: " << switchNumberConnected << "\n";
    cerr << "namedPipeSystem: " << namedPipeSystem << "\n";
    cerr << "namedPipeSwitch: " << namedPipeSwitch << "\n";
}

vector<char> System::readFromFile(string fileName)
{
    ifstream file(fileName);
    ostringstream ss;
    ss << file.rdbuf();
    const string& s = ss.str();
    vector<char> vec(s.begin(), s.end());
    return vec;
}

Frame System::makeSingleFrame(vector<string> command, int num, vector<char> dataChunk, int numOfNeededFrames)
{
    Frame frame;
    frame.destination = stoi(command[1]);
    frame.source = systemNumber;
    frame.num = num;
    frame.dataSize = dataChunk.size();
    frame.data = dataChunk;
    frame.numberOfFrame = numOfNeededFrames;
    frame.fileName = command[2];
    return frame;
}

vector<Frame> System::makeFrames(vector<string> command)
{
    vector<Frame> frames;
    string fileName = command[2];
    vector<char> fileContent = readFromFile(fileName);
    int numOfNeededFrames = (fileContent.size() + MAX_DATA_SIZE - 1) / MAX_DATA_SIZE;
    for(int i = 0; i < numOfNeededFrames; i++)
    {
        int start = 0 + MAX_DATA_SIZE * i;
        int end = 0;
        int dataSize;
        if(i != numOfNeededFrames - 1)
        {
            end += (i + 1) * MAX_DATA_SIZE;
            dataSize = MAX_DATA_SIZE;
        }
        else
        {
            end += i * MAX_DATA_SIZE + fileContent.size() % MAX_DATA_SIZE;
            dataSize = fileContent.size() % MAX_DATA_SIZE;
        }
        vector<char> dataChunk;
        for(int i = start; i < end; i++)
            dataChunk.push_back(fileContent[i]);
        frames.push_back(makeSingleFrame(command, i, dataChunk, numOfNeededFrames));
    }
    return frames;
}

void System::handleCommandSend(vector<string> command)
{
    vector<Frame> frames = makeFrames(command);
    cerr << "size" << frames.size() << endl;
    cerr << "destination" << frames[0].destination << endl;
    cerr << "source" << frames[0].source << endl;
    cerr << "num" << frames[0].num << endl;
    cerr << "dataSize" << frames[0].dataSize << endl;
    cerr << "numberOfFrame" << frames[0].numberOfFrame << endl;
    cerr << "fileName" << frames[0].fileName << endl;
    for(int i = 0; i < frames.size(); i++)
        writeInNamedPipe(convertStringToCharArr(namedPipeSystem), frames[i]);
}

void System::handleCommandReceive(vector<string> command)
{
    vector<char> v(command[2].begin(), command[2].end());
    Frame frame = makeSingleFrame(command, RECEIVE_DATA, v, 1);
    writeInNamedPipe(convertStringToCharArr(namedPipeSystem), frame);
}

void System::handleCommand(char* buf)
{
    vector<string> command = split(string(buf), ' ');
    if(command[0] == "connect")
        handleCommandConnect(command);
    else if(command[0] == "send")
        handleCommandSend(command);
    else
        handleCommandReceive(command);
}

void System::setNamePipeFd(int fd)
{
    namedPipeSwitchFd = fd;
}

bool compareFrames(Frame frame1, Frame frame2)
{
    return (frame1.num < frame2.num);
}

void System::margeFramesData(vector<Frame> frames)
{
    sort(frames.begin(), frames.end(), compareFrames);
    for(int i = 0; i < frames.size(); i++)
        writeDataInFile(frames[i].fileName, frames[i].data);
}

void System::handleFrame(Frame inputFrame)
{
    if(inputFrame.destination != systemNumber)
        return;
    if(inputFrame.num == RECEIVE_DATA){
        vector<string>command;
        command.push_back("send");
        command.push_back(to_string(inputFrame.source));
        string str(inputFrame.data.begin(), inputFrame.data.end());
        command.push_back(str);
        handleCommandSend(command);
    }
    else{
        cerr << "destination" << inputFrame.destination << endl;
        cerr << "source" << inputFrame.source << endl;
        cerr << "num" << inputFrame.num << endl;
        cerr << "dataSize" << inputFrame.dataSize << endl;
        cerr << "numberOfFrame" << inputFrame.numberOfFrame << endl;
        cerr << "fileName" << inputFrame.fileName << endl;
        int numOfNeededFrames = inputFrame.numberOfFrame - 1;
        int source = inputFrame.source;
        string fileName = inputFrame.fileName;
        vector<Frame> frames;
        frames.push_back(inputFrame);
        while(numOfNeededFrames)
        {
            ReadInfoFromNamedPipe readInfoFromN = readFromNamedpipe(namedPipeSwitchFd);
            if(readInfoFromN.frame.source == source && readInfoFromN.frame.fileName == fileName)
            {
                numOfNeededFrames--;
                frames.push_back(readInfoFromN.frame);
            }
            else
                handleFrame(readInfoFromN.frame);
        }

        margeFramesData(frames);
    }
    
}

int main(int argc, char** argv){
    System* system = new System(atoi(argv[3]), atoi(argv[1]), atoi(argv[2]));
    fd_set readfd;
    int maxFd;
    while(1){
        FD_ZERO(&readfd);
        FD_SET(system->getReadEnd(), &readfd);
        int namedPipe = open(convertStringToCharArr(system->getNamedPipeSwitch()), O_RDONLY | O_NONBLOCK);
        system->setNamePipeFd(namedPipe);
        FD_SET(namedPipe, &readfd);
        maxFd = max(system->getReadEnd(), namedPipe);

        select(maxFd + 1, &readfd, NULL, NULL, NULL);

        if(FD_ISSET(system->getReadEnd(), &readfd)){
            ReadInfoFromUnnamedPipe readInfoFromUn = readFromUnnamedPipe(system->getWriteEnd(), system->getReadEnd());
            if(readInfoFromUn.valread > 0)
                system->handleCommand(readInfoFromUn.buf);
        }
        
        if(FD_ISSET(namedPipe, &readfd)){
            ReadInfoFromNamedPipe readInfoFromN = readFromNamedpipe(namedPipe);
            if(readInfoFromN.valread > 0)
            {
                system->handleFrame(readInfoFromN.frame);
                close(namedPipe);
            }
        }
    }
}