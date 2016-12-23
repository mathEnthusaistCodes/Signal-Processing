#ifndef SPEECH_H_INCLUDED
#define SPEECH_H_INCLUDED

# include "types.h"

vector<double> suppressNoiseWiener(vector<double> &, double);
void bufferAndWindowSpeech(vector<double> &, double, double, double);
vector<double> suppressNoiseSpectralSubtraction(vector<double> &, double);

#endif // SPEECH_H_INCLUDED
