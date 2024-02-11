
#include "sandbox.h"

QUARTZ_ENTRY(Sandbox)

void Sandbox::Shutdown()
{
  for (uint32_t i = 0; i < objectCount; i++)
  {
    pbrResources.materials[i].Shutdown();
    pbrResources.textures.maps[i].Shutdown();
    pbrResources.buffers.buffers[i].Shutdown();

    pbrResources.textures.maps[i].Shutdown();
  }
  pbrResources.materialBase.Shutdown();
  pbrResources.textures.albedo.Shutdown();
  pbrResources.textures.normal.Shutdown();
  pbrResources.textures.sky.Shutdown();

  skybox.mesh.Shutdown();
  skybox.material.Shutdown();

  testInfo.material.Shutdown();
  testInfo.mesh.Shutdown();
  testInfo.texture.Shutdown();
  testInfo.buffer.Shutdown();

  objectMesh.Shutdown();

  for (uint32_t i = 0; i < lightbulbCount; i++)
  {
    lights.buffers[i].Shutdown();
    lights.materials[i].Shutdown();
  }
  lights.materialBase.Shutdown();
  lights.mesh.Shutdown();

  QTZ_DEBUG("Sandbox shutdown");
}
