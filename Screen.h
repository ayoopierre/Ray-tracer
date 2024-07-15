#ifndef _SCREEN_H_
#define _SCREEN_H_
#include <SDL2/SDL.h>

class Screen{
private:
    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Event event;
    Uint32 *buffer;
    Uint32 *post_processing_buffer;
public:
    const int HEIGHT = 800;
    const int WIDTH = 800;
    bool init();
    bool create_renderer();
    bool update();
    void set_skybox(Uint32 bottom_color, Uint32 top_color);
    void set_buffer(Uint32 color, int x, int y);
    void add_box_blur();
    Uint32 get_buffer_color(int x, int y);
    Screen();

};
#endif