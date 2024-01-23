
#include <quartz.h>

class Sandbox : public Quartz::Application
{
private:
  Quartz::Material mat;
  Quartz::Mesh mesh;

  Quartz::Entity e;
  Quartz::Entity cam;

  Quartz::Texture tex;

public:
  ~Sandbox() {}

  QuartzResult Init() override
  {
    QTZ_DEBUG("Sandbox Init");

    Quartz::Buffer b(1024);
    b.Shutdown();

    QTZ_ATTEMPT(tex.Init("D:/Dev/QuartzSandbox/res/textures/AltImage.png"));

    QTZ_ATTEMPT(
      mat.Init(
        {
          "D:/Dev/QuartzSandbox/res/shaders/compiled/0_blank.vert.spv",
          "D:/Dev/QuartzSandbox/res/shaders/compiled/0_blank.frag.spv"
        },
        {
          {.type = Quartz::Input_Texture, .texture = tex }
        })
      );

    QTZ_ATTEMPT(mesh.Init("D:/Dev/QuartzSandbox/res/models/Cyborg_Weapon.obj"));

    e.Get<Transform>()->position.z = -1.0f;
    Quartz::Renderable* r = e.Add<Quartz::Renderable>();
    r->mesh = &mesh;
    r->material = &mat;

    cam.Get<Transform>()->position.z = 1.0f;
    Quartz::Camera* c = cam.Add<Quartz::Camera>();
    float ratio = (float)Quartz::WindowWidth() / (float)Quartz::WindowHeight();
    c->projectionMatrix = ProjectionPerspectiveExtended(ratio, 1.0f, 100.0f, 0.1f, 10.0f);

    return Quartz_Success;
  }

  QuartzResult Update(double deltaTime) override
  {
    if (Quartz::Input::OnButtonPress(Quartz::Key_Escape))
    {
      Quartz::RequestQuit();
    }

    if (Quartz::Input::ButtonStatus(Quartz::Key_Space))
    {
      e.Get<Transform>()->rotation.y += deltaTime * 90.0f;
    }

    if (Quartz::Input::ButtonStatus(Quartz::Key_F))
    {
      static double t = 0.0f;
      cam.Get<Transform>()->rotation.x = sin(t += deltaTime) * 45.0f;
    }

    return Quartz_Success;
  }

  void Shutdown() override
  {
    mat.Shutdown();
    mesh.Shutdown();
    tex.Shutdown();
    QTZ_DEBUG("Sandbox shutdown");
  }

  void RenderImgui() override
  {
    ImGui::Begin("Sandbox");

    ImGui::Text("Sandbox test");
    ImGui::Image(tex.ForImgui(), {128, 128});

    ImGui::End();
  }
};

QUARTZ_ENTRY(Sandbox)
