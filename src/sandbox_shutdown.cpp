
#include "sandbox.h"

void Sandbox::Shutdown()
{
  pbrTextures.albedo.Shutdown();
  pbrTextures.normal.Shutdown();
  pbrTextures.maps.Shutdown();

  for (uint32_t i = 0; i < skyboxTextures.size(); i++)
  {
    skyboxTextures[i].Shutdown();
  }
  skyboxTextures.clear();

  skybox.buffer.Shutdown();
  skybox.material.Shutdown();
  skybox.mesh.Shutdown();

  object.material.Shutdown();
  object.mesh.Shutdown();

  QTZ_DEBUG("Sandbox shutdown");
}