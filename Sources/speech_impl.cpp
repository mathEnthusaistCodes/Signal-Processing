#include <complex>
#include <ctime>
#include <algorithm>

#include "utils.h"
#include "types.h"
#include "speech.h"
#include "windows.h"

void bufferAndWindowSpeech(vector<double> &data_in,double Fs,double bufferDuration, double overlap)
{
    double bufferLength = bufferDuration*Fs; //time to samples

    // Adjust buffer length to ext power of two.
    bufferLength = pow(2,ceil(log2(bufferLength)));

    SpeechBuffer sp(1000,bufferLength,bufferLength/2);
    sp.initializeBuffer(data_in);

    // Iterate through the buffered speech and window
    vector<double> currentVector;
    vector<double> hanning_window = hanning(bufferLength);
    vector<double> windowedSpeech;
    windowedSpeech.resize(bufferLength);
    ComplexVector wave_freq;
    vector<double> abs_fft;
    vector<double> spectrum;

    //Do some time analysis
    double input_speech_time = data_in.size()/Fs;
    DEBUG("Speech length in sec: ");
    DEBUG(input_speech_time);
    clock_t start;
    double diff;
    start = clock();
    // End time- start

    currentVector.resize(bufferLength);
    for (int j=0; j<sp.getLength(); j++)
    {
        currentVector = sp.get_next();
        // Window the buffered speech
        transform(currentVector.begin(), currentVector.end(), hanning_window.begin(), windowedSpeech.begin(), multiplies<double>());
        wave_freq = fft(real2complex(windowedSpeech));
        abs_fft = get_abs(wave_freq);
        spectrum = powerspectrum(abs_fft);
    }

    //time analysis - diff
    diff = ( std::clock() - start ) / (double)CLOCKS_PER_SEC;
    cout<<"Time it took to calc spectrum of " << sp.getLength() << "buffers was : "<< diff <<"s\n";
    // End time

    //plotWave(windowedSpeech,"Current speech buffer");
}

double getNoisePower(vector<double> & speech_in, double timeLength, double bufferLength, double fs, vector<double> &runningNoisePower)
{

        // 1) Estimate noise power from first 100ms data
    int noiseLength = fs*timeLength;

    SpeechBuffer noiseBuffer(1000,bufferLength,0);
    vector<double> noise_input(speech_in.begin(), speech_in.begin() + noiseLength);
    noiseBuffer.initializeBuffer(noise_input);

    vector<double> currentVector;
    vector<double> hanning_window = hanning(bufferLength);
    vector<double> windowedSpeech;
    windowedSpeech.resize(bufferLength);
    currentVector.resize(bufferLength);
    ComplexVector wave_freq;
    vector<double> abs_fft;
    abs_fft.resize(bufferLength);

    runningNoisePower.assign(bufferLength,0);


    for (int j=0; j<noiseBuffer.getLength(); j++)
    {
        currentVector = noiseBuffer.get_next();
        // Window the buffered speech
        transform(currentVector.begin(), currentVector.end(), hanning_window.begin(), windowedSpeech.begin(), multiplies<double>());

        wave_freq = fft(real2complex(windowedSpeech));
        abs_fft = get_abs(wave_freq);
        transform(abs_fft.begin(), abs_fft.end(), runningNoisePower.begin(), runningNoisePower.begin(), [](double n, double m){return(n+m);});
        //plotWave(abs_fft,"FFT");
    }
    int buffers = noiseBuffer.getLength();
    for_each(runningNoisePower.begin(), runningNoisePower.end(), [&buffers](double &n){n = n/buffers;});

    // Noise power estimate
    double noisePower = 0;
    for_each(runningNoisePower.begin(), runningNoisePower.end(), [&noisePower](double &n){noisePower = noisePower + pow(n,2);});
    noisePower = noisePower/runningNoisePower.size();

    return noisePower;
}


