
#include "sandbox.h"

static Transform* cameraTransformPointer = nullptr;

QuartzResult Sandbox::Update(double deltaTime)
{
  static Quartz::TextureSkybox* prevSkybox = skybox.pCurrentTexture;
  static Quartz::Texture* prevAlbedo = object.pCurrentAlbedo;
  static Quartz::Texture* prevNormal = object.pCurrentNormal;
  static Quartz::Texture* prevMaps = object.pCurrentMaps;

  if (cameraTransformPointer == nullptr)
  {
    cameraTransformPointer = camera.Get<Transform>();
  }

  if (prevSkybox != skybox.pCurrentTexture)
  {
    skybox.material.SetSingleInput(0, { .texture = &skybox.pCurrentTexture->GetBase() });
    skybox.material.UpdateInputs();

    object.material.SetSingleInput(3, { .texture = &skybox.pCurrentTexture->GetDiffuse() });
    object.material.SetSingleInput(4, { .texture = &skybox.pCurrentTexture->GetSpecular() });
    object.material.SetSingleInput(5, { .texture = &skybox.pCurrentTexture->GetBrdf() });
    object.material.UpdateInputs();

    prevSkybox = skybox.pCurrentTexture;
  }

  if (prevAlbedo != object.pCurrentAlbedo)
  {
    object.material.SetSingleInput(0, { .texture = object.pCurrentAlbedo });
    object.material.UpdateInputs();

    prevAlbedo = object.pCurrentAlbedo;
  }

  if (prevNormal != object.pCurrentNormal)
  {
    object.material.SetSingleInput(1, { .texture = object.pCurrentNormal });
    object.material.UpdateInputs();

    prevNormal = object.pCurrentNormal;
  }

  if (prevMaps != object.pCurrentMaps)
  {
    object.material.SetSingleInput(2, { .texture = object.pCurrentMaps });
    object.material.UpdateInputs();

    prevMaps = object.pCurrentMaps;
  }

  if (ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard)
  {
    return Quartz_Success;
  }

  if (Quartz::Input::OnButtonPress(Quartz::Key_Escape))
  {
    Quartz::RequestQuit();
  }

  if (Quartz::Input::OnButtonPress(Quartz::Key_Space) || Quartz::Input::OnButtonPress(Quartz::Mouse_Middle))
  {
    QTZ_ATTEMPT(object.material.Reload());
    QTZ_ATTEMPT(skybox.material.Reload());
  }

  // Camera
  // ============================================================

  if (useOrbitCam)
  {
    UpdateCameraOrbit();
  }
  else
  {
    UpdateCameraFly();
  }

  return Quartz_Success;
}

void Sandbox::UpdateCameraOrbit()
{
  if (Quartz::Input::ButtonStatus(Quartz::Mouse_Right))
  {
    cameraTransformPointer->rotation.y += Quartz::Input::GetMouseDelta().x * 0.25f;
    cameraTransformPointer->rotation.x += Quartz::Input::GetMouseDelta().y * 0.25f;
  }
  cameraArmLength -= Quartz::Input::GetMouseScroll().y * 0.1f;

  Quaternion rot = Quaternion(cameraTransformPointer->rotation);
  cameraTransformPointer->position = rot * Vec3{ 0.0f, 0.0f, cameraArmLength };
}

void Sandbox::UpdateCameraFly()
{
  float speed = cameraSpeed;
  if (Quartz::Input::ButtonStatus(Quartz::Key_Shift_L))
  {
    speed *= speedShiftMultiplier;
  }
  if (Quartz::Input::ButtonStatus(Quartz::Key_Ctrl_L))
  {
    speed *= speedAltMultiplier;
  }

  if (Quartz::Input::ButtonStatus(Quartz::Mouse_Right))
  {
    cameraTransformPointer->rotation.y += Quartz::Input::GetMouseDelta().x * 0.25f;
    cameraTransformPointer->rotation.x += Quartz::Input::GetMouseDelta().y * 0.25f;
  }

  float fwd = Quartz::Input::ButtonStatus(Quartz::Key_W) - Quartz::Input::ButtonStatus(Quartz::Key_S);
  float rit = Quartz::Input::ButtonStatus(Quartz::Key_D) - Quartz::Input::ButtonStatus(Quartz::Key_A);
  float up = Quartz::Input::ButtonStatus(Quartz::Key_E) - Quartz::Input::ButtonStatus(Quartz::Key_Q);

  Quaternion rot = Quaternion(cameraTransformPointer->rotation);
  Vec3 camFwd = rot * Vec3{ 0.0f, 0.0f, -1.0f };
  Vec3 camRit = rot * Vec3{ 1.0f, 0.0f, 0.0f };
  Vec3 camUp = Vec3{ 0.0f, 1.0f, 0.0f };

  camFwd = camFwd * fwd;
  camRit = camRit * rit;
  camUp = camUp * up;

  Vec3 direction = (camFwd + camRit + camUp).Normal();
  cameraTransformPointer->position = cameraTransformPointer->position + (direction * Quartz::DeltaTime() * speed);
}
