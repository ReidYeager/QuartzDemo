
#include <quartz.h>

class Sandbox : public Quartz::Application
{
public:
  QuartzResult Init() override
  {
    QTZ_DEBUG("Sandbox Init");
    return Quartz_Success;
  }

  QuartzResult Update(double deltaTime) override
  {
    if (Quartz::Input::OnButtonPress(Quartz::Key_Escape))
    {
      Quartz::RequestQuit();
    }

    return Quartz_Success;
  }

  void Shutdown() override
  {
    QTZ_DEBUG("Sandbox shutdown");
  }

  void RenderImgui() override
  {
    ImGui::Begin("Sandbox");

    ImGui::Text("Sandbox test");

    ImGui::End();
  }
};

QUARTZ_ENTRY(Sandbox)
