
#include "sandbox.h"

QUARTZ_ENTRY(Sandbox)

QuartzResult Sandbox::Init()
{
  QTZ_DEBUG("Sandbox Init");

  Quartz::Buffer dummyBuffer(1);

  // Pbr
  // ============================================================

  QTZ_ATTEMPT(pbrTextures.albedo.Init("D:/Dev/QuartzSandbox/res/textures/CyborgWeapon/Weapon_albedo.png"));
  QTZ_ATTEMPT(pbrTextures.normal.Init("D:/Dev/QuartzSandbox/res/textures/CyborgWeapon/Weapon_normal.png"));
  QTZ_ATTEMPT(pbrTextures.maps.Init("D:/Dev/QuartzSandbox/res/textures/CyborgWeapon/Weapon_AoRoughnessMetallic.png"));

  QTZ_ATTEMPT(
    pbrMaterial.Init(
      { "D:/Dev/QuartzSandbox/res/shaders/compiled/0_blank.vert.spv",
        "D:/Dev/QuartzSandbox/res/shaders/compiled/3_pbr.frag.spv" },
        //"D:/Dev/QuartzSandbox/res/shaders/compiled/t_bufferalignment.frag.spv" },
      { {.type = Quartz::Input_Buffer,  .value = { .buffer  = dummyBuffer        } },
        {.type = Quartz::Input_Texture, .value = { .texture = pbrTextures.albedo } },
        {.type = Quartz::Input_Texture, .value = { .texture = pbrTextures.normal } },
        {.type = Quartz::Input_Texture, .value = { .texture = pbrTextures.maps   } }
      })
    );

  // Lights
  // ============================================================

  QTZ_ATTEMPT(lightbulbMesh.Init("D:/Dev/QuartzSandbox/res/models/SphereSmooth.obj"));
  QTZ_ATTEMPT(lightbulbMaterial.Init(
    { "D:/Dev/QuartzSandbox/res/shaders/compiled/0_blank.vert.spv",
      "D:/Dev/QuartzSandbox/res/shaders/compiled/x_light.frag.spv" },
    { {.type = Quartz::Input_Buffer, .value = {.buffer = dummyBuffer } } }
    ));

  // Directional ==========

  dirLightPtr = dirLight.Add<Quartz::LightDirectional>();
  dirLightPtr->color = Vec3{ 1.0f, 1.0f, 1.0f };
  dirLightPtr->direction = Vec3{ 0.0f, -1.0f, 0.0f };

  // Point ==========

  for (uint32_t i = 0; i < pointLightCount; i++)
  {
    lightbulbBuffers[i].Init(sizeof(Vec3));
    lightbulbInstances[i].Init(lightbulbMaterial, { { .buffer = lightbulbBuffers[i] } });
    *pointLights[i].Add<Quartz::Renderable>() = Quartz::Renderable{ .mesh = &lightbulbMesh, .material = &lightbulbInstances[i] };
    pointLights[i].Add<Quartz::LightPoint>();
  }

  Vec3 colors[4] = {
    { 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f },
  };

  for (uint32_t i = 0; i < pointLightCount; i++)
  {
    pointLightPtr[i] = pointLights[i].Get<Quartz::LightPoint>();
    pointLightPtr[i]->color = colors[i];
    pointLightPtr[i]->linear = 0.0f;
    pointLightPtr[i]->quadratic = 0.0f;
    lightbulbBuffers[i].PushData(&pointLightPtr[i]->color);

    lightbulbTransforms[i] = pointLights[i].Get<Transform>();
    lightbulbTransforms[i]->scale = Vec3{ 0.1f, 0.1f, 0.1f };
    lightbulbTransforms[i]->position = Vec3{ (float)(i % 2), (float)(i / 2), 1.0f};
  }

  // Spot ==========

  for (uint32_t i = 0; i < spotLightCount; i++)
  {
    uint32_t bulbIndex = i + pointLightCount;
    lightbulbBuffers[bulbIndex].Init(sizeof(Vec3));
    lightbulbInstances[bulbIndex].Init(lightbulbMaterial, { { .buffer = lightbulbBuffers[bulbIndex] } });
    *spotLights[i].Add<Quartz::Renderable>() = Quartz::Renderable{ .mesh = &lightbulbMesh, .material = &lightbulbInstances[bulbIndex] };
    spotLights[i].Add<Quartz::LightSpot>();
  }

  for (uint32_t i = 0; i < spotLightCount; i++)
  {
    uint32_t bulbIndex = i + pointLightCount;
    spotLightPtr[i] = spotLights[i].Get<Quartz::LightSpot>();
    spotLightPtr[i]->color = Vec3SubtractVec3(Vec3{ 1.0f, 1.0f, 1.0f }, colors[i]);
    spotLightPtr[i]->direction = Vec3{ 0.0f, 0.0f, 1.0f };
    spotLightPtr[i]->linear = 0.0f;
    spotLightPtr[i]->quadratic = 0.0f;
    spotLightPtr[i]->inner = cos(PERI_DEGREES_TO_RADIANS(45.0f));
    spotLightPtr[i]->outer = cos(PERI_DEGREES_TO_RADIANS(80.0f));
    lightbulbBuffers[bulbIndex].PushData(&spotLightPtr[i]->color);

    lightbulbTransforms[bulbIndex] = spotLights[i].Get<Transform>();
    lightbulbTransforms[bulbIndex]->scale = Vec3{ 0.1f, 0.1f, 0.3f };
    lightbulbTransforms[bulbIndex]->position = Vec3{ (float)i * 2.0f, 4.0f, 2.0f };
  }

  // Objects
  // ============================================================

  //QTZ_ATTEMPT(objectMesh.Init("D:/Dev/QuartzSandbox/res/models/Cyborg_Weapon.obj"));
  QTZ_ATTEMPT(objectMesh.Init("D:/Dev/QuartzSandbox/res/models/SphereSmooth.obj"));

  for (uint32_t i = 0; i < objectCount; i++)
  {
    float y = (float)(i / 5);
    float x = (float)(i % 5);

    Quartz::Entity& e = objects[i];

    Transform* t = e.Get<Transform>();
    t->position = Vec3{ x, y, 0.0f };
    t->scale = Vec3{ 0.5f, 0.5f, 0.5f };

    pbrBuffers[i].Init(sizeof(PbrBufferInfo));
    pbrInstances[i].Init(
      pbrMaterial,
      { { .buffer  = pbrBuffers[i]      },
        { .texture = pbrTextures.albedo },
        { .texture = pbrTextures.normal },
        { .texture = pbrTextures.maps   }
      });

    pbrBufferData[i].baseReflectivity = Vec3{ 0.56f, 0.57f, 0.58f };
    pbrBufferData[i].roughness = x / 5.0f + 0.1f;
    pbrBufferData[i].metalness = y / 5.0f + 0.1f;
    pbrBuffers[i].PushData(&pbrBufferData[i]);

    *e.Add<Quartz::Renderable>() = Quartz::Renderable{ .mesh = &objectMesh, .material = &pbrInstances[i] };
  }

  // Camera
  // ============================================================

  camera.Get<Transform>()->position.z = 3.0f;
  Quartz::Camera* c = camera.Add<Quartz::Camera>();
  *c = Quartz::Camera{ .fov = 90.0f, .desiredRatio = 1.0f, .nearClip = 0.1f, .farClip = 100.0f };
  float windowRatio = (float)Quartz::WindowWidth() / (float)Quartz::WindowHeight();
  c->projectionMatrix = ProjectionPerspectiveExtended(windowRatio, c->desiredRatio, c->fov, c->nearClip, c->farClip);


  dummyBuffer.Shutdown();
  return Quartz_Success;
}

