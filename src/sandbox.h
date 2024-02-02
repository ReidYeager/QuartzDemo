#pragma once

#include <quartz.h>

class Sandbox : public Quartz::Application
{
private:

  // Objects
  // ============================================================

  Quartz::Mesh objectMesh;

  static const uint32_t axisCount = 7;
  static const uint32_t objectCount = axisCount * axisCount;
  Quartz::Entity objects[objectCount] = {};

  // Pbr
  // ============================================================

  struct PbrBufferInfo
  {
    Vec3 baseReflectivity;
    float roughness;
    float metalness;
  };

  struct
  {
    struct
    {
      Quartz::Texture albedo;
      Quartz::Texture normal;
      Quartz::Texture maps[objectCount];
    } textures;

    struct
    {
      Quartz::Buffer buffers[objectCount];
      PbrBufferInfo data[objectCount];
    } buffers;

    Quartz::Material materialBase;
    Quartz::Material materials[objectCount];

  } pbrResources;

  // Lights
  // ============================================================

  static const uint32_t pointLightCount = 4; //QTZ_LIGHT_POINT_MAX_COUNT;
  static const uint32_t spotLightCount = 2; //QTZ_LIGHT_SPOT_MAX_COUNT;
  static const uint32_t lightbulbCount = pointLightCount + spotLightCount;

  // For visualizing point/spot lights' positions
  struct
  {
    Quartz::Mesh     mesh;
    Quartz::Material materialBase;
    Quartz::Material materials[lightbulbCount];
    Quartz::Buffer   buffers  [lightbulbCount];

    Transform* transforms[lightbulbCount];
    struct
    {
      Quartz::LightDirectional* dir;
      Quartz::LightPoint* point[pointLightCount];
      Quartz::LightSpot* spot[spotLightCount];
    } pointers;

    Quartz::Entity directional;
    Quartz::Entity points[pointLightCount];
    Quartz::Entity spots[spotLightCount];

  } lights;

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

private:
  QuartzResult InitResources();
  QuartzResult InitObjects();
  QuartzResult InitLights();

  QuartzResult UpdateCamera();

};
