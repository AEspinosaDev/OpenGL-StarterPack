#include "application.h"

void Application::setup_user_interface_frame()
{
    Renderer::setup_user_interface_frame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(300, 230), ImGuiCond_Once);
    ImGui::Begin("Settings");
    ImGui::SeparatorText("Profiler");
    ImGui::Text(" %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Separator();
    ImGui::SeparatorText("Global Settings");
    if (ImGui::Checkbox("V-Sync", &m_settings.vSync))
    {
        set_v_sync(m_settings.vSync);
    }
    ImGui::SeparatorText("Controls");
    ImGui::BulletText("Move camera: WASD");
    ImGui::BulletText("Move camera: Mouse+Left Buttom");
    ImGui::BulletText("Fullscreen: F11");
    ImGui::BulletText("Quit: ESC");
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(m_window.extent.width-360, 10), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(350, 280), ImGuiCond_Once);
    ImGui::Begin("Flock of birds");
    ImGui::Separator();
    widget::draw_transform_widget(m_seagulls.mesh);
    ImGui::DragFloat("Birds size", &m_seagulls.birdSize, 0.1f, 0.1f, 2.0f);
    ImGui::DragFloat("Wing size", &m_seagulls.wingLength, 0.1f, 0.1f, 1.0f);
    ImGui::DragFloat("Wing speed", &m_seagulls.wingSpeed, 0.1f, 0.1f, 5.0f);
    ImGui::DragFloat("Speed", &m_seagulls.speed, 0.1f, 0.0f, 5.0f);
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(m_window.extent.width-390, 300), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(380, 300), ImGuiCond_Once);
    ImGui::Begin("Ocean");
    ImGui::Separator();
    widget::draw_transform_widget(m_water.mesh);
    ImGui::Checkbox("Dynamic Res", &m_water.dynamicResolution);
    ImGui::DragFloat("Max Tess level", &m_water.maxResolution, 1.0f, 2.0f, 64.0f);
    ImGui::DragFloat("Min Tess level", &m_water.minResolution, 1.0f, 2.0f, 32.0f);
    ImGui::DragFloat("Max distance", &m_water.maxDistance, 1.0f, 2.0f, 1000.0f);
    ImGui::DragFloat("Min distance", &m_water.minDistance, 1.0f, 2.0f, 1000.0f);
    ImGui::DragFloat("Height", &m_water.heightPower, 0.01f, 0.01f, 1.0f);
    ImGui::ColorEdit3("Water tint", (float *)&m_water.tint);
    ImGui::End();

    ImGui::Render();
}