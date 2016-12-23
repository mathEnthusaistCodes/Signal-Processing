#include "exception"
#include <iostream>
#include <algorithm>

#include "Window.h"

using namespace std;


Window::Window(int width,int height, string title)
{
    this->height = height;
    this->width = width;
    this->initialize(title);

}

void Window::initialize(string title)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        // Unrecoverable error, exit here.
        cout << "SDL_Init failed: %s\n" <<  SDL_GetError() << endl;
        throw exception();
    }

    window = SDL_CreateWindow(title.c_str(),
                                         SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                         width, height, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        cout << "SDL_Init failed: %s\n" << SDL_GetError();
        SDL_Quit();
        throw exception();
    }

    window_renderer = SDL_CreateRenderer(window, -1 , SDL_RENDERER_SOFTWARE);
    window_texture = SDL_CreateTexture(window_renderer, SDL_PIXELFORMAT_RGBA8888,
                                                    SDL_TEXTUREACCESS_STATIC, width, height);

    if (window_renderer == NULL || window_texture == NULL)
    {
        cout << "SDL_Init failed: %s\n" << SDL_GetError();
        SDL_Quit();
        throw exception();
    }


}

void Window::setBackgroundColor(int color)
{
    Uint32 *pixels = new Uint32[width*height];
    for_each(pixels, pixels + width*height,[&color](Uint32 &n) {n = color;} );
    SDL_UpdateTexture(window_texture, NULL, pixels, width*sizeof(Uint32));
    SDL_RenderClear(window_renderer);
    SDL_RenderCopy(window_renderer, window_texture, NULL, NULL);
    SDL_RenderPresent(window_renderer);
    delete [] pixels;
}

void Window::createAxes(double x_range, double y_range, int offset)
{
    SDL_SetRenderDrawColor(window_renderer, 255, 0, 0, 255);
    this->x_range = x_range;
    this->y_range = y_range;
    ax_offset = offset;
    x_off = ax_offset* width /100;
    y_off = ax_offset* height/100;


    drawLine(0, 0 , x_range, 0);
    drawLine(0, 0 , 0, y_range);
    drawLine(0, y_range, x_range, y_range);

    drawLine(x_range, 0 , x_range, y_range);

    addAxesLabels();
}

void Window::transsformPoints(double X, double Y, double &plotX, double &plotY)
{
    // Apply offset, scaling and flip
    X*= (double)(width - 2*x_off)/x_range;
    Y*= (double)(height - 2*y_off)/y_range;

    X += x_off;
    Y += y_off;

    plotX = X;
    plotY = height - Y;
}

void Window::drawLine(double X1, double Y1, double X2, double Y2)
{
    double plotX1, plotY1, plotX2, plotY2;
    transsformPoints( X1 , Y1 , plotX1, plotY1);
    transsformPoints( X2 , Y2 , plotX2, plotY2);
    SDL_RenderDrawLine(window_renderer, (int)plotX1, (int)plotY1, (int)plotX2, (int)plotY2);
}

void Window::renderWindow()
{
        SDL_RenderPresent(window_renderer);
}

void Window::addAxesLabels()
{
    // Not available yet
/*   TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 12);
    SDL_Color White = {255, 0, 0};
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, "Test Plot", White);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(window_renderer, surfaceMessage);
    SDL_Rect Message_rect;
    Message_rect.x = WINDOW_WIDTH/2 - 10;
    Message_rect.y = Y_offset/2;
    Message_rect.w = 100;
    Message_rect.h = 20;
    SDL_RenderCopy(window_renderer, Message, NULL, &Message_rect);
*/
}


void Window::waitAndClose()
{
    bool quit_now = false;

    SDL_Event event;

    while(!quit_now)
    {

        while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT) quit_now = true;
            }
    }
    destroy();

}

void Window::destroy()
{
    SDL_DestroyTexture(window_texture);
    SDL_DestroyRenderer(window_renderer);
    SDL_DestroyWindow(window);

}

Window::~Window()
{
    //dtor
    destroy();
    SDL_Quit();
}
