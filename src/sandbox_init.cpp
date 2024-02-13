
#include "sandbox.h"

#include <math.h>

#define UseResCyborg 1
#define UseResHdris 1
#define UseResShaders 1

QuartzResult Sandbox::Init()
{
  QTZ_DEBUG("Sandbox Init");

  Quartz::Buffer dummyBuffer;
  dummyBuffer.Init(1);

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
    //SANDBOX_RES_DIR "textures/hdri/whipple_creek_regional_park_04_4k.exr",
    //SANDBOX_RES_DIR "textures/hdri/studio_small_08_4k.exr",
    //SANDBOX_RES_DIR "textures/hdri/industrial_sunset_02_puresky_4k.exr",
    //SANDBOX_RES_DIR "textures/hdri/lilienstein_4k.exr",
    //SANDBOX_RES_DIR "textures/hdri/fireplace_4k.exr",
    //SANDBOX_RES_DIR "textures/hdri/peppermint_powerplant_2_4k.exr",
    //SANDBOX_RES_DIR "textures/hdri/autoshop_01_4k.exr",
    SANDBOX_RES_DIR "textures/hdri/artist_workshop_4k.exr",
    SANDBOX_RES_DIR "textures/hdri/glass_passage_4k.exr"
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

  // Lights
  // ============================================================

  // Resources ==============================

  QTZ_ATTEMPT(m_lightResources.pointMesh.Init(SANDBOX_RES_DIR "models/SphereSmooth.obj"));
  QTZ_ATTEMPT(m_lightResources.spotMesh.Init(SANDBOX_RES_DIR "models/SpotlightCone.obj"));
  QTZ_ATTEMPT(
    m_lightResources.materialBase.Init(
      {
        SANDBOX_RES_DIR "shaders/compiled/0_blank.vert.spv",
        SANDBOX_RES_DIR "shaders/compiled/x_light.frag.spv"
      },
      {
        { .type = Quartz::Input_Buffer, .value = { .buffer = &dummyBuffer } }
      }
    )
  );

  // Directional ==============================

  m_directional.entity.Add<Quartz::LightDirectional>();
  m_directional.light = m_directional.entity.Get<Quartz::LightDirectional>();
  m_directional.light->color = Vec3{ 1.0f, 1.0f, 1.0f };
  m_directional.light->direction = Vec3{ 0.0f, -1.0f, 0.0f };
  m_directional.light->intensity = 1.0f;
  m_directional.entity.Disable();
  m_directional.enabled = false;

  // Points ==============================

  Vec3 pointColors[4] = {
    Vec3{ 1.0f, 0.0f, 0.0f },
    Vec3{ 0.0f, 1.0f, 0.0f },
    Vec3{ 0.0f, 0.0f, 1.0f },
    Vec3{ 0.0f, 1.0f, 1.0f }
  };

  // Add all components
  for (uint32_t i = 0; i < m_pointCount; i++)
  {
    SandboxPointLight& p = m_points[i];

    QTZ_ATTEMPT(p.buffer.Init(sizeof(Vec3)));

    QTZ_ATTEMPT(
      p.materialInstance.Init(
        m_lightResources.materialBase,
        {
          { .buffer = &p.buffer }
        }
      )
    );

    Quartz::Renderable* r = p.entity.Add<Quartz::Renderable>();
    r->mesh = &m_lightResources.pointMesh;
    r->material = &p.materialInstance;

    Quartz::LightPoint* light = p.entity.Add<Quartz::LightPoint>();
    light->color = pointColors[i];
    light->intensity = 1.0f;
    light->position = Vec3{ 0.0f, 0.0f, 0.0f };

    QTZ_ATTEMPT(p.buffer.PushData(&light->color));

    p.entity.Disable();
    p.enabled = false;
  }

  // Get final component pointers
  for (uint32_t i = 0; i < m_pointCount; i++)
  {
    SandboxPointLight& p = m_points[i];
    p.transform = p.entity.Get<Transform>();
    p.light = p.entity.Get<Quartz::LightPoint>();

    p.transform->scale *= 0.04f;
  }

  // Spots ==============================

  for (uint32_t i = 0; i < m_spotCount; i++)
  {
    SandboxSpotLight& s = m_spots[i];

    QTZ_ATTEMPT(s.buffer.Init(sizeof(Vec3)));

    QTZ_ATTEMPT(
      s.materialInstance.Init(
        m_lightResources.materialBase,
        {
          { .buffer = &s.buffer }
        }
      )
    );

    Quartz::Renderable* r = s.entity.Add<Quartz::Renderable>();
    r->mesh = &m_lightResources.spotMesh;
    r->material = &s.materialInstance;

    Quartz::LightSpot* light = s.entity.Add<Quartz::LightSpot>();
    light->color = pointColors[3 - i];
    light->intensity = 1.0f;
    light->inner = 0.0f;
    light->position = Vec3{ 0.0f, 0.0f, 0.0f };
    light->direction = Vec3{ 0.0f, 0.0f, -1.0f };
    light->inner = cos(PERI_DEGREES_TO_RADIANS(s.innerDegrees));
    light->outer = cos(PERI_DEGREES_TO_RADIANS(s.outerDegrees));

    QTZ_ATTEMPT(s.buffer.PushData(&light->color));

    s.entity.Disable();
    s.enabled = false;
  }

  for (uint32_t i = 0; i < m_spotCount; i++)
  {
    SandboxSpotLight& s = m_spots[i];
    s.transform = s.entity.Get<Transform>();
    s.light = s.entity.Get<Quartz::LightSpot>();

    s.transform->scale *= 0.04f;
  }

  // Shutdown
  // ============================================================

  dummyBuffer.Shutdown();
  return Quartz_Success;
}
