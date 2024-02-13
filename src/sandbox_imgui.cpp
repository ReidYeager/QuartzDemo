
#include "sandbox.h"

static const char* meshNames[] = {
  "Cerberus",
  "Sphere",
  "Cube"
};

void Sandbox::RenderImgui()
{
  ImGui::Begin("Settings");

  ImguiCamera();
  ImguiSkybox();
  ImguiObjectModel();
  ImguiObjectTextures();


  ImGui::SeparatorText("Lights");

  ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
  ImguiDirectionalLight();
  ImguiPointLights();
  ImguiSpotLights();
  ImGui::PopStyleVar();

  ImGui::End();
}

void Sandbox::ImguiCamera()
{
  ImGui::SeparatorText("Camera");
  ImGui::Checkbox("Use orbit camera", &useOrbitCam);
  ImGui::Text("Hold right mouse button to rotate");
}

void Sandbox::ImguiSkybox()
{
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
}

void Sandbox::ImguiObjectModel()
{
  static uint32_t selectedMeshIndex = 0;
  static Transform* objectTransform = object.entity.Get<Transform>();

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
}

void Sandbox::ImguiObjectTextures()
{
  static uint32_t selectedAlbedoIndex = 0, selectedMapsIndex = 0;
  static Vec3 customAlbedo = { 1.0f, 1.0f, 1.0f }, prevCustomAlbedo = customAlbedo;
  static float customRougness = 0.0f, prevCustomRougness = customRougness;
  static float customMetallic = 0.0f, prevCustomMetallic = customMetallic;

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
}

void Sandbox::ImguiDirectionalLight()
{
  ImGui::PushID("Directional");

  ImGui::BeginChild("Directional", ImVec2(-FLT_MIN, 0.0f), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_MenuBar);

  if (ImGui::BeginMenuBar())
  {
    ImGui::MenuItem("Directional");
    if (ImGui::MenuItem(m_directional.enabled ? "Disable" : "Enable"))
    {
      if (m_directional.enabled)
      {
        m_directional.entity.Disable();
      }
      else
      {
        m_directional.entity.Enable();
      }

      m_directional.enabled = !m_directional.enabled;
    }

    ImGui::EndMenuBar();
  }

  if (m_directional.enabled)
  {
    ImGui::ColorEdit3("Color", m_directional.light->color.elements);
    ImGui::DragFloat("Intensity", &m_directional.light->intensity, 0.001f, 0.0f);
    ImGui::DragFloat3("Direction", m_directional.light->direction.elements, 0.001f);
    m_directional.light->direction.Normalize();
  }

  ImGui::EndChild();

  ImGui::PopID();
}

void Sandbox::ImguiPointLights()
{
  ImGui::PushID("Points");
  char nameBuffer[12];

  for (uint32_t i = 0; i < m_pointCount; i++)
  {
    ImGui::PushID(i);
    SandboxPointLight& p = m_points[i];

    sprintf_s(nameBuffer, 12, "Point %d", i);

    ImGui::BeginChild(nameBuffer, ImVec2(-FLT_MIN, 0.0f), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
      ImGui::MenuItem(nameBuffer);
      if (ImGui::MenuItem(p.enabled ? "Disable" : "Enable"))
      {
        if (p.enabled)
        {
          p.entity.Disable();
        }
        else
        {
          p.entity.Enable();
        }

        p.enabled = !p.enabled;
      }

      ImGui::EndMenuBar();
    }

    if (p.enabled)
    {
      ImGui::ColorEdit3("Color", p.light->color.elements);
      ImGui::DragFloat("Intensity", &p.light->intensity, 0.001f);
      ImGui::DragFloat3("Position", p.transform->position.elements, 0.001f);

      p.light->position = p.transform->position;
    }

    ImGui::EndChild();

    ImGui::PopID();
  }

  ImGui::PopID();
}

void Sandbox::ImguiSpotLights()
{
  ImGui::PushID("Spots");
  char nameBuffer[12];

  for (uint32_t i = 0; i < m_spotCount; i++)
  {
    ImGui::PushID(i);
    sprintf_s(nameBuffer, 12, "Spot %d", i);

    SandboxSpotLight& s = m_spots[i];

    ImGui::BeginChild(nameBuffer, ImVec2(-FLT_MIN, 0.0f), ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
      ImGui::MenuItem(nameBuffer);
      if (ImGui::MenuItem(s.enabled ? "Disable" : "Enable"))
      {
        if (s.enabled)
        {
          s.entity.Disable();
        }
        else
        {
          s.entity.Enable();
        }

        s.enabled = !s.enabled;
      }

      ImGui::EndMenuBar();
    }

    if (s.enabled)
    {
      ImGui::ColorEdit3("Color", s.light->color.elements);
      ImGui::DragFloat("Intensity", &s.light->intensity, 0.001f);
      ImGui::DragFloat("Inner angle", &s.innerDegrees, 0.05f);
      ImGui::DragFloat("Outer angle", &s.outerDegrees, 0.05f);

      s.light->inner = cos(PERI_DEGREES_TO_RADIANS(s.innerDegrees));
      s.light->outer = cos(PERI_DEGREES_TO_RADIANS(s.outerDegrees));

      ImGui::Separator();

      ImGui::Checkbox("Lock to camera", &s.lockedToCamera);
      if (s.lockedToCamera)
      {
        Transform* camTrans = camera.Get<Transform>();
        s.transform->position = camTrans->position;
        s.transform->rotation = camTrans->rotation;
      }
      else
      {
        ImGui::DragFloat3("Position", s.transform->position.elements, 0.001f);
        // TODO : Replace with "Direction" once Peridot is updated
        ImGui::DragFloat3("Rotation", s.transform->rotation.elements, 0.05f);
      }

      s.light->position = s.transform->position;
      s.light->direction = Quaternion(s.transform->rotation) * Vec3 { 0.0f, 0.0f, -1.0f };
    }

    ImGui::EndChild();
    ImGui::PopID();
  }

  ImGui::PopID();
}
