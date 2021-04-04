#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "mcquote.h"

#define MAX_CHARS 8192

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
    if (ret_surf == NULL)
    {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        return NULL;
    }

    SDL_BlitSurface(sign_surface, NULL, ret_surf, NULL);

    for (int i = 0; i < size; i++)
    {
        //text[i][strlen(text[i]) * 4 ] = 0;  /*тут небезапасна чинить нада блять*/
        if(strlen(text[i]) == 0) continue;

        SDL_Surface *surf = TTF_RenderUTF8_Solid(font, text[i], (SDL_Color){0, 0, 0, 255});
        if (surf == NULL)
        {
            TTF_CloseFont(font);
            printf("TTF rendering error: %s\n", TTF_GetError());
            return NULL;
        }
        SDL_Rect rect = {32, 32 * (i + 1) + i * 16, 736, 47};
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
    TTF_CloseFont(font);
    SDL_Quit();
    IMG_Quit();
}
SDL_Surface *empty_surface(int w, int h)
{
    return SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);
}
void render_signs(SDL_Surface *result, int pages, int last_chars, char **lines)
{
    for (int i = 0; i < pages; i++)
    {
        SDL_Surface *surf = NULL;
        surf = render_text(&lines[i * 7], i < pages - 1 ? 7 : last_chars);
        if (surf == NULL)
            return;

        SDL_BlitSurface(surf, NULL, result, &(SDL_Rect){0, i * 400, 800, (i + 1) * 400});
        SDL_FreeSurface(surf);
    }
}
char **parse_text(char *text, int *lines_count, const int maxsize)
{
    char **parsed_text;
    char *char_poiner = text;
    int l_count = 1; 

    while (*char_poiner != 0)
    {
        if (*char_poiner == '\n') 
            {
            l_count++;
            }
        char_poiner++;
    }

    parsed_text = (char **)malloc(l_count * sizeof(char *));

    char_poiner = text;
    parsed_text[0] = text;
    
    int i = 1;
    while (*char_poiner != EOF && *char_poiner != 0)
    {        
        if (*char_poiner == '\n' /*&& char_poiner[1] != '\n'*/)
        {
            *char_poiner = 0;
            parsed_text[i] = char_poiner + 1;
            i++;
        }
        char_poiner++;
    }
    *lines_count = l_count;
    return parsed_text;     
}

int generate_quote(char *message)
{
    int lines_count = 0;
    char **lines = parse_text(message, &lines_count, MAX_CHARS);

    int pages = ceil((double)lines_count / 7.0);
    int last_chars = lines_count - (pages - 1) * 7;

    init_mcq();
    
    result = empty_surface(800, 400 * pages);

    render_signs(result, pages, last_chars, lines);
    free(lines);
    save_mcq("result.png");
    quit_mcq();
    return 0;
}
