#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>
#include <fstream>
#include <vector>

# include "types.h"

// Hepler function defined in other cpp files
void plotWave(std::vector<double>&, const std::string&);
void plotComplexWave(ComplexVector& , string );
void getRange(double &, double &, double &, std::vector<double>&);
ComplexVector getComplexExpWave();
ComplexVector dft(ComplexVector);
ComplexVector fft(ComplexVector);
ComplexVector idft(ComplexVector&);
ComplexVector ifft(ComplexVector&);
ComplexVector bitReverseOrder(ComplexVector);
ComplexVector getTwiddles(int);
int binaryToDec(std::string);
vector<double> get_real(ComplexVector);
vector<double> get_imag(ComplexVector);
vector<double> get_abs(ComplexVector);
vector<double> get_phase(ComplexVector);
ComplexVector real2complex(vector<double>);
vector<double> powerspectrum(vector<double>);
void normalize(vector<double>&);

class WaveFileReadWriter
{
    public:
        vector<double> readWaveFile(string fileName, double &Fs);
        vector<double> updateWaveFile(string fileName, double &Fs);
        void writeToFile(vector<double> data_in);
        ~WaveFileReadWriter();

    private:
        string fileName;
        int data_length;
};

#endif
