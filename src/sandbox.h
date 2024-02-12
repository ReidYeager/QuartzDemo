#pragma once

#include <quartz.h>

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#ifdef QTZ_DEBUG
#define SANDBOX_RES_DIR "D:/Dev/QuartzSandbox/res/"
#else
#define SANDBOX_RES_DIR "./"
#endif

class Sandbox : public Quartz::Application
{
private:

  // Object
  // ============================================================

  struct
  {
    std::vector<Quartz::Mesh> meshes;
    Quartz::Material material;
    Quartz::Entity entity;

    Quartz::Texture* pCurrentAlbedo;
    Quartz::Texture* pCurrentNormal;
    Quartz::Texture* pCurrentMaps;
  } object;

  // Skybox
  // ============================================================

  struct
  {
    Quartz::TextureSkybox* pCurrentTexture;
    Quartz::Buffer buffer;

    Quartz::Mesh mesh;
    Quartz::Material material;
    Quartz::Entity entity;

    struct
    {
      float gamma = 1.5f;
      float exposure = 1.0f;
    } bufferData;
  } skybox;

  // Textures
  // ============================================================

  struct
  {
    const uint32_t albedoCount = 3;
    Quartz::Texture albedos[3];

    const uint32_t normalCount = 3;
    Quartz::Texture normals[3];

    const uint32_t mapCount = 2;
    Quartz::Texture maps[2];
  } pbrTextures;

  std::vector<Quartz::TextureSkybox> skyboxTextures;

  // Camera
  // ============================================================

  Quartz::Entity camera;
  bool useOrbitCam = true;
  float cameraArmLength = 2.0f;
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
  void UpdateCameraOrbit();
  void UpdateCameraFly();

  FILE* outDumpFile;
  bool StartDump();
  void DumpTexture(const char* name, const Quartz::Texture& texture);
  void DumpFile(const char* name, const char* path);
  void DumpByteArray(const char* name, uint64_t byteCount, void* bytes);
  void DumpText(const char* string);
  void EndDump();

};
