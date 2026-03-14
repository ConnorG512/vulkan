#include "vulkan/engine.hpp"
#include <SDL3/SDL_events.h>

auto main() -> int 
{
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
