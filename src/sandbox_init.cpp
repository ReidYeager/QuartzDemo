
#include "sandbox.h"

QuartzResult Sandbox::Init()
{
  QTZ_DEBUG("Sandbox Init");

  QTZ_ATTEMPT(InitResources());
  QTZ_ATTEMPT(InitObjects());
  QTZ_ATTEMPT(InitLights());

  // Camera
  // ============================================================

  camera.Get<Transform>()->position = Vec3{ float(axisCount / 2), float(axisCount / 2), 7.0f};
  Quartz::Camera* c = camera.Add<Quartz::Camera>();
  *c = Quartz::Camera{ .fov = 65.0f, .desiredRatio = 1.0f, .nearClip = 0.1f, .farClip = 100.0f };
  float windowRatio = (float)Quartz::WindowWidth() / (float)Quartz::WindowHeight();
  c->projectionMatrix = ProjectionPerspectiveExtended(windowRatio, c->desiredRatio, c->fov, c->nearClip, c->farClip);

  return Quartz_Success;
}

QuartzResult Sandbox::InitResources()
{
  Quartz::Buffer dummyBuffer(1);

  // Textures
  // ============================================================

  //QTZ_ATTEMPT(pbrResources.textures.albedo.Init("D:/Dev/QuartzSandbox/res/textures/CyborgWeapon/Weapon_albedo.png"));
  QTZ_ATTEMPT(pbrResources.textures.albedo.Init(1, 1, { Vec3{ 1.0f, 1.0f, 1.0f } }));

  //QTZ_ATTEMPT(pbrResources.textures.normal.Init("D:/Dev/QuartzSandbox/res/textures/CyborgWeapon/Weapon_normal.png"));
  //QTZ_ATTEMPT(pbrResources.textures.normal.Init("D:/Dev/QuartzSandbox/res/textures/TestNormal.png"));
  QTZ_ATTEMPT(pbrResources.textures.normal.Init(1, 1, { Vec3{ 0.5f, 0.5f, 1.0f } }));

  //QTZ_ATTEMPT(pbrResources.textures.maps.Init("D:/Dev/QuartzSandbox/res/textures/CyborgWeapon/Weapon_AoRoughnessMetallic.png"));
  for (uint32_t y = 0; y < axisCount; y++)
  for (uint32_t x = 0; x < axisCount; x++)
  {
    uint32_t index = (y * axisCount) + x;
    float offset = (0.5f / axisCount);
    pbrResources.buffers.buffers[index].Init(sizeof(PbrBufferInfo));
    QTZ_ATTEMPT(pbrResources.textures.maps[index].Init(1, 1, { Vec3{1.0f, ((float)x / axisCount) + offset, ((float)y / axisCount) + offset } }));
  }

  // Material
  // ============================================================

  QTZ_ATTEMPT(
    pbrResources.materialBase.Init(
      { "D:/Dev/QuartzSandbox/res/shaders/compiled/0_blank.vert.spv",
      "D:/Dev/QuartzSandbox/res/shaders/compiled/3_pbr.frag.spv" },
      //"D:/Dev/QuartzSandbox/res/shaders/compiled/4_ibl.frag.spv" },
      { {.type = Quartz::Input_Buffer,  .value = {.buffer = dummyBuffer          } },
        {.type = Quartz::Input_Texture, .value = {.texture = pbrResources.textures.albedo  } },
        {.type = Quartz::Input_Texture, .value = {.texture = pbrResources.textures.normal  } },
        {.type = Quartz::Input_Texture, .value = {.texture = pbrResources.textures.maps[0] } }
      })
    );

  // Mesh
  // ============================================================

  //QTZ_ATTEMPT(objectMesh.Init("D:/Dev/QuartzSandbox/res/models/Cyborg_Weapon.obj"));
  QTZ_ATTEMPT(objectMesh.Init("D:/Dev/QuartzSandbox/res/models/SphereSmooth.obj"));
  //QTZ_ATTEMPT(objectMesh.Init("D:/Dev/QuartzSandbox/res/models/sponza/sponza.obj"));
  //QTZ_ATTEMPT(objectMesh.InitFromDump("D:/Dev/QuartzSandbox/res/models/sponza/sponza.dmp"));

  dummyBuffer.Shutdown();
  return Quartz_Success;
}

