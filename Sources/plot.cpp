#include "algorithm"

#include "utils.h"
#include "window.h"


using namespace std;


void plotComplexWave(ComplexVector& wave_iq, string title)
{
    vector<double> real_wave,complex_wave,abs_wave,phase_wave;
    //real_wave = wave_iq.get_real();
    //complex_wave = wave_iq.get_complex();
    abs_wave = get_real(wave_iq);
    phase_wave = get_phase(wave_iq);
    plotWave(abs_wave,"Magnitude FT using: " + title);
    plotWave(phase_wave, "Phase FT using " + title);
}

void plotWave(vector<double>& wave_i, const string& title )
{
    //#ifndef DEBUG
        Window my_sdl_window(500, 500, title);
        my_sdl_window.setBackgroundColor(255);

        double x_range, y_range;
        double y_offset = 0;
        getRange(x_range, y_range, y_offset, wave_i);
        my_sdl_window.createAxes(x_range, y_range, 5);

        for(int i=0; i<wave_i.size()-1; i++)
        {
            my_sdl_window.drawLine(i, wave_i.at(i)-y_offset , i+1 , wave_i.at(i+1)-y_offset);
        }

        my_sdl_window.renderWindow();

        my_sdl_window.waitAndClose();
    //#endif // DEBUG
}

void getRange(double & x_range, double & y_range, double & y_offset, vector<double>& data_in)
{
    double max_val = *max_element(data_in.begin(), data_in.end());
    double min_val = *min_element(data_in.begin(), data_in.end());
    x_range = data_in.size();
    y_range = max_val - min_val;
    y_offset = min_val;
}
