#include "vulkan/engine.hpp"
#include "window.hpp"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>
#include <format>
#include <stdexcept>


auto main() -> int 
{
  if(!SDL_Init(SDL_INIT_VIDEO))
    throw std::runtime_error(std::format("Failed to init video. [{}],", SDL_GetError()));
  Window::Instance window{};

  Vulkan::Engine engine{};
  engine.init();
  
  bool done {false};

  while(!done)
  {
    SDL_Event event; 

    while (SDL_PollEvent(&event)) 
    {
      if(event.type == SDL_EVENT_QUIT)
        done = true;
    }

    engine.draw();
  }

  return 0;
}
