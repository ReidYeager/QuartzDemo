#pragma once

#include <quartz.h>

#include <stdio.h>
#include <stdlib.h>
#include <vector>

class Sandbox : public Quartz::Application
{
private:

  // Object
  // ============================================================

  struct
  {
    Quartz::Mesh mesh;
    Quartz::Material material;
    Quartz::Entity entity;
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
    Quartz::Texture albedo;
    Quartz::Texture normal;
    Quartz::Texture maps;
  } pbrTextures;

  std::vector<Quartz::TextureSkybox> skyboxTextures;

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
  QuartzResult UpdateCamera();

  FILE* outDumpFile;
  bool StartDump();
  void DumpTexture(const char* name, const Quartz::Texture& texture);
  void DumpFile(const char* name, const char* path);
  void DumpByteArray(const char* name, uint64_t byteCount, void* bytes);
  void DumpText(const char* string);
  void EndDump();

};
