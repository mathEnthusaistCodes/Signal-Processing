#ifndef WINDOW_H
#define WINDOW_H

#include "vector"
#include "string"

#define SDL_MAIN_HANDLED
#include "SDL.h"

using namespace std;

class Window
{
    public:
        Window(int, int, string);
        virtual ~Window();
        void setBackgroundColor(int color);
        void createAxes(double, double, int);
        void drawLine(double X1, double Y1, double X2, double Y2);
        void waitAndClose();
        void renderWindow();

    protected:

    private:
        int height, width;
        int ax_offset, x_off, y_off ; // Percentage
        double x_range, y_range;
        SDL_Window *window;
        SDL_Renderer *window_renderer;
        SDL_Texture *window_texture;
        void transsformPoints(double X, double Y, double &plotX, double &plotY);
        void addAxesLabels();
        void initialize(string);
        void destroy();
};


#endif // WINDOW_H
