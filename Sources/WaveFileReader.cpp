#include <iostream>
#include <fstream>
#include <stdint.h>
#include <vector>
#include <algorithm>

#include "utils.h"
#include "types.h"

using namespace std;

vector<double> WaveFileReadWriter::readWaveFile(string fileNameIn, double &Fs)
{
    fileName = fileNameIn;
    ifstream readFile(fileName.c_str(), ios::in | ios::binary);

    vector<int16_t> data_in;

    int32_t header32Data;

    bool data = false;
    readFile.seekg(24,ios::beg);// Move to location of Fs info in WAV header
    readFile.read(reinterpret_cast<char *>(&header32Data), sizeof(header32Data));
    Fs = (double)header32Data;

    readFile.seekg(40,ios::beg);// Move to location of Data header
    readFile.read(reinterpret_cast<char *>(&header32Data), sizeof(header32Data));
    data_length = (int)header32Data/2; //16-bits per sample
    //vector<double> data_out(data_in.begin(), data_in.end());

    vector<int16_t> data_16bit;
    data_16bit.resize(data_length);

    readFile.read(reinterpret_cast<char *>(&data_16bit.front()), data_length*2);
    readFile.close();

    vector<double> data_out(data_16bit.begin(),data_16bit.end());
    return data_out;
}

void WaveFileReadWriter::writeToFile(vector<double> data_in)
{

    vector<int16_t> data_16bit(data_in.begin(),data_in.end());

    ifstream readFile(fileName.c_str(), ios::in | ios::binary);
    readFile.seekg(0, ios::end);
    streampos sizeOfFile = readFile.tellg(); // in bytes
    char* all_data = new char[sizeOfFile];
    ofstream writeToFile("Output.wav", ofstream::binary);
    readFile.seekg (0, ios::beg);
    readFile.read(all_data, sizeOfFile);
    writeToFile.write(all_data, sizeOfFile);

    // Overwrite data
    writeToFile.seekp(44, ios::beg);
    writeToFile.write(reinterpret_cast<char *>(&data_16bit.front()), data_length*2);

    writeToFile.close();
    delete[] all_data;
}


WaveFileReadWriter::~WaveFileReadWriter()
{

}

