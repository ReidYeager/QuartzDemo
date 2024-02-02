
#include "sandbox.h"

void Sandbox::RenderImgui()
{
  ImGui::Begin("Sandbox");

  // Directional
  // ============================================================

  ImGui::SeparatorText("Directional light");
  ImGui::PushID("DirectionalLight");

  Quartz::LightDirectional* ptr = lights.pointers.dir;
  ImGui::DragFloat3("Color", (float*)&ptr->color, 0.001f);
  ImGui::DragFloat("Intensity", &ptr->intensity, 0.001f);
  ImGui::DragFloat3("Direction", (float*)&ptr->direction, 0.001f);
  ptr->direction.Normalize();

  ImGui::PopID();

  // Point
  // ============================================================

  ImGui::SeparatorText("Point lights");
  ImGui::PushID("PointLights");
  for (uint32_t i = 0; i < 1; i++)
  {
    Quartz::LightPoint* ptr = lights.pointers.point[i];
    ImGui::PushID(i);

    char buffer[2];
    sprintf(buffer, "%u", i);
    ImGui::SeparatorText(buffer);

    ImGui::DragFloat3("Color", (float*)&ptr->color, 0.001f);
    ImGui::DragFloat("Intensity", &ptr->intensity, 0.001f);
    ImGui::DragFloat3("Position", (float*)&lights.transforms[i]->position, 0.01f);

    ptr->position = lights.transforms[i]->position;
    lights.buffers[i].PushData(&ptr->color);

    ImGui::PopID();
  }
  ImGui::PopID();

  // Spot
  // ============================================================

  //ImGui::SeparatorText("Spot lights");
  //ImGui::PushID("SpotLights");
  //for (uint32_t i = 0; i < spotLightCount; i++)
  //{
  //  uint32_t bulbIndex = i + pointLightCount;
  //  Quartz::LightSpot* ptr = lights.pointers.spot[i];
  //  ImGui::PushID(i);

  //  char buffer[2];
  //  sprintf(buffer, "%u", i);
  //  ImGui::SeparatorText(buffer);

  //  ImGui::DragFloat3("Color", (float*)&ptr->color, 0.001f);
  //  ImGui::DragFloat("Intensity", &ptr->intensity, 0.001f);
  //  ImGui::DragFloat3("Position", (float*)&lights.transforms[bulbIndex]->position, 0.01f);
  //  ImGui::DragFloat3("Direction", (float*)&ptr->direction, 0.01f);
  //  ImGui::DragFloat("Inner", &ptr->inner, 0.001f);
  //  ImGui::DragFloat("Outer", &ptr->outer, 0.001f);

  //  ptr->position = lights.transforms[bulbIndex]->position;
  //  lights.buffers[bulbIndex].PushData(&ptr->color);

  //  ImGui::PopID();
  //}
  //ImGui::PopID();

  ImGui::End();
}
