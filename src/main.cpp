
#include <quartz.h>
#include <stdio.h>

#include <diamond.h>

struct TestBufferInfo
{
  int x;
  float y;
};

class GameLayer : public Quartz::Layer
{
public:
  GameLayer() : Layer("Game layer") {}

  void OnAttach() override
  {
    // Resources
    // ============================================================

    TestBufferInfo tbi = { 10, -43.11f };

    m_buffer = Quartz::CreateBuffer(sizeof(TestBufferInfo));
    m_buffer.PushData(&tbi);

    m_image = Quartz::CreateTexture("D:/Dev/QuartzSandbox/res/textures/AltImage.png");

    m_mat = Quartz::CreateMaterial(
      { "D:/Dev/QuartzSandbox/res/shaders/compiled/0_blank.vert.spv",
        "D:/Dev/QuartzSandbox/res/shaders/compiled/1_lights.frag.spv" },
      { {.type = Quartz::Input_Texture, .texture = m_image },
        {.type = Quartz::Input_Buffer, .buffer = m_buffer } }
      );

    m_mesh = Quartz::CreateMesh("D:/Dev/QuartzSandbox/res/models/Cube.obj");

    // Camera
    // ============================================================
    Quartz::Camera* camStruct = m_camera.Add<Quartz::Camera>();
    Transform* camTransform = m_camera.Get<Transform>();
    *camTransform = transformIdentity;
    camTransform->position.z = -1.0f;
    camTransform->rotation.y = 180.0f;

    camStruct->desiredRatio = 16.0f / 9.0f;
    camStruct->farClip = 100.0f;
    camStruct->nearClip = 0.1f;
    camStruct->fov = 65.0f;

    float screenRatio = (float)Quartz::WindowGetWidth() / (float)Quartz::WindowGetHeight();
    camStruct->projectionMatrix = ProjectionPerspectiveExtended(
      screenRatio,
      camStruct->desiredRatio,
      camStruct->fov,
      camStruct->nearClip,
      camStruct->farClip);

    // Entities
    // ============================================================

    Vec3 positions[4] = {
      { 0.0f, 0.0f, 1.0f },
      { 2.0f, 1.0f, 2.0f },
      { 5.0f, 0.0f, 10.0f },
      { -2.0f, -4.0f, 6.0f }
    };

    m_entities.resize(4);
    for (uint32_t i = 0; i < 4; i++)
    {
      Transform* t = m_entities[i].Get<Transform>();
      *t = transformIdentity;
      t->position = positions[i];

      Quartz::Renderable* r = m_entities[i].Add<Quartz::Renderable>();
      r->material = &m_mat;
      r->mesh = &m_mesh;
    }

    // Lights
    // ============================================================

    // Directional
    ld = m_lightDir.Add<Quartz::LightDirectional>();
    ld->color = Vec3{ 0.0f, 0.0f, 0.0f };
    ld->direction = Vec3{ 0.0f, -1.0f, 0.0f };

    // Point
    lp = m_lightPoint.Add<Quartz::LightPoint>();
    lp->color = Vec3{ 1.0f, 1.0f, 1.0f };
    lp->position = Vec3{ 0.0f, 1.25f, 1.3f };
    lp->linear = 0.09f;
    lp->quadratic = 0.032f;

    // Spot
    ls = m_lightSpot.Add<Quartz::LightSpot>();
    ls->color = Vec3{ 1.0f, 1.0f, 1.0f };
    ls->position = Vec3{ 0.0f, 0.0f, 0.0f };
    ls->direction = Vec3{ 0.0f, -1.0f, 0.0f };
    ls->inner = 1.05f;
    ls->outer = 0.9f;
  }