QuartzResult Sandbox::InitObjects()
{
  for (uint32_t i = 0; i < objectCount; i++)
  {
    float y = (float)(i / axisCount);
    float x = (float)(i % axisCount);

    Quartz::Entity& e = objects[i];

    Transform* t = e.Get<Transform>();
    t->position = Vec3{ x, y, 0.0f };
    t->scale = Vec3{ 0.5f, 0.5f, 0.5f };

    pbrResources.materials[i].Init(
      pbrResources.materialBase,
      {
        { .buffer = pbrResources.buffers.buffers[i] },
        { .texture = pbrResources.textures.albedo   },
        { .texture = pbrResources.textures.normal   },
        { .texture = pbrResources.textures.maps[i]  }
      });

    pbrResources.buffers.data[i].baseReflectivity = Vec3{ 0.56f, 0.57f, 0.58f };
    pbrResources.buffers.data[i].roughness = (float)x / axisCount + 0.1f;
    pbrResources.buffers.data[i].metalness = (float)y / axisCount + 0.1f;
    pbrResources.buffers.buffers[i].PushData(&pbrResources.buffers.data[i]);

    *e.Add<Quartz::Renderable>() = Quartz::Renderable{ .mesh = &objectMesh, .material = &pbrResources.materials[i] };
  }

  return Quartz_Success;
}

QuartzResult Sandbox::InitLights()
{
  Quartz::Buffer dummyBuffer(1);

  // Resources
  // ============================================================

  QTZ_ATTEMPT(lights.mesh.Init("D:/Dev/QuartzSandbox/res/models/SphereSmooth.obj"));
  QTZ_ATTEMPT(lights.materialBase.Init(
    { "D:/Dev/QuartzSandbox/res/shaders/compiled/0_blank.vert.spv",
      "D:/Dev/QuartzSandbox/res/shaders/compiled/x_light.frag.spv" },
    { {.type = Quartz::Input_Buffer, .value = {.buffer = dummyBuffer } } }
  ));

  // Directional
  // ============================================================

  lights.pointers.dir = lights.directional.Add<Quartz::LightDirectional>();
  lights.pointers.dir->color = Vec3{ 1.0f, 1.0f, 1.0f };
  lights.pointers.dir->intensity = 1.0f;
  lights.pointers.dir->direction = Vec3{ 0.0f, 0.0f, -1.0f };

  // Point
  // ============================================================

  for (uint32_t i = 0; i < pointLightCount; i++)
  {
    lights.buffers[i].Init(sizeof(Vec3));
    lights.materials[i].Init(lights.materialBase, { {.buffer = lights.buffers[i] } });
    *lights.points[i].Add<Quartz::Renderable>() = Quartz::Renderable{ .mesh = &lights.mesh, .material = &lights.materials[i] };
    lights.points[i].Add<Quartz::LightPoint>();
  }

  Vec3 colors[4] = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f },
  };

  for (uint32_t i = 0; i < pointLightCount; i++)
  {
    lights.pointers.point[i] = lights.points[i].Get<Quartz::LightPoint>();
    lights.pointers.point[i]->color = colors[i];
    lights.pointers.point[i]->intensity = 0.0f;
    lights.buffers[i].PushData(&lights.pointers.point[i]->color);

    lights.transforms[i] = lights.points[i].Get<Transform>();
    lights.transforms[i]->scale = Vec3{ 0.1f, 0.1f, 0.1f };
    //lights.transforms[i]->position = Vec3{ (float)(i % 2) * 2.0f - 1.0f, (float)(i / 2) * 2.0f - 1.0f, 1.5f};
  }

  // Spot
  // ============================================================

  for (uint32_t i = 0; i < spotLightCount; i++)
  {
    uint32_t bulbIndex = i + pointLightCount;
    lights.buffers[bulbIndex].Init(sizeof(Vec3));
    lights.materials[bulbIndex].Init(lights.materialBase, { {.buffer = lights.buffers[bulbIndex] } });
    *lights.spots[i].Add<Quartz::Renderable>() = Quartz::Renderable{ .mesh = &lights.mesh, .material = &lights.materials[bulbIndex] };
    lights.spots[i].Add<Quartz::LightSpot>();
  }

  for (uint32_t i = 0; i < spotLightCount; i++)
  {
    uint32_t bulbIndex = i + pointLightCount;
    lights.pointers.spot[i] = lights.spots[i].Get<Quartz::LightSpot>();
    //lights.pointers.spot[i]->color = Vec4SubtractVec4(Vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, colors[i]);
    lights.pointers.spot[i]->color = colors[i];
    lights.pointers.spot[i]->intensity = 0.0f;
    lights.pointers.spot[i]->direction = Vec3{ 0.0f, 0.0f, 1.0f };
    lights.pointers.spot[i]->inner = cos(PERI_DEGREES_TO_RADIANS(5.0f * i));
    lights.pointers.spot[i]->outer = cos(PERI_DEGREES_TO_RADIANS(10.0f * i));
    lights.buffers[bulbIndex].PushData(&lights.pointers.spot[i]->color);

    lights.transforms[bulbIndex] = lights.spots[i].Get<Transform>();
    lights.transforms[bulbIndex]->scale = Vec3{ 0.1f, 0.1f, 0.3f };
    lights.transforms[bulbIndex]->position = Vec3{ (float)i * 2.0f, 4.0f, 2.0f };
  }

  dummyBuffer.Shutdown();
  return Quartz_Success;
}
