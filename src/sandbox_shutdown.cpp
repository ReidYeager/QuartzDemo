
#include "sandbox.h"

void Sandbox::Shutdown()
{
  // Lights ==============================

  for (uint32_t i = 0; i < m_pointCount; i++)
  {
    m_points[i].buffer.Shutdown();
    m_points[i].materialInstance.Shutdown();
  }

  for (uint32_t i = 0; i < m_spotCount; i++)
  {
    m_spots[i].buffer.Shutdown();
    m_spots[i].materialInstance.Shutdown();
  }

  m_lightResources.materialBase.Shutdown();
  m_lightResources.pointMesh.Shutdown();
  m_lightResources.spotMesh.Shutdown();

  // Textures ==============================

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

  // Skybox ==============================

  for (uint32_t i = 0; i < skyboxTextures.size(); i++)
  {
    skyboxTextures[i].Shutdown();
  }
  skyboxTextures.clear();

  skybox.buffer.Shutdown();
  skybox.material.Shutdown();
  skybox.mesh.Shutdown();

  // Object ==============================

  object.material.Shutdown();
  for (uint32_t i = 0; i < object.meshes.size(); i++)
  {
    object.meshes[i].Shutdown();
  }
  object.meshes.clear();

  QTZ_DEBUG("Sandbox shutdown");
}