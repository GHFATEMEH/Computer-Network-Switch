#include "functions.h"

using namespace std;

vector<string> split(string line, char delimiter){
    stringstream stream(line);
    vector<string> outList;
    string block;
    while(getline(stream, block, delimiter)){
        outList.push_back(block);
    }
    return  outList;
}

char* convertStringToCharArr(string str)
{
	char* charArr = new char[str.length() + 1];
	strcpy(charArr, str.c_str());
	return charArr;
}

char** makeArgv(char* input, int unnamedPipe[], vector<string> splittedInput)
{
 	static char* argv[6];
	char* pipeReadEnd = convertStringToCharArr(to_string(unnamedPipe[0]));
	char* pipeWriteEnd = convertStringToCharArr(to_string(unnamedPipe[1]));
	argv[0] = input;
	argv[1] = pipeReadEnd;
	argv[2] = pipeWriteEnd;
    argv[3] = convertStringToCharArr(splittedInput[1]);
    if (splittedInput.size() == 3){
        argv[4] = convertStringToCharArr(splittedInput[2]);
    }
    else
        argv[4] = NULL;
    argv[5] = NULL;
	return argv;
}

void writeInUnnamedPipe(char* toBeWriten, int unnamedPipe[])
{
	write(unnamedPipe[1], toBeWriten, strlen(toBeWriten)); 
}

string convertCharArrToString(vector<char> v)
{
	ostringstream out;
    for (char c : v) {
        out << c;
    }
    string s(out.str());
    return s;
}

string convertFrameToString(Frame frame)
{
	string s;
	s = to_string(frame.dataSize) + "_";
	s += convertCharArrToString(frame.data);
	s += to_string(frame.destination) + "_" + to_string(frame.source) + "_" + to_string(frame.num) + "_" + to_string(frame.numberOfFrame) + "_" + frame.fileName;
	return s;
}

Frame convertCharArrToFrame(char* input)
{
	Frame frame;
	string s = string(input);
	frame.dataSize = stoi(s.substr(0, s.find("_")));
	s.erase(0, s.find("_") + 1);
	for(int i = 0; i < frame.dataSize; i++)
		frame.data.push_back(s[i]);
	s.erase(0, frame.dataSize);
	frame.destination = stoi(s.substr(0, s.find("_")));
	s.erase(0, s.find("_") + 1);
	frame.source = stoi(s.substr(0, s.find("_")));
	s.erase(0, s.find("_") + 1);
	frame.num = stoi(s.substr(0, s.find("_")));
	s.erase(0, s.find("_") + 1);
	frame.numberOfFrame = stoi(s.substr(0, s.find("_")));
	s.erase(0, s.find("_") + 1);
	frame.fileName = s;
	return frame;
}

void writeInNamedPipe(const char* namedPipeName, Frame frame)
{
	char* data = convertStringToCharArr(convertFrameToString(frame));
	int namedPipe = open(namedPipeName, O_WRONLY | O_NONBLOCK);
	write(namedPipe, data, strlen(data) + 1);
	close(namedPipe);
}

ReadInfoFromUnnamedPipe readFromUnnamedPipe(int writeEnd, int readEnd)
{
    ReadInfoFromUnnamedPipe readInfo;
	static char input[SIZE] = "";
	memset(input, 0, strlen(input));
	int valread = read(readEnd, input, SIZE);
    readInfo.buf = input;
    readInfo.valread = valread;
	return readInfo;
}

ReadInfoFromNamedPipe readFromNamedpipe(int namedPipe)
{
    ReadInfoFromNamedPipe readInfo;
	static char input[SIZE_NAMED_PIPE] = "";
	memset(input, 0, strlen(input));
	int valread = read(namedPipe, input, SIZE_NAMED_PIPE);
	if(valread > 0)
    	readInfo.frame = convertCharArrToFrame(input);
    readInfo.valread = valread;
	return readInfo;
}

void writeDataInFile(string fileName, vector<char> data)
{
    ofstream file;
    file.open("out-" + fileName, ios::app);
	string s(data.begin(), data.end());
    file << s;
}