#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
   uint32_t rmask = 0xff000000;
   uint32_t gmask = 0x00ff0000;
   uint32_t bmask = 0x0000ff00;
   uint32_t amask = 0x000000ff;
#else
   uint32_t rmask = 0x000000ff;
   uint32_t gmask = 0x0000ff00;
   uint32_t bmask = 0x00ff0000;
   uint32_t amask = 0xff000000;
#endif

SDL_Surface *result = NULL;
SDL_Surface *sign_surface = NULL;
TTF_Font *font = NULL;
bool init_mcq()
{
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) | IMG_INIT_PNG | IMG_INIT_JPG))
    {
        printf("Can't init SDL2_image. SDL error: %s", SDL_GetError());
        return false;
    }
    if (TTF_Init() == -1)
    {
        printf("TTF_Init: %s\n", TTF_GetError());
        return 2;
    }
    sign_surface = IMG_Load("assets/sign.png");
    font = TTF_OpenFont("assets/font.ttf", 32);   
}

SDL_Surface *render_text(char *text[], int size)
{
    SDL_Surface *ret_surf;
    ret_surf = SDL_CreateRGBSurface(0, 800, 400, 32,
                                   rmask, gmask, bmask, amask);
    if (ret_surf == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        return NULL;
    }

    SDL_BlitSurface(sign_surface, NULL, ret_surf, NULL);

    for (int i = 0; i < size; i++)
    {
        //text[i][strlen(text[i]) * 4 ] = 0;  /*тут небезапасна чинить нада блять*/
        SDL_Surface *surf = TTF_RenderUTF8_Solid(font, text[i], (SDL_Color){0, 0, 0, 255});
        if (surf == NULL)
        {
            TTF_CloseFont(font);
            printf("TTF rendering error: %s\n", TTF_GetError());
            return NULL;
        }
        SDL_Rect rect = {32, 32*(i+1) + i*16, 736, 47};
        SDL_BlitSurface(surf, NULL, ret_surf, &rect);
    }
    return ret_surf;
} 

void save_mcq(char *file)
{
    IMG_SavePNG(result, file);
}

bool quit_mcq()
{
    SDL_FreeSurface(sign_surface);
    SDL_FreeSurface(result);
    SDL_Quit();
    IMG_Quit();
}

int main(int argc, char *argv[])
{
    init_mcq();

    int pages = (( argc-1 > 7 ? argc : argc-1 )/8) + 1;
    int last_chars = (( argc-1 > 7 ? argc : argc-1 ) % 8) ;
    
        printf("pages: %d\nchars: %d\n", pages, last_chars);
    result = SDL_CreateRGBSurface(0, 800, 400*pages, 32,
                                   rmask, gmask, bmask, amask);
    for(int i = 1; i <= pages; i++)
    {
        SDL_BlitSurface(render_text(&argv[i], pages > 1 ? 7 : last_chars), NULL, result, NULL);
    }
    save_mcq("result.png");
    quit_mcq();
    return 0;
}