  void OnUpdate() override
  {
    if (Quartz::Input::OnButtonPress(Quartz::Key_Escape))
    {
      Quartz::Quit();
    }

    Transform* t = m_camera.Get<Transform>();
    if (Quartz::Input::ButtonStatus(Quartz::Mouse_Right))
    {
      Vec2I mouseMove = Quartz::Input::GetMouseDelta();
      t->rotation.x += mouseMove.y * m_cameraSensitivity;
      t->rotation.y += mouseMove.x * m_cameraSensitivity;
    }

    // Cam movement
    // ============================================================
    Quaternion rotQ = QuaternionFromEuler(t->rotation);
    Vec3 forward = QuaternionMultiplyVec3(rotQ, Vec3{ 0.0f, 0.0f, -1.0f });
    Vec3 right = QuaternionMultiplyVec3(rotQ, Vec3{ 1.0f, 0.0f, 0.0f });

    forward = Vec3MultiplyFloat(forward, Quartz::Input::ButtonStatus(Quartz::Key_W) - Quartz::Input::ButtonStatus(Quartz::Key_S));
    right = Vec3MultiplyFloat(right, Quartz::Input::ButtonStatus(Quartz::Key_D) - Quartz::Input::ButtonStatus(Quartz::Key_A));
    Vec3 up = Vec3MultiplyFloat(Vec3{ 0.0f, 1.0f, 0.0f }, Quartz::Input::ButtonStatus(Quartz::Key_E) - Quartz::Input::ButtonStatus(Quartz::Key_Q));

    Vec3 movement = Vec3Normalize(Vec3AddVec3(Vec3AddVec3(forward, right), up));
    movement = Vec3MultiplyFloat(movement, Quartz::time.deltaTime * (4 + (4 * Quartz::Input::ButtonStatus(Quartz::Key_Shift_L))));

    t->position = Vec3AddVec3(movement, t->position);
    //QTZ_INFO("Cam pos ({}, {}, {})", t->position.x, t->position.y, t->position.z);

    // Update cubes
    // ============================================================
    //for (uint32_t i = 0; i < 4; i++)
    //{
    //  Transform* r = m_entities[i].Get<Transform>();
    //  r->rotation.y += Quartz::time.deltaTime * 30.0f * (i + 1);
    //  r->rotation.x += Quartz::time.deltaTime * 30.0f * (i + 1);
    //  r->rotation.z += Quartz::time.deltaTime * 30.0f * (i + 1);
    //  r->position.x = sin(Quartz::time.totalTimeDeltaSum / (i + 1)) * i * 3;
    //}
  }

  void OnUpdateImgui() override
  {
    ImGui::Begin("Game");

    Transform* t = m_camera.Get<Transform>();
    ls->position = t->position;
    ls->direction = QuaternionMultiplyVec3(QuaternionFromEuler(t->rotation), Vec3{ 0.0f, 0.0f, 1.0f });

    Vec3 p = t->position;
    ImGui::Text("Cam : %f, %f, %f", p.x, p.y, p.z);

    ImGui::SeparatorText("Directional");
    ImGui::PushID("Dir");
    ImGui::DragFloat3("Color", (float*)&ld->color, 0.01f);
    ImGui::DragFloat3("Direction", (float*)&ld->direction, 0.01f);
    ImGui::PopID();

    ImGui::SeparatorText("Point");
    ImGui::PushID("Point");
    ImGui::DragFloat3("Color", (float*)&lp->color, 0.01f);
    ImGui::DragFloat3("Position", (float*)&lp->position, 0.01f);
    ImGui::DragFloat("Linear", (float*)&lp->linear, 0.01f);
    ImGui::DragFloat("Quadratic", (float*)&lp->quadratic, 0.01f);
    ImGui::PopID();

    ImGui::SeparatorText("Spot");
    ImGui::PushID("Spot");
    ImGui::DragFloat3("Color", (float*)&ls->color, 0.01f);
    ImGui::DragFloat3("Position", (float*)&ls->position, 0.01f);
    ImGui::DragFloat3("Direction", (float*)&ls->direction, 0.01f);
    ImGui::DragFloat("Inner", (float*)&ls->inner, 0.01f);
    ImGui::DragFloat("Outer", (float*)&ls->outer, 0.01f);
    ImGui::DragFloat("Linear", (float*)&ls->linear, 0.01f);
    ImGui::DragFloat("Quadratic", (float*)&ls->quadratic, 0.01f);
    ImGui::PopID();

    ImGui::End();
  }

  void OnDetach() override
  {
    m_buffer.Shutdown();
    m_image.Shutdown();
    m_mesh.Shutdown();
    m_mat.Shutdown();
  }

  void OnEvent(Quartz::Event& e) override
  {
    //QTZ_WARNING("Game handling event : {}", e.GetTypeName_Debug());
    e.NotifyHandled();

    if (e.GetType() == Quartz::Event_Window_Resize)
    {
      Quartz::Camera* camStruct = m_camera.Get<Quartz::Camera>();

      float screenRatio = (float)Quartz::WindowGetWidth() / (float)Quartz::WindowGetHeight();
      camStruct->projectionMatrix = ProjectionPerspectiveExtended(
        screenRatio,
        camStruct->desiredRatio,
        camStruct->fov,
        camStruct->nearClip,
        camStruct->farClip);
    }
  }

private:
  const float m_cameraSensitivity = 0.2f;
  std::vector<Quartz::Renderable*> renderables;
  Quartz::Entity m_camera;
  std::vector<Quartz::Entity> m_entities;
  Quartz::Material m_mat {};
  Quartz::Mesh m_mesh {};
  Quartz::Texture m_image {};
  Quartz::Buffer m_buffer {};

  Quartz::Entity m_lightDir;
  Quartz::Entity m_lightPoint;
  Quartz::Entity m_lightSpot;
  Quartz::LightSpot* ls;
  Quartz::LightPoint* lp;
  Quartz::LightDirectional* ld;
};

QUARTZ_GAME_LAYER(GameLayer)
QUARTZ_ENTRY_POINT
