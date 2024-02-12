
#include "sandbox.h"

#include <math.h>

#define UseResCyborg 1
#define UseResHdris 1
#define UseResShaders 1

QuartzResult Sandbox::Init()
{
  QTZ_DEBUG("Sandbox Init");

  // Camera
  // ============================================================

  camera.Get<Transform>()->position = Vec3{ 0.0f, 0.0f, 2.0f};
  Quartz::Camera* c = camera.Add<Quartz::Camera>();
  *c = Quartz::Camera{ .fov = 65.0f, .desiredRatio = 1.0f, .nearClip = 0.1f, .farClip = 100.0f };
  float windowRatio = (float)Quartz::WindowWidth() / (float)Quartz::WindowHeight();
  c->projectionMatrix = ProjectionPerspectiveExtended(windowRatio, c->desiredRatio, c->fov, c->nearClip, c->farClip);

  // Textures
  // ============================================================

  // Albedo ==============================

  QTZ_ATTEMPT(pbrTextures.albedos[0].Init(SANDBOX_RES_DIR "textures/Cerberus/Cerberus_Albedo.png"));
  QTZ_ATTEMPT(pbrTextures.albedos[1].Init(SANDBOX_RES_DIR "textures/AltImage.png"));

  uint32_t customAlbedoIndex = pbrTextures.albedoCount - 1;
  pbrTextures.albedos[customAlbedoIndex].extents = Vec2U{ 1, 1 };
  QTZ_ATTEMPT(pbrTextures.albedos[customAlbedoIndex].Init({ Vec3{ 1.0f, 1.0f, 1.0f } }));

  object.pCurrentAlbedo = &pbrTextures.albedos[0];

  // Normal ==============================

  QTZ_ATTEMPT(pbrTextures.normals[0].Init(SANDBOX_RES_DIR "textures/Cerberus/Cerberus_Normal.png"));
  QTZ_ATTEMPT(pbrTextures.normals[1].Init(SANDBOX_RES_DIR "textures/TestNormal.png"));
  QTZ_ATTEMPT(pbrTextures.normals[2].Init(SANDBOX_RES_DIR "textures/EmptyNormal.png"));

  object.pCurrentNormal = &pbrTextures.normals[0];

  // Maps ==============================

  QTZ_ATTEMPT(pbrTextures.maps[0].Init(SANDBOX_RES_DIR "textures/Cerberus/Cerberus_AoRoughMetal.png"));

  uint32_t customMapIndex = pbrTextures.mapCount - 1;
  pbrTextures.maps[customMapIndex].extents = Vec2U{ 1, 1 };
  QTZ_ATTEMPT(pbrTextures.maps[customMapIndex].Init({ Vec3{ 1.0f, 0.0f, 0.0f } }));

  object.pCurrentMaps = &pbrTextures.maps[0];

  // Skyboxes ==============================

  const char* skyboxTexturePaths[] = {
    SANDBOX_RES_DIR "textures/hdri/whipple_creek_regional_park_04_4k.exr",
    SANDBOX_RES_DIR "textures/hdri/studio_small_08_4k.exr",
    //SANDBOX_RES_DIR "textures/hdri/industrial_sunset_02_puresky_4k.exr",
    //SANDBOX_RES_DIR "textures/hdri/lilienstein_4k.exr",
    //SANDBOX_RES_DIR "textures/hdri/fireplace_4k.exr",
    //SANDBOX_RES_DIR "textures/hdri/peppermint_powerplant_2_4k.exr",
    //SANDBOX_RES_DIR "textures/hdri/autoshop_01_4k.exr",
    //SANDBOX_RES_DIR "textures/hdri/artist_workshop_4k.exr",
    //SANDBOX_RES_DIR "textures/hdri/glass_passage_4k.exr"
  };

  skyboxTextures.resize(sizeof(skyboxTexturePaths) / sizeof(*skyboxTexturePaths));
  for (uint32_t i = 0; i < skyboxTextures.size(); i++)
  {
    QTZ_ATTEMPT(skyboxTextures[i].Init(skyboxTexturePaths[i]));
  }

  skybox.pCurrentTexture = &skyboxTextures[0];
  QTZ_ATTEMPT(skybox.buffer.Init(sizeof(skybox.bufferData)));
  skybox.buffer.PushData(&skybox.bufferData);

  // Meshes
  // ============================================================

  const char* objectMeshPaths[] = {
    SANDBOX_RES_DIR "models/Cerberus.obj",
    SANDBOX_RES_DIR "models/SphereSmooth.obj",
    SANDBOX_RES_DIR "models/Cube.obj",
  };

  object.meshes.resize(sizeof(objectMeshPaths) / sizeof(*objectMeshPaths));
  for (uint32_t i = 0; i < object.meshes.size(); i++)
  {
    QTZ_ATTEMPT(object.meshes[i].Init(objectMeshPaths[i]));
  }

  std::vector<Quartz::Vertex> verts(4);
  verts[0].position = Vec3{ -1.0f, -1.0f, 0.0f }; // TL
  verts[1].position = Vec3{  1.0f, -1.0f, 0.0f }; // TR
  verts[2].position = Vec3{ -1.0f,  1.0f, 0.0f }; // BL
  verts[3].position = Vec3{  1.0f,  1.0f, 0.0f }; // BR

  std::vector<uint32_t> indices = {
    0, 1, 2,
    2, 3, 1
  };

  QTZ_ATTEMPT(skybox.mesh.Init(verts, indices));

  // Materials
  // ============================================================

  QTZ_ATTEMPT(
    object.material.Init(
      {
        SANDBOX_RES_DIR "shaders/compiled/0_blank.vert.spv",
        SANDBOX_RES_DIR "shaders/compiled/4_ibl.frag.spv"
      },
      {
        { .type = Quartz::Input_Texture, .value = { .texture = object.pCurrentAlbedo                  } },
        { .type = Quartz::Input_Texture, .value = { .texture = object.pCurrentNormal                  } },
        { .type = Quartz::Input_Texture, .value = { .texture = object.pCurrentMaps                    } },
        { .type = Quartz::Input_Texture, .value = { .texture = &skybox.pCurrentTexture->GetDiffuse()  } },
        { .type = Quartz::Input_Texture, .value = { .texture = &skybox.pCurrentTexture->GetSpecular() } },
        { .type = Quartz::Input_Texture, .value = { .texture = &skybox.pCurrentTexture->GetBrdf()     } },
        { .type = Quartz::Input_Buffer , .value = { .buffer  = &skybox.buffer                         } }
      }
    )
  );

  QTZ_ATTEMPT(
    skybox.material.Init(
      {
        SANDBOX_RES_DIR "shaders/compiled/s_skybox.vert.spv",
        SANDBOX_RES_DIR "shaders/compiled/s_skybox.frag.spv"
      },
      {
        { .type = Quartz::Input_Texture, .value = { .texture = &skybox.pCurrentTexture->GetBase() } },
        { .type = Quartz::Input_Buffer , .value = { .buffer  = &skybox.buffer                     } }
      },
      Quartz::Pipeline_Cull_None | Quartz::Pipeline_Depth_Compare_LessEqual
    )
  );

  // Entities
  // ============================================================

  Quartz::Renderable* r;

  r = object.entity.Add<Quartz::Renderable>();
  r->material = &object.material;
  r->mesh = &object.meshes[0];

  object.entity.Get<Transform>()->rotation.y = 90.0f;

  r = skybox.entity.Add<Quartz::Renderable>();
  r->material = &skybox.material;
  r->mesh = &skybox.mesh;

  return Quartz_Success;
}
