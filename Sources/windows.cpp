
#include "windows.h"
#include "types.h"

using namespace std;

vector<double> hanning(double length)
{
    vector<double> window_out;
    window_out.resize(length); // initialize
    for(int j = 0;j<length;j++)
    {
        window_out[j] = 0.5 * (1 - cos(2*PI*j/(length-1)));
    }
    return window_out;
}

vector<double> hamming(double length)
{
    vector<double> window_out;
    window_out.resize(length); // initialize
    for(int j = 0;j<length;j++)
    {
        window_out[j] = 0.5 * (1 - cos(2*PI*j/(length-1)));
    }
    return window_out;
}
