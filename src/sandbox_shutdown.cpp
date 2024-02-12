
#include "sandbox.h"

void Sandbox::Shutdown()
{
  for (uint32_t i = 0; i < pbrTextures.albedoCount; i++)
  {
    pbrTextures.albedos[i].Shutdown();
  }

  for (uint32_t i = 0; i < pbrTextures.normalCount; i++)
  {
    pbrTextures.normals[i].Shutdown();
  }

  for (uint32_t i = 0; i < pbrTextures.mapCount; i++)
  {
    pbrTextures.maps[i].Shutdown();
  }

  for (uint32_t i = 0; i < skyboxTextures.size(); i++)
  {
    skyboxTextures[i].Shutdown();
  }
  skyboxTextures.clear();

  skybox.buffer.Shutdown();
  skybox.material.Shutdown();
  skybox.mesh.Shutdown();

  object.material.Shutdown();
  for (uint32_t i = 0; i < object.meshes.size(); i++)
  {
    object.meshes[i].Shutdown();
  }
  object.meshes.clear();

  QTZ_DEBUG("Sandbox shutdown");
}