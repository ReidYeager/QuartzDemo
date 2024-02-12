
#include "sandbox.h"

void Sandbox::RenderImgui()
{
  static uint32_t selectedIndex = 1;

  ImGui::Begin("Sandbox");

  ImGui::DragFloat("Gamma", &skybox.bufferData.gamma, 0.001f);
  ImGui::DragFloat("Exposure", &skybox.bufferData.exposure, 0.001f);
  skybox.buffer.PushData(&skybox.bufferData);

  if (ImGui::ImageButton(skybox.pCurrentTexture->ForImgui(), { 128, 64 }))
  {
    ImGui::OpenPopup("imageSelectPopup");
  }
  ImGui::SameLine();

  if (ImGui::BeginPopup("imageSelectPopup"))
  {
    for (uint32_t i = 0; i < skyboxTextures.size(); i++)
    {
      if (i % 2)
      {
        ImGui::SameLine();
      }

      if (ImGui::ImageButton(skyboxTextures[i].ForImgui(), { 128, 64 }))
      {
        skybox.pCurrentTexture = &skyboxTextures[i];
        ImGui::CloseCurrentPopup();
      }
    }

    ImGui::EndPopup();
  }

  ImGui::End();
}
