
#include "sandbox.h"

static const char* meshNames[] = {
  "Cerberus",
  "Sphere",
  "Cube"
};

void Sandbox::RenderImgui()
{
  static uint32_t selectedMeshIndex = 0, selectedAlbedoIndex = 0, selectedMapsIndex = 0;
  static Vec3 customAlbedo = { 1.0f, 1.0f, 1.0f }, prevCustomAlbedo = customAlbedo;
  static float customRougness = 0.0f, prevCustomRougness = customRougness;
  static float customMetallic = 0.0f, prevCustomMetallic = customMetallic;
  static Transform* objectTransform = object.entity.Get<Transform>();

  ImGui::Begin("Sandbox");

  // Camera
  // ============================================================

  ImGui::SeparatorText("Camera");
  ImGui::Checkbox("Use orbit camera", &useOrbitCam);

  // Skybox
  // ============================================================

  ImGui::SeparatorText("Skybox settings");
  if (ImGui::ImageButton(skybox.pCurrentTexture->ForImgui(), { 128, 64 }))
  {
    ImGui::OpenPopup("skyboxImageSelectPopup");
  }
  ImGui::SameLine();

  if (ImGui::BeginPopup("skyboxImageSelectPopup"))
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

  ImGui::NewLine();
  ImGui::DragFloat("Gamma", &skybox.bufferData.gamma, 0.001f);
  ImGui::DragFloat("Exposure", &skybox.bufferData.exposure, 0.001f);
  skybox.buffer.PushData(&skybox.bufferData);

  // Object
  // ============================================================

  ImGui::SeparatorText("Object settings");

  // Rotation ==============================

  ImGui::DragFloat3("Rotation", objectTransform->rotation.elements, 0.1f);

  // Mesh ==============================

  if (ImGui::BeginCombo("Mesh", meshNames[selectedMeshIndex]))
  {
    for (int i = 0; i < sizeof(meshNames) / sizeof(*meshNames); i++)
    {
      const bool is_selected = (selectedMeshIndex == i);
      if (ImGui::Selectable(meshNames[i], is_selected))
      {
        object.entity.Get<Quartz::Renderable>()->mesh = &object.meshes[i];
        selectedMeshIndex = i;
      }

      if (is_selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  // Textures ==============================

  // Albedo =====

  if (ImGui::ImageButton(object.pCurrentAlbedo->ForImgui(), { 64, 64 }))
  {
    ImGui::OpenPopup("albedoImageSelectPopup");
  }
  ImGui::SameLine();
  ImGui::Text("Albedo");
  ImGui::SameLine();

  if (ImGui::BeginPopup("albedoImageSelectPopup"))
  {
    for (uint32_t i = 0; i < pbrTextures.albedoCount; i++)
    {
      //if (i % 2)
      //{
      //  ImGui::SameLine();
      //}

      if (ImGui::ImageButton(pbrTextures.albedos[i].ForImgui(), { 64, 64 }))
      {
        object.pCurrentAlbedo = &pbrTextures.albedos[i];
        selectedAlbedoIndex = i;

        ImGui::CloseCurrentPopup();
      }
    }

    ImGui::EndPopup();
  }
  ImGui::NewLine();

  if (selectedAlbedoIndex == pbrTextures.albedoCount - 1)
  {
    ImGui::ColorEdit3("##Custom albedo", customAlbedo.elements);

    if (customAlbedo != prevCustomAlbedo)
    {
      uint8_t pixels[4] = { uint8_t(customAlbedo.x * 255), uint8_t(customAlbedo.y * 255), uint8_t(customAlbedo.z * 255), 255 };
      object.pCurrentAlbedo->FillImage(pixels);

      prevCustomAlbedo = customAlbedo;
    }
  }

  // Normal =====

  if (ImGui::ImageButton(object.pCurrentNormal->ForImgui(), { 64, 64 }))
  {
    ImGui::OpenPopup("normalImageSelectPopup");
  }
  ImGui::SameLine();
  ImGui::Text("Normal");
  ImGui::SameLine();

  if (ImGui::BeginPopup("normalImageSelectPopup"))
  {
    for (uint32_t i = 0; i < pbrTextures.normalCount; i++)
    {
      //if (i % 2)
      //{
      //  ImGui::SameLine();
      //}

      if (ImGui::ImageButton(pbrTextures.normals[i].ForImgui(), { 64, 64 }))
      {
        object.pCurrentNormal = &pbrTextures.normals[i];

        ImGui::CloseCurrentPopup();
      }
    }

    ImGui::EndPopup();
  }
  ImGui::NewLine();

  // Maps ====

  if (ImGui::ImageButton(object.pCurrentMaps->ForImgui(), { 64, 64 }))
  {
    ImGui::OpenPopup("mapImageSelectPopup");
  }
  ImGui::SameLine();
  ImGui::Text("Roughness/Metallic");
  ImGui::SameLine();

  if (ImGui::BeginPopup("mapImageSelectPopup"))
  {
    for (uint32_t i = 0; i < pbrTextures.mapCount; i++)
    {
      //if (i % 2)
      //{
      //  ImGui::SameLine();
      //}

      if (ImGui::ImageButton(pbrTextures.maps[i].ForImgui(), { 64, 64 }))
      {
        object.pCurrentMaps = &pbrTextures.maps[i];
        selectedMapsIndex = i;

        ImGui::CloseCurrentPopup();
      }
    }

    ImGui::EndPopup();
  }
  ImGui::NewLine();

  if (selectedMapsIndex == pbrTextures.mapCount - 1)
  {
    ImGui::DragFloat("Metallic", &customMetallic, 0.001f, 0.0f, 1.0f);
    ImGui::DragFloat("Roughness", &customRougness, 0.001f, 0.0f, 1.0f);

    if (customMetallic != prevCustomMetallic || customRougness != prevCustomRougness)
    {
      uint8_t pixels[4] = { 255, uint8_t(customRougness * 255), uint8_t(customMetallic * 255), 255 };
      object.pCurrentMaps->FillImage(pixels);

      prevCustomMetallic = customMetallic;
      prevCustomRougness = customRougness;
    }
  }

  ImGui::End();
}
