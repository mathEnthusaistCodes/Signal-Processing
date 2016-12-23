#include <math.h>

#include "types.h"
#include "utils.h"
#include "math.h"
#include "algorithm"

ComplexVector getComplexExpWave()
{
    ComplexVector wave_iq;
    double sampleTime = .001;//seconds, just say
    double dataLength = sampleTime*1023;//seconds
    double freq = 20;

    //   wave_iq.push_back(1);
    for(double ind=0; ind<=dataLength; ind+=sampleTime)
    {
        //DEBUG(2*PI*i*freq*ind);
        complex<double> theta_t = 2*PI*freq*double(ind)*i;
        wave_iq.push_back(exp(theta_t));
        //DEBUG(cos(theta_t));
    }
    return wave_iq;

}

vector<double> get_real(ComplexVector cv)
{
    vector<double> real_items;
    real_items.resize(cv.size());
    for(unsigned int index=0; index < cv.size(); index ++)
    {
        real_items[index] = real(cv.at(index));
    }
    return(real_items);
}

vector<double> get_imag(ComplexVector cv)
{
    vector<double> real_items;
    real_items.resize(cv.size());
    for(unsigned int index=0; index<cv.size(); index ++)
    {
        real_items.push_back(imag(cv.at(index)));
    }
    return(real_items);
}

vector<double> get_abs(ComplexVector cv)
{
    vector<double> real_items;
    real_items.resize(cv.size());
    for(int unsigned index=0; index<cv.size(); index ++)
    {
        real_items[index] = abs(cv.at(index));
    }
    return(real_items);
}

vector<double> get_phase(ComplexVector cv)
{
    vector<double> real_items;
    real_items.resize(cv.size());
	for (unsigned int index = 0; index<cv.size(); index++)
    {
        real_items[index] = arg(cv.at(index));
    }
    return(real_items);
}

ComplexVector real2complex(vector<double> rv)
{
    ComplexVector cv;
    cv.resize(rv.size());
	for (unsigned int index = 0; index<rv.size(); index++)
    {
        cv[index] = complex<double>(rv[index]);
    }
    return(cv);
}

vector<double> powerspectrum(vector<double> fft_in)
{
    vector<double> ps;
    ps.resize(fft_in.size());
	for (unsigned int index = 0; index<fft_in.size(); index++)
    {
        ps[index] = 20*log(fft_in[index]);
    }
    return(ps);
}

void normalize(vector<double>& in ) 
{
    double max_val = *max_element(in.begin(),in.end());
	for (unsigned int index = 0; index<in.size(); index++)
    {
        in[index] = in[index]/abs(max_val);
    }
}
