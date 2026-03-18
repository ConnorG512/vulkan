#include "vulkan/engine.hpp"
#include "window.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_vulkan.h"


#include <SDL3/SDL_events.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_oldnames.h>
#include <format>
#include <stdexcept>


auto main() -> int 
{
  if(!SDL_Init(SDL_INIT_VIDEO))
    throw std::runtime_error(std::format("Failed to init video. [{}],", SDL_GetError()));
  Window::Instance window{};

  Vulkan::Engine engine{};
  engine.init(window);
  
  bool done {false};
  bool stop_rendering {false};

  while(!done)
  {
    SDL_Event event; 

    while (SDL_PollEvent(&event)) 
    {
      if(event.type == SDL_EVENT_QUIT)
        done = true;

      if(event.type == SDL_EVENT_WINDOW_MINIMIZED) {
        stop_rendering = true;
      }
      
      if(event.type == SDL_EVENT_WINDOW_RESTORED) {
        stop_rendering = false;
      }
    }
    //ImGui_ImplVulkan_NewFrame();
    //ImGui_ImplSDL3_NewFrame();
    //ImGui::NewFrame();
    
    //ImGui::ShowDemoWindow();
    //ImGui::Render();
    
    engine.draw();
  }

  return 0;
}