vector<double> suppressNoiseWiener(vector<double> &speech_in, double fs)
{

    double bufferDuration = .02; //seconds
    double bufferLength = bufferDuration*fs; //time to samples
    // Adjust buffer length to next power of two.
    bufferLength = pow(2,ceil(log2(bufferLength)));

    vector<double> runningNoisePower(bufferLength);

    double noisePower = getNoisePower(speech_in, .1,  bufferLength, fs, runningNoisePower);

    DEBUG(noisePower);

    //Now, buffer input speech
    SpeechBuffer sp = SpeechBuffer(1000, bufferLength, bufferLength/2.0);
    sp.initializeBuffer(speech_in);


    vector<double> currentVector;
    vector<double> hanning_window = hanning(bufferLength);
    vector<double> windowedSpeech;
    windowedSpeech.resize(bufferLength);
    currentVector.resize(bufferLength);
    ComplexVector wave_freq;
    vector<double> abs_fft;
    abs_fft.resize(bufferLength);

    vector<double> cleanSpeech(speech_in.size() + bufferLength, 0);

    int bufferPosition = 0; // for overlap add
    double snrPriori = 0;
    double beta = .98;
    double snrPost = 0;

    double snr,H;
    double spPower = 0;

    for(int j=0; j<sp.getLength(); j++)
    {
        currentVector = sp.get_next();
        // Window the buffered speech
        transform(currentVector.begin(), currentVector.end(), hanning_window.begin(), windowedSpeech.begin(), multiplies<double>());
        wave_freq = fft(real2complex(windowedSpeech));
        abs_fft = get_abs(wave_freq);

        spPower = 0;
        for_each(abs_fft.begin(), abs_fft.end(), [&spPower](double &n){spPower = spPower + pow(n,2);});
        spPower = spPower/bufferLength;

        snrPost = spPower/noisePower;
        if((snrPost - 1)< 0)
        {
            snrPost = 1;
        }
        snr = (1 - beta)*(snrPost - 1)+beta*snrPriori;
        H = sqrt(snr/(1 + snr));
        for_each(wave_freq.begin(), wave_freq.end(), [H](complex<double> &n){n*=H;});
        abs_fft = get_real(ifft(wave_freq));
        transform(cleanSpeech.begin() + bufferPosition, cleanSpeech.begin() + bufferPosition + bufferLength,
                  abs_fft.begin(), cleanSpeech.begin() + bufferPosition, [](double n, double m)->double{return(n+m);});
        spPower = 0;
        abs_fft = get_abs(wave_freq);
        for_each(abs_fft.begin(), abs_fft.end(), [&spPower](double &n){spPower = spPower + pow(n,2);});
        spPower = spPower/bufferLength;
        snrPriori = spPower/noisePower;

        bufferPosition += bufferLength/2;
    }

    return (cleanSpeech);

}

