
#include "sandbox.h"

QuartzResult Sandbox::Update(double deltaTime)
{
  if (Quartz::Input::OnButtonPress(Quartz::Key_Escape))
  {
    Quartz::RequestQuit();
  }

  if (Quartz::Input::OnButtonPress(Quartz::Key_Space) || Quartz::Input::OnButtonPress(Quartz::Mouse_Middle))
  {
    pbrResources.materialBase.Reload();
    skybox.material.Reload();
  }

  // Camera
  // ============================================================

  QTZ_ATTEMPT(UpdateCamera());

  // Update spotlight transforms
  // ============================================================

  Transform* camTrans = camera.Get<Transform>();
  Quaternion rot = Quaternion(camTrans->rotation);

  for (uint32_t i = 0; i < spotLightCount; i++)
  {
    Transform* t = lights.spots[i].Get<Transform>();
    t->position = camTrans->position;

    lights.spots[i].Get<Quartz::LightSpot>()->direction = rot * Vec3{ 0.0f, 0.0f, 1.0f };
  }

  return Quartz_Success;
}

QuartzResult Sandbox::UpdateCamera()
{
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

  Quaternion rot = Quaternion(camTrans->rotation);
  Vec3 camFwd = rot * Vec3{ 0.0f, 0.0f, -1.0f };
  Vec3 camRit = rot * Vec3{ 1.0f, 0.0f, 0.0f };
  Vec3 camUp = Vec3{ 0.0f, 1.0f, 0.0f };

  camFwd = camFwd * fwd;
  camRit = camRit * rit;
  camUp = camUp * up;

  Vec3 direction = (camFwd + camRit + camUp).Normal();
  camTrans->position = camTrans->position + (direction * Quartz::DeltaTime() * speed);

  return Quartz_Success;
}
