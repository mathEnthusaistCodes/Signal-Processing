#include <cmath>
#include <complex>
#include <ctime>
#include <algorithm>
#include <functional>
#include <assert.h>

#include "types.h"
#include "utils.h"
#include "windows.h"
#include "speech.h"

using namespace std;

ComplexVector getExpWave(double a)
{
    ComplexVector wave_iq;
    double dataLength = 50;//samples

    for(double ind=0; ind<dataLength; ind++)
    {
        wave_iq.push_back(pow(a,ind));
    }
    DEBUG(wave_iq.at(0));
    //wave_iq.at(0) = 0;
    return wave_iq;

}
ComplexVector getRectWave(double length)
{
    ComplexVector wave_iq;

    for(int ind=0; ind<length; ind++)
    {
        wave_iq.push_back(1);
    }
    return wave_iq;

}

void measureFFTPerformance()
{
    ComplexVector wave_iq = getComplexExpWave();
    ComplexVector wave_freq_fft;
    ComplexVector wave_freq_dft;
    clock_t start;
    double diff;
    start = clock();

    for(int j=1;j<=50;j++)
    {
        wave_freq_fft = fft(wave_iq);
    }
    diff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
    cout<<"Time it took for 1024 fft for 50 runs: "<< diff <<"s\n";

    plotComplexWave(wave_freq_fft,"FFT");

    start = clock();

    for(int j=1;j<=50;j++)
    {
        wave_freq_dft = dft(wave_iq);
     }
    diff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
    cout<<"Time it took for dft: "<< diff <<"s\n";

    plotComplexWave(wave_freq_dft,"DFT");
}

void testBuffer()
{
    SpeechBuffer sp(25,100,25);
    vector<double> testVec(1024,22);
    sp.initializeBuffer(testVec);
    vector<double> testVector;
    testVector.resize(100);
    testVector = sp.get_next();
    for(int ind=0;ind<12;ind++)
    {
        sp.add_to_end(std::vector<double>(100,12));
    }
    testVector = sp.get_next();
    assert(testVector[25] == 12);
    DEBUG("Test Passed!!!");
}

void testWindow(vector<double> &data_in,double Fs,double bufferDuration, double overlap)
{
    double bufferLength = bufferDuration*Fs; //time to samples

    // Adjust buffer length to ext power of two.
    int base_2 = ceil(log2(bufferLength));
    bufferLength = pow(2,base_2);

    SpeechBuffer sp(1000,bufferLength,bufferLength/2);
    sp.initializeBuffer(data_in);
    vector<double> currentVector;
    currentVector.resize(bufferLength);
    // sp.get_next();
    // sp.get_next();
    // sp.get_next();
    sp.get_next();
    currentVector = sp.get_next();
    plotWave(currentVector,"2nd Buffer");

    vector<double> hanning_window = hanning(bufferLength);
    vector<double> windowedSpeech;
    windowedSpeech.resize(bufferLength);

    // Window the buffered speech
    transform(currentVector.begin(), currentVector.end(), hanning_window.begin(), windowedSpeech.begin(), multiplies<double>());

    // Compare Spectrums, rectangular and hanning

    ComplexVector wave_freq = fft(real2complex(currentVector));
    vector<double> abs_fft = get_abs(wave_freq);
    vector<double> spectrum = powerspectrum(abs_fft);
    plotWave(spectrum,"Spectrum with rectanngular window");

    wave_freq = fft(real2complex(windowedSpeech));
    abs_fft = get_abs(wave_freq);
    spectrum = powerspectrum(abs_fft);
    plotWave(spectrum,"Spectrum with hanning window");

}

string whichWavFile()
{
  int which;
  cout << "Which wav file to use? 1) Train 2) noisy speech:";
  cin >> which;
  if(which ==1)
    return "train.wav";
  else if (which ==2)
    return "sp01_car_sn10.wav";

}


int main(int argc, char* argv[])
{
    int user_input;
    cout << "Type required test: \n";
    cout << "1) FFT Performance: \n";
    cout << "2) Test buffer: \n";
    cout << "3) Test Window with a wave file: \n";
    cout << "4) Spectrum for windows of speech from a wave file: \n";
    cout << "5) Suppress noise with wiener filter: \n";
     cin >> user_input;
    vector<double> y;
    double diff;
    clock_t start;
    double fs;

    vector<double> data_out;

    WaveFileReadWriter wavfile;

    double scale;

    switch(user_input)
    {
      case 1:
        measureFFTPerformance();
        break;
      case 2:
        testBuffer();
        break;
      case 3:
        y = wavfile.readWaveFile(whichWavFile(), fs);
        normalize(y);
        plotWave(y,"Data from wav");
        testWindow(y, fs, 1/(fs*.02) , 0);
        break;
      case 4:
        y = wavfile.readWaveFile(whichWavFile(), fs);
        normalize(y);
        bufferAndWindowSpeech(y, fs, 1/(fs*.02), 0);
        break;
      case 5:
        y = wavfile.readWaveFile(whichWavFile(), fs);
        scale = *max_element(y.begin(), y.end());
        normalize(y);
        plotWave(y,"Data input");
        start = clock();
        data_out = suppressNoiseSpectralSubtraction(y, fs);
        diff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
        cout<<"Time it took for suppressing noise in input: "<< diff <<"s\n";
        normalize(data_out);
        for_each(data_out.begin(), data_out.end(), [scale](double &n){n*=scale;});
        scale = *max_element(data_out.begin(), data_out.end());
        plotWave(data_out,"Data output");
        wavfile.writeToFile(data_out);
        break;
      default:
        break;
    }

    return 0;
}
