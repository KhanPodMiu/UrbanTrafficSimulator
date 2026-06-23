// #include "SDL2/SDL.h"
// #include "SDL2/SDL_image.h"
// #include <iostream>
// #include "core/renderWindow.hpp"

// int main(int argc, char* args[]) {

//     SDL_Init(SDL_INIT_EVERYTHING);

//     RenderWindow* window = new RenderWindow("Urban Traffic", 1600, 900);

//     SDL_Texture* MapBackground = window -> loadTexture("assets/maps/background.png");

//     if(MapBackground == nullptr){
//         std::cerr << "Hey.. recheck the IMG PATH" << SDL_GetError();
//         return 1;
//     }

//     bool is_game_running = true;

//     SDL_Event event;

//     while(is_game_running){
//         while(SDL_PollEvent(&event)){
//             if(event.type == SDL_QUIT){
//                 is_game_running = false;
//             }
//         }

//         window -> clear();
//         window -> render(MapBackground);
//         window -> display();

//     }

//     window -> cleanUp();

//     window -> cleanUpTexture(MapBackground);

//     SDL_Quit();
//     return 0;
// }

int main() {
    return 0;
}