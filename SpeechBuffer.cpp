#include "utils.h"
#include "types.h"

SpeechBuffer::SpeechBuffer(int rows, int bufferLength, int overlap)
{
    this->bufferLength = bufferLength;
    this->overlap = overlap;
    this->length = rows;
    this->bufferMatrix.resize(rows, vector<double>(bufferLength ,0));
    this->nextOut = 0;
}

void SpeechBuffer::initializeBuffer(vector<double> data_all)
{
    int inputLength = data_all.size();
    int numberOfRowsInInput = ceil(double(inputLength - this->overlap)/double(this->bufferLength - this->overlap));
    int counter = 0;
    this->currentRow = 0;
    vector<double> tempVector;
    tempVector.resize(this->bufferLength);
    vector<double>::iterator it = data_all.begin();
    for(int i=0;i<numberOfRowsInInput-1;i++)
    {
        tempVector.assign(it+counter,it+counter+this->bufferLength);
        this->add_to_end(tempVector);
        counter += this->bufferLength - overlap;
    }
    //last row with rest of items
    tempVector.assign(it+counter,data_all.end());
    this->add_to_end(tempVector);
    
}

vector<double> SpeechBuffer::get_next()
{
    //vector<double> tempVector;
    return this->bufferMatrix[this->nextOut++];
}

void SpeechBuffer::add_to_end(vector<double> data_in)
{
    
    if ((this->currentRow+1) >= this->length)
    {
        // Circular buffer
        this->currentRow = 0;
    }
    this->bufferMatrix[currentRow] = data_in;
    this->currentRow++;
}

int SpeechBuffer::getLength()
{
    
    return this->currentRow;
}
