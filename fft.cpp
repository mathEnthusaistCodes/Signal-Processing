#include <assert.h>
#include <math.h>
#include <bitset>
#include <algorithm>


#include "types.h"
#include "utils.h"

ComplexVector dft(ComplexVector timeVector)
{
    ComplexVector freqVector = timeVector;
    //freqVector.resize(timeVector.size());
    // Use DFT
    for(int k=0; k<timeVector.size(); k++)
    {
        int N = timeVector.size();
        //DEBUG(N);
        complex<double> freq_at_index = 0;
        for(int n = 0; n<N; n++)
        {
            //DEBUG(n);
            freq_at_index += timeVector.at(n)*exp((-2*PI*double(n)*double(k)*i)/double(N));
        }
        freqVector.at(k) = freq_at_index;
    }
    return freqVector;
}

ComplexVector idft(ComplexVector& freqVector)
{
    ComplexVector timeVector;
    // Use IDFT
    for(int k=0; k<freqVector.size(); k++)
    {
        int N = freqVector.size();
        //DEBUG(N);
        complex<double> time_at_index = 0;
        for(int n=0; n<=N-1; n++)
        {
            //DEBUG(n);
            time_at_index += freqVector.at(n)*exp((2*PI*double(n)*double(k)*i)/double(N));
        }
        timeVector.push_back(time_at_index/double(N));

    }
    return timeVector;
}

ComplexVector fft(ComplexVector timeVector)
//FFT 2-Radix, as explained in Digital Signal Processing for Modern Communications, Schwarzinger
{
    ComplexVector freqVector = timeVector;
    freqVector = bitReverseOrder(freqVector); //Re-arrange the inputs for DIT
    int N = freqVector.size();
    assert(std::fmod(log2(N),1) == 0);
    int iter = log2(N);
    //    static int prevN = N;
    ComplexVector twiddles = getTwiddles(N);

    for(int ind=1; ind<=iter ; ind++)
    {
        double fft_length = pow(2,ind);
        vector<double> twiddle_factors;
        for (int j=0; j<fft_length/2; j++) twiddle_factors.push_back(j);
        int reps = N/fft_length; //Repetitions/Blocks per iteration
        int block = 0;           //FFT-sized block
        for(int reps_ind = 1;reps_ind <=reps; reps_ind++)
        {
            for(vector<double>::iterator it = twiddle_factors.begin() ; it != twiddle_factors.end(); ++it)
            {
                complex<double> W_k_N = twiddles.at((*it)*reps);
                complex<double> in_1 = freqVector.at((*it)+block);
                complex<double> in_2 = freqVector.at((*it)+block+fft_length/2);
                freqVector.at((*it)+block) = in_1 + W_k_N*in_2;;
                freqVector.at((*it)+block+fft_length/2) =  in_1 - W_k_N*in_2;
            }
            block = block + fft_length;
        }
    }

    return(freqVector);
}


// Immplemented as mentioned here - http://www.embedded.com/design/configurable-systems/4210789/DSP-Tricks--Computing-inverse-FFTs-using-the-forward-FFT
ComplexVector ifft(ComplexVector& freqVector)
{
    ComplexVector SwappedVector;
    SwappedVector.resize(freqVector.size());
    for(int k=0; k<freqVector.size(); k++)
    {
        SwappedVector[k] = imag(freqVector[k]) + i*real(freqVector[k]);
    }

    SwappedVector = fft(SwappedVector);

    for(int k=0; k<freqVector.size(); k++)
    {
        SwappedVector[k] = imag(SwappedVector[k]) + i*real(SwappedVector[k]);
    }

    return SwappedVector;

}

ComplexVector getTwiddles(int length)
{
    ComplexVector twiddle_out;
    for(int ind = 0;ind<=(length/2 -1);ind++)
            {
                complex<double> theta_t = (-2*PI*double(ind)*i)/double(length);
                twiddle_out.push_back(exp(theta_t));
            }
    return twiddle_out;
}

ComplexVector bitReverseOrder(ComplexVector data_in)
{
    int bitS = log2(data_in.size());
    ComplexVector data_out = data_in;
    bitset<256> b;
    for(int ind=0;ind<data_in.size();ind++)
            {
                string bit = bitset<256>(ind).to_string();;
                string bit_needed = bit.substr(256 - bitS,bitS);
                reverse(bit_needed.begin(),bit_needed.end());
                //int new_index = bitset<256> bit_r(bit_needed).to_ullong(); // Requires C++ 11
                int new_index = binaryToDec(bit_needed);
                data_out.at(new_index) = data_in.at(ind);
            }
    return data_out;
}

int binaryToDec(std::string number)
{
    int result = 0, pow = 1;
    for ( int i = number.length() - 1; i >= 0; --i, pow <<= 1 )
        result += (number[i] - '0') * pow;

    return result;
}