QuartzResult Sandbox::Update(double deltaTime)
{
  if (Quartz::Input::OnButtonPress(Quartz::Key_Escape))
  {
    Quartz::RequestQuit();
  }

  if (Quartz::Input::OnButtonPress(Quartz::Key_Space))
  {
    pbrMaterial.Reload();
  }

  float speed = cameraSpeed;
  if (Quartz::Input::ButtonStatus(Quartz::Key_Shift_L))
  {
    speed *= speedShiftMultiplier;
  }
  if (Quartz::Input::ButtonStatus(Quartz::Key_Alt_L))
  {
    speed *= speedAltMultiplier;
  }

  Transform* camTrans = camera.Get<Transform>();
  if (Quartz::Input::ButtonStatus(Quartz::Mouse_Right))
  {
    camTrans->rotation.y += Quartz::Input::GetMouseDelta().x * 0.25f;
    camTrans->rotation.x += Quartz::Input::GetMouseDelta().y * 0.25f;
  }

  float fwd = Quartz::Input::ButtonStatus(Quartz::Key_W) - Quartz::Input::ButtonStatus(Quartz::Key_S);
  float rit = Quartz::Input::ButtonStatus(Quartz::Key_D) - Quartz::Input::ButtonStatus(Quartz::Key_A);
  float up = Quartz::Input::ButtonStatus(Quartz::Key_E) - Quartz::Input::ButtonStatus(Quartz::Key_Q);

  Quaternion rot = QuaternionFromEuler(camTrans->rotation);
  Vec3 camFwd = QuaternionMultiplyVec3(rot, Vec3{ 0.0f, 0.0f, -1.0f });
  Vec3 camRit = QuaternionMultiplyVec3(rot, Vec3{ 1.0f, 0.0f, 0.0f });
  Vec3 camUp = Vec3{ 0.0f, 1.0f, 0.0f };

  camFwd = Vec3MultiplyFloat(camFwd, fwd);
  camRit = Vec3MultiplyFloat(camRit, rit);
  camUp = Vec3MultiplyFloat(camUp, up);

  Vec3 direction = Vec3Normalize(Vec3AddVec3(Vec3AddVec3(camFwd, camRit), camUp));
  camTrans->position = Vec3AddVec3(camTrans->position, Vec3MultiplyFloat(direction, deltaTime * speed));

  return Quartz_Success;
}

