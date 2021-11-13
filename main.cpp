#include <stdio.h>
#include <SDL.h>

#include <bounce.h>

// screen dimensions
const int H_RES = 640;
const int V_RES = 480;

typedef struct Pixel {  // for SDL texture
    uint8_t a;  // transparency
    uint8_t b;  // blue
    uint8_t g;  // green
    uint8_t r;  // red
} Pixel;

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init failed.\n");
        return 1;
    }

    Pixel screenbuffer[H_RES*V_RES];

    SDL_Window*   sdl_window   = NULL;
    SDL_Renderer* sdl_renderer = NULL;
    SDL_Texture*  sdl_texture  = NULL;

    sdl_window = SDL_CreateWindow("Top Bounce", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, H_RES, V_RES, SDL_WINDOW_SHOWN);
    if (!sdl_window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        return 1;
    }

    sdl_renderer = SDL_CreateRenderer(sdl_window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdl_renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        return 1;
    }

    sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, H_RES, V_RES);
    if (!sdl_texture) {
        printf("Texture creation failed: %s\n", SDL_GetError());
        return 1;
    }

    cxxrtl_design::p_top__bounce top;

    top.p_rst.set(1U);
    top.p_clk__pix.set(0U);
    top.step();
    top.p_rst.set(0U);
    top.step();

    uint64_t frame_count = 0;
    uint64_t start_ticks = SDL_GetPerformanceCounter();
    while (1) {
        // cycle the clock
        top.p_clk__pix.set(1U);
        top.step();
        top.p_clk__pix.set(0U);
        top.step();

        auto sy = top.p_sy.get<uint32_t>();
        auto sx = top.p_sx.get<uint32_t>();

        // update pixel if not in blanking interval
        if (top.p_de.get<bool>()) {
            Pixel* p = &screenbuffer[sy * H_RES + sx];
            p->a = 0xFF;  // transparency
            p->b = top.p_sdl__b.get<uint8_t>();
            p->g = top.p_sdl__g.get<uint8_t>();
            p->r = top.p_sdl__r.get<uint8_t>();
        }

        // update texture once per frame at start of blanking
        if (sy == V_RES && sx == 0) {
            // check for quit event
            SDL_Event e;
            if (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    break;
                }
            }

            SDL_UpdateTexture(sdl_texture, NULL, screenbuffer, H_RES*sizeof(Pixel));
            SDL_RenderClear(sdl_renderer);
            SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
            SDL_RenderPresent(sdl_renderer);
            frame_count++;
        }

    }

    uint64_t end_ticks = SDL_GetPerformanceCounter();
    double duration = ((double)(end_ticks-start_ticks))/SDL_GetPerformanceFrequency();
    double fps = (double)frame_count/duration;
    printf("Frames per second: %.1f\n", fps);

    SDL_DestroyTexture(sdl_texture);
    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);
    SDL_Quit();

    return 0;
}
