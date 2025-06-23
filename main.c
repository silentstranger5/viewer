#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

static float texture_width = 0;
static float texture_height = 0;
static float texture_sx = 1;
static float texture_sy = 1;
static float texture_dx = 0;
static float texture_dy = 0;

static float mouse_x = 0;
static float mouse_y = 0;

static int image_loaded = 0;
static int mouse_down = 0;

static SDL_DialogFileFilter file_filters[] = {
    {"BMP images", "bmp"},
    {"GIF images", "gif"},
    {"PNG images", "png"},
    {"PNM images", "pbm;pgm;ppm"},
    {"JPEG images", "jpg;jpeg"},
    {"SVG images", "svg"},
    {"TGA images", "tga"},
    {"All images", "bmp;gif;png;pbm;pgm;ppm;jpg;jpeg;svg;tga"},
    {"All files", "*"},
};

#define WINDOW_WIDTH    640
#define WINDOW_HEIGHT   480

static void file_callback(void *userdata, const char * const *filelist, int filter)
{
    if (!filelist)
    {
        SDL_Log("Error occured: %s", SDL_GetError());
        return;
    }
    else if (!*filelist)
    {
        return;
    }

    SDL_Surface *surface = IMG_Load(filelist[0]);
    if (!surface)
    {
        SDL_Log("Couldn't load file: %s", SDL_GetError());
        return;
    }

    texture_width = (float) surface->w;
    texture_height = (float) surface->h;
    texture_sx = texture_sy = 1;
    texture_dx = texture_dy = 0;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    SDL_DestroySurface(surface);

    image_loaded = 1;
}

int app_init(int argc, char **argv)
{
    SDL_SetAppMetadata("Image Viewer", "1.0", "com.example.viewer");

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("image viewer", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

int app_event(SDL_Event *event)
{
    switch (event->type)
    {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            if (event->wheel.y < 0)
            {
                texture_sx *= 0.8f;
                texture_sy *= 0.8f;
            }
            else if (event->wheel.y > 0)
            {
                texture_sx *= 1.25f;
                texture_sy *= 1.25f;
            }
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            mouse_x = event->button.x;
            mouse_y = event->button.y;
            mouse_down = 1;
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            mouse_down = 0;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_R)
            {
                texture_sx = texture_sy = 1;
                texture_dx = texture_dy = 0;
            }
            if (event->key.key == SDLK_O && event->key.mod & SDL_KMOD_CTRL)
            {
                SDL_ShowOpenFileDialog(file_callback, NULL, window, file_filters, 9, NULL, false);
            }
            break;
    }
    return SDL_APP_CONTINUE;
}

int app_iterate(void)
{
    SDL_FRect dst_rect;

    if (image_loaded)
    {
        if (mouse_down)
        {
            float x, y;
            SDL_GetMouseState(&x, &y);
            texture_dx += 2 * (x - mouse_x) / texture_sx;
            texture_dy += 2 * (y - mouse_y) / texture_sy;
            mouse_x = x;
            mouse_y = y;
        }

        dst_rect.x = ((float) (WINDOW_WIDTH - texture_width * texture_sx + texture_dx * texture_sx)) / 2.0f;
        dst_rect.y = ((float) (WINDOW_HEIGHT - texture_height * texture_sy + texture_dy * texture_sy)) / 2.0f;
        dst_rect.w = (float) texture_width * texture_sx;
        dst_rect.h = (float) texture_height * texture_sy;
    
        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, NULL, &dst_rect);
        SDL_RenderPresent(renderer);
    }
    else
    {
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_SetRenderScale(renderer, 2.0f, 2.0f);
        SDL_RenderDebugText(renderer, WINDOW_WIDTH / 6.8f, WINDOW_HEIGHT / 4, "CTRL+O to open file");
        SDL_SetRenderScale(renderer, 1.0f, 1.0f);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderPresent(renderer);
    }

    return SDL_APP_CONTINUE;
}

void app_quit(void)
{
    SDL_Quit();
}

int main(int argc, char **argv)
{
    if (app_init(argc, argv) != SDL_APP_CONTINUE)
        return 1;

    SDL_Event e;
    int loop = 1;
    int status = 0;

    while ((status = app_iterate()) == SDL_APP_CONTINUE)
    {
        while (SDL_PollEvent(&e) != 0)
        {
            status = app_event(&e);
            if (status != SDL_APP_CONTINUE)
            {
                loop = 0;
                break;
            }
        }
        if (!loop) break;
    }

    app_quit();

    return !(status == SDL_APP_SUCCESS);
}
