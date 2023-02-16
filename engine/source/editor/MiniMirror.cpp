#include <iostream>

#include "editor/MiniMirror.h"
int main() {
  /*auto config_system = MM::ConfigSystem::ConfigSystem::GetInstance();
  for (const auto& a: config_system->GetAllConfig()) {
    std::cout << a.first << "\t" << a.second << std::endl;
  }*/
  MM::RenderSystem::RenderEngine render_system;
  render_system.Init();
  render_system.Run();
  render_system.CleanUp();
  const MM::RenderSystem::RenderEngine* ptr = new MM::RenderSystem::RenderEngine();
  delete ptr;
}