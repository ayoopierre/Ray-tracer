#include "Screen.h"
#include <SDL2/SDL.h>

bool Screen::init()
{
    if(SDL_Init(SDL_INIT_EVERYTHING)<0){
        return false;
    }

    window = SDL_CreateWindow("Window", 
                                SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED,
                                WIDTH, HEIGHT, 0);
    if(!window){
        return false;
    }

    surface = SDL_GetWindowSurface(window);

    if(!surface){
        return false;
    }

    return true;
}

bool Screen::create_renderer()
{
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
    buffer = new Uint32[WIDTH*HEIGHT];
    post_processing_buffer = new Uint32[WIDTH*HEIGHT];
    for(int i=0; i<HEIGHT*WIDTH; i++){
        buffer[i] = 0;
    }
    for(int i=0; i<HEIGHT*WIDTH; i++){
        post_processing_buffer[i] = 0;
    }
    return true;
}

bool Screen::update()
{
    //add_box_blur();
    SDL_Event e;
    bool keep_window_open = true;
    while(keep_window_open)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) > 0)
        {
            switch(e.type)
            {
                case SDL_QUIT:
                    keep_window_open = false;
                    break;
            }

            SDL_UpdateTexture(texture, NULL, buffer, WIDTH*sizeof(Uint32));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    //close()
    delete []buffer;
    delete []post_processing_buffer;
    SDL_Quit();

    return true;
}

void Screen::set_skybox(Uint32 bottom_color, Uint32 top_color)
{
    Uint8 new_red;
    Uint8 new_green;
    Uint8 new_blue;

    Uint32 copy = bottom_color;
    copy = copy >> 8;
    Uint8 b_blue = copy & 0xff;
    copy = copy >> 8;
    Uint8 b_green = copy & 0xff;
    copy = copy >> 8;
    Uint8 b_red = copy & 0xff;

    copy = top_color;
    copy = copy >> 8;
    Uint8 t_blue = copy & 0xff;
    copy = copy >> 8;
    Uint8 t_green = copy & 0xff;
    copy = copy >> 8;
    Uint8 t_red = copy & 0xff;


    for(int i = 0; i<HEIGHT ; i++){
        //setup new
        new_red = (i * b_red + (HEIGHT-i) * t_red)/HEIGHT;
        new_green = (i * b_green + (HEIGHT-i) * t_green)/HEIGHT;
        new_blue = (i * b_blue + (HEIGHT-i) * t_blue)/HEIGHT;

        copy = 0;
        copy += new_red;
        copy = copy << 8;
        copy += new_green;
        copy = copy << 8;
        copy += new_blue;
        copy = copy << 8;
        copy += 0xFF;

        for(int j = 0; j<WIDTH ; j++){
            buffer[i*WIDTH + j] = copy;
        }
    }


}

void Screen::set_buffer(Uint32 color, int x, int y)
{
    buffer[(HEIGHT-y)*WIDTH + x] = color;//(HEIGHT-y)*WIDTH
}

Uint32 Screen::get_buffer_color(int x, int y){
    Uint32 ret = buffer[(HEIGHT-y)*WIDTH + x];
    return ret;
}

void Screen::add_box_blur(){

    int sum_red = 0;
    int sum_green = 0;
    int sum_blue = 0;

    Uint32 copy;

    for(int x=1; x < WIDTH-1 ; x++){
        for(int y=1; y<HEIGHT-1 ; y++){
            for(int x_in=-1; x_in < 2; x_in++){
                for(int y_in=-1; y_in<2; y_in++){
                    copy = buffer[(y+y_in)*WIDTH+(x+x_in)];
                    copy = copy >> 8;
                    Uint8 b_blue = copy & 0xff;
                    copy = copy >> 8;
                    Uint8 b_green = copy & 0xff;
                    copy = copy >> 8;
                    Uint8 b_red = copy & 0xff;
                    sum_red += b_red;
                    sum_blue += b_blue;
                    sum_green += b_green;
                } 
            }
            sum_red = sum_red/9;
            sum_green = sum_green/9;
            sum_blue = sum_blue/9;

            copy = 0;
            copy += sum_red;
            copy = copy << 8;
            copy += sum_green;
            copy = copy << 8;
            copy += sum_blue;
            copy = copy << 8;
            copy += 0xFF;
            post_processing_buffer[y*WIDTH+x] = copy;

            sum_blue = 0;
            sum_green = 0;
            sum_red = 0;
        }
    }

}

Screen::Screen()
{
}