vector<double> suppressNoiseSpectralSubtraction(vector<double> &speech_in, double fs)
{

    double bufferDuration = .02; //seconds
    double bufferLength = bufferDuration*fs; //time to samples
    // Adjust buffer length to next power of two.
    bufferLength = pow(2,ceil(log2(bufferLength)));

    vector<double> runningNoisePower(bufferLength);
    double noisePower = getNoisePower(speech_in, .1,  bufferLength, fs, runningNoisePower);

    DEBUG(noisePower);

    //Now, buffer input speech
    SpeechBuffer sp = SpeechBuffer(1000, bufferLength, bufferLength/2.0);
    sp.initializeBuffer(speech_in);


    vector<double> currentVector;
    vector<double> hanning_window = hanning(bufferLength);
    vector<double> windowedSpeech;
    windowedSpeech.resize(bufferLength);
    currentVector.resize(bufferLength);
    ComplexVector wave_freq;
    vector<double> abs_fft;
    abs_fft.resize(bufferLength);

    vector<double> cleanSpeech(speech_in.size() + bufferLength, 0);
    int bufferPosition = 0; // for overlap add
    double snrPriori = 0;
    vector<double> phaseNoisySpeech;
    double snrPost = 0;

    double c_yd,c_xd;
    double spPower = 0;

    ComplexVector cleanEstimate_1, cleanEstimate_2, cleanEstimate_3;
    cleanEstimate_1.resize(bufferLength);
    cleanEstimate_2.resize(bufferLength);
    cleanEstimate_3.resize(bufferLength);
    phaseNoisySpeech.resize(bufferLength);

    for(int j=0; j<sp.getLength(); j++)
    {
        currentVector = sp.get_next();
        // Window the buffered speech
        transform(currentVector.begin(), currentVector.end(), hanning_window.begin(), windowedSpeech.begin(), multiplies<double>());
        wave_freq = fft(real2complex(windowedSpeech));
        abs_fft = get_abs(wave_freq);

        phaseNoisySpeech = get_phase(wave_freq);

        spPower = 0;
        for_each(abs_fft.begin(), abs_fft.end(), [&spPower](double &n){spPower = spPower + pow(n,2);});
        spPower = spPower/bufferLength;

        snrPost = spPower/noisePower;
        c_yd = max((snrPost+1-snrPriori)/2*sqrt(snrPost),1.0);
        c_xd = max((snrPost-1-snrPriori)/2*sqrt(snrPriori),1.0);

        c_yd = min(c_yd,50.0);
        c_xd = min(c_xd,50.0);

        transform(abs_fft.begin(), abs_fft.end(), runningNoisePower.begin(), cleanEstimate_1.begin(),
                  [&c_yd](double n, double m)->complex<double>{return (sqrt(abs(pow(n - m*c_yd,2) + pow(m,2)*(1-pow(c_yd,2)))));});
        transform(abs_fft.begin(), abs_fft.end(), runningNoisePower.begin(), cleanEstimate_2.begin(),
                  [&c_yd, &c_xd](double n, double m)->complex<double>{return ( (n*c_yd - m)/c_xd );});
        transform(abs_fft.begin(), abs_fft.end(), runningNoisePower.begin(), cleanEstimate_3.begin(),
                  [&c_yd](double n, double m)->complex<double>{return (sqrt(abs(pow(m - n*c_yd,2) + pow(n,2)*(1-pow(c_yd,2)))));});

        transform(cleanEstimate_1.begin(), cleanEstimate_1.end(), cleanEstimate_2.begin(),
                  cleanEstimate_1.begin(), //output goes here
                  [](complex<double> n, complex<double> m)->complex<double>{return ( (n+m)/(2.0) );});
        transform(cleanEstimate_1.begin(), cleanEstimate_1.end(), cleanEstimate_3.begin(),
                  cleanEstimate_1.begin(), //output goes here
                  [](complex<double> n, complex<double> m)->complex<double>{return ( (n+m)/(2.0) );});

        transform(cleanEstimate_1.begin(), cleanEstimate_1.end(), phaseNoisySpeech.begin(),
                  wave_freq.begin(), //output goes here
                  [](complex<double> n, double m)->complex<double>{return ( n *(exp(m*i)) );});

        abs_fft = get_real(ifft(wave_freq));
        transform(cleanSpeech.begin() + bufferPosition, cleanSpeech.begin() + bufferPosition + bufferLength,
                  abs_fft.begin(), cleanSpeech.begin() + bufferPosition, [](double n, double m)->double{return(n+m);});

        if (j == 21)
        {
            plotWave(cleanSpeech, "Clean part 1");
        }

        spPower = 0;
        abs_fft = get_abs(wave_freq);
        for_each(abs_fft.begin(), abs_fft.end(), [&spPower](double &n){spPower = spPower + pow(n,2);});
        spPower = spPower/bufferLength;
        snrPriori = spPower/noisePower;

        bufferPosition += bufferLength/2;
    }

    return (cleanSpeech);

}
