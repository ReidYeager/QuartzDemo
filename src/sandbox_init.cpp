
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

  camera.Get<Transform>()->position = Vec3{ 0.0f, 0.0f, 7.0f};
  Quartz::Camera* c = camera.Add<Quartz::Camera>();
  *c = Quartz::Camera{ .fov = 65.0f, .desiredRatio = 1.0f, .nearClip = 0.1f, .farClip = 100.0f };
  float windowRatio = (float)Quartz::WindowWidth() / (float)Quartz::WindowHeight();
  c->projectionMatrix = ProjectionPerspectiveExtended(windowRatio, c->desiredRatio, c->fov, c->nearClip, c->farClip);

  // Textures
  // ============================================================

  QTZ_ATTEMPT(pbrTextures.albedo.Init("D:/Dev/QuartzSandbox/res/textures/Cerberus/Cerberus_Albedo.png"));
  QTZ_ATTEMPT(pbrTextures.normal.Init("D:/Dev/QuartzSandbox/res/textures/Cerberus/Cerberus_Normal.png"));
  QTZ_ATTEMPT(pbrTextures.maps.Init  ("D:/Dev/QuartzSandbox/res/textures/Cerberus/Cerberus_AoRoughMetal.png"));

  const char* skyboxTexturePaths[] = {
    "D:/Dev/QuartzSandbox/res/textures/hdri/whipple_creek_regional_park_04_4k.exr",
    "D:/Dev/QuartzSandbox/res/textures/hdri/studio_small_08_4k.exr",
    "D:/Dev/QuartzSandbox/res/textures/hdri/industrial_sunset_02_puresky_4k.exr",
    "D:/Dev/QuartzSandbox/res/textures/hdri/lilienstein_4k.exr",
    "D:/Dev/QuartzSandbox/res/textures/hdri/fireplace_4k.exr",
    "D:/Dev/QuartzSandbox/res/textures/hdri/peppermint_powerplant_2_4k.exr",
    "D:/Dev/QuartzSandbox/res/textures/hdri/autoshop_01_4k.exr",
    "D:/Dev/QuartzSandbox/res/textures/hdri/artist_workshop_4k.exr",
    "D:/Dev/QuartzSandbox/res/textures/hdri/glass_passage_4k.exr"
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

  QTZ_ATTEMPT(object.mesh.Init("D:/Dev/QuartzSandbox/res/models/Cerberus.obj"));

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
        "D:/Dev/QuartzSandbox/res/shaders/compiled/0_blank.vert.spv",
        "D:/Dev/QuartzSandbox/res/shaders/compiled/4_ibl.frag.spv"
      },
      {
        { .type = Quartz::Input_Texture, .value = { .texture = &pbrTextures.albedo                    } },
        { .type = Quartz::Input_Texture, .value = { .texture = &pbrTextures.normal                    } },
        { .type = Quartz::Input_Texture, .value = { .texture = &pbrTextures.maps                      } },
        { .type = Quartz::Input_Texture, .value = { .texture = &skybox.pCurrentTexture->GetDiffuse()  } },
        { .type = Quartz::Input_Texture, .value = { .texture = &skybox.pCurrentTexture->GetSpecular() } },
        { .type = Quartz::Input_Texture, .value = { .texture = &skybox.pCurrentTexture->GetBrdf()     } },
        {.type = Quartz::Input_Buffer , .value = {.buffer = &skybox.buffer                            } }
      }
    )
  );

  QTZ_ATTEMPT(
    skybox.material.Init(
      {
        "D:/Dev/QuartzSandbox/res/shaders/compiled/s_skybox.vert.spv",
        "D:/Dev/QuartzSandbox/res/shaders/compiled/s_skybox.frag.spv"
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
  r->mesh = &object.mesh;

  object.entity.Get<Transform>()->rotation.y = 90.0f;

  r = skybox.entity.Add<Quartz::Renderable>();
  r->material = &skybox.material;
  r->mesh = &skybox.mesh;

  return Quartz_Success;
}