void Sandbox::Shutdown()
{
  for (uint32_t i = 0; i < objectCount; i++)
  {
    pbrInstances[i].Shutdown();
    pbrBuffers[i].Shutdown();
  }
  pbrMaterial.Shutdown();
  pbrTextures.albedo.Shutdown();
  pbrTextures.normal.Shutdown();
  pbrTextures.maps.Shutdown();

  objectMesh.Shutdown();

  for (uint32_t i = 0; i < lightbulbCount; i++)
  {
    lightbulbBuffers[i].Shutdown();
    lightbulbInstances[i].Shutdown();
  }
  lightbulbMesh.Shutdown();
  lightbulbMaterial.Shutdown();

  QTZ_DEBUG("Sandbox shutdown");
}

void Sandbox::RenderImgui()
{
  ImGui::Begin("Sandbox");

  // Directional

  ImGui::SeparatorText("Directional light");
  ImGui::PushID("DirectionalLight");

  ImGui::DragFloat3("Color", (float*)&dirLightPtr->color);
  ImGui::DragFloat3("Direction", (float*)&dirLightPtr->direction);
  dirLightPtr->direction = Vec3Normalize(dirLightPtr->direction);

  ImGui::PopID();

  // Point

  ImGui::SeparatorText("Point lights");
  ImGui::PushID("PointLights");
  for (uint32_t i = 0; i < pointLightCount; i++)
  {
    Quartz::LightPoint* ptr = pointLightPtr[i];
    ImGui::PushID(i);

    char buffer[2];
    sprintf(buffer, "%u", i);
    ImGui::SeparatorText(buffer);

    ImGui::DragFloat3("Color", (float*)&ptr->color, 0.001f);
    ImGui::DragFloat3("Position", (float*)&lightbulbTransforms[i]->position, 0.01f);
    ImGui::DragFloat("Linear", &ptr->linear, 0.001f);
    ImGui::DragFloat("Quadratic", &ptr->quadratic, 0.001f);

    ptr->position = lightbulbTransforms[i]->position;
    lightbulbBuffers[i].PushData(&ptr->color);

    ImGui::PopID();
  }
  ImGui::PopID();

  // Spot

  ImGui::SeparatorText("Spot lights");
  ImGui::PushID("SpotLights");
  for (uint32_t i = 0; i < spotLightCount; i++)
  {
    uint32_t bulbIndex = i + pointLightCount;
    Quartz::LightSpot* ptr = spotLightPtr[i];
    ImGui::PushID(i);

    char buffer[2];
    sprintf(buffer, "%u", i);
    ImGui::SeparatorText(buffer);

    ImGui::DragFloat3("Color", (float*)&ptr->color, 0.001f);
    ImGui::DragFloat3("Position", (float*)&lightbulbTransforms[bulbIndex]->position, 0.01f);
    ImGui::DragFloat3("Direction", (float*)&ptr->direction, 0.01f);
    ImGui::DragFloat("Linear", &ptr->linear, 0.001f);
    ImGui::DragFloat("Quadratic", &ptr->quadratic, 0.001f);
    ImGui::DragFloat("Inner", &ptr->inner, 0.001f);
    ImGui::DragFloat("Outer", &ptr->outer, 0.001f);

    ptr->position = lightbulbTransforms[bulbIndex]->position;
    lightbulbBuffers[bulbIndex].PushData(&ptr->color);

    ImGui::PopID();
  }
  ImGui::PopID();

  ImGui::End();
}
