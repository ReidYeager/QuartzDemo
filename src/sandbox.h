#pragma once

#include <quartz.h>

class Sandbox : public Quartz::Application
{
private:

  // Objects
  // ============================================================

  Quartz::Mesh objectMesh;

  static const uint32_t objectCount = 25;
  Quartz::Entity objects[objectCount] = {};

  // Pbr
  // ============================================================

  struct
  {
    Quartz::Texture albedo;
    Quartz::Texture normal;
    Quartz::Texture maps;
  } pbrTextures;

  struct PbrBufferInfo
  {
    Vec3 baseReflectivity;
    float roughness;
    float metalness;
  };

  Quartz::Material pbrMaterial;

  Quartz::Material pbrInstances[objectCount] = {};
  Quartz::Buffer pbrBuffers[objectCount] = {};
  PbrBufferInfo pbrBufferData[objectCount] = {};

  // Lights
  // ============================================================

  // For visualizing point/spot lights' positions
  Quartz::Mesh     lightbulbMesh;
  Quartz::Material lightbulbMaterial;

  static const uint32_t pointLightCount = 4; //QTZ_LIGHT_POINT_MAX_COUNT;
  static const uint32_t spotLightCount = 2; //QTZ_LIGHT_SPOT_MAX_COUNT;
  static const uint32_t lightbulbCount = pointLightCount + spotLightCount;
  Quartz::Material lightbulbInstances[lightbulbCount] = {};
  Quartz::Buffer   lightbulbBuffers[lightbulbCount] = {};
  Transform* lightbulbTransforms[lightbulbCount] = {};

  Quartz::Entity dirLight;
  Quartz::Entity pointLights[pointLightCount] = {};
  Quartz::Entity spotLights[spotLightCount] = {};

  Quartz::LightDirectional* dirLightPtr;
  Quartz::LightPoint* pointLightPtr[pointLightCount] = {};
  Quartz::LightSpot* spotLightPtr[spotLightCount] = {};

  // Camera
  // ============================================================

  Quartz::Entity camera;
  float cameraSpeed = 4.0f;
  float speedShiftMultiplier = 2.0f;
  float speedAltMultiplier = 0.5f;

  // Declarations
  // ============================================================
public:
  ~Sandbox() {}

  QuartzResult Init()                   override;
  QuartzResult Update(double deltaTime) override;
  void         Shutdown()               override;
  void         RenderImgui()            override;
};
