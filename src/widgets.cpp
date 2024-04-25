/*
    This file is part of OpenGL-StarterPack (GLSP), an open source OpenGL based framework
    that facilitates and speeds up demo and project creation by offering an abstraction to
    the basic objects of OpenGL as well as incluing the necessary libraries.

    MIT License

    Copyright (c) 2024 Antonio Espinosa Garcia

*/
#include <GLSP/widgets.h>

GLSP_NAMESPACE_BEGIN

void widget::draw_transform_widget(Object3D *obj, std::string label)
{
    ImGui::PushID(obj);
    ImGui::Spacing();

    if (label.empty())
        ImGui::BulletText("Transform");
    else
        ImGui::BulletText((label+" transform").c_str());

    const float PI = 3.14159265359f;

    float position[3] = {obj->get_position().x,
                         obj->get_position().y,
                         obj->get_position().z};

    if (ImGui::DragFloat3("Position", position, 0.1f))
    {
        obj->set_position(glm::vec3(position[0], position[1], position[2]));
    };
    float rotation[3] = {obj->get_rotation().x,
                         obj->get_rotation().y,
                         obj->get_rotation().z};
    if (ImGui::DragFloat3("Rotation", rotation, 0.1f))
    {
        obj->set_rotation(glm::vec3(rotation[0],
                                    rotation[1],
                                    rotation[2]));
    };
    float scale[3] = {obj->get_scale().x,
                      obj->get_scale().y,
                      obj->get_scale().z};
    if (ImGui::DragFloat3("Scale", scale, 0.1f))
    {
        obj->set_scale(glm::vec3(scale[0], scale[1], scale[2]));
    };

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::PopID();
}

void widget::draw_light_widget(Light *l, std::string label)
{
    ImGui::Spacing();
     if (label.empty())
        ImGui::BulletText("Light");
    else
        ImGui::BulletText((label+" Light").c_str());

    float intensity = l->get_intensity();
    if (ImGui::DragFloat("Intensity", &intensity, 0.005f, 0.0f, 10.0f))
        l->set_intensity(intensity);
    glm::vec3 color = l->get_color();
    if (ImGui::ColorEdit3("Color", (float *)&color))
    {
        l->set_color(color);
    };

    bool castShadows = l->get_cast_shadows();

    if (ImGui::Checkbox("Cast Shadows", &castShadows))
    {
        l->set_cast_shadows(castShadows);
    };
    if (castShadows)
    {
        ShadowConfig shadow = l->get_shadow_config();

        float shadowNear = shadow.nearPlane;
        if (ImGui::DragFloat("Shadow Near Plane", &shadowNear, 0.005f, 0.0f, 10.0f))
        {
            shadow.nearPlane = shadowNear;
            l->set_shadow_config(shadow);
        }
        float shadowFar = shadow.farPlane;
        if (ImGui::DragFloat("Shadow Far Plane", &shadowFar, 1.0f, 10.0f, 1000.0f))
        {

            shadow.farPlane = shadowFar;
            l->set_shadow_config(shadow);
        }
        float shadowFov = shadow.fov;
        if (ImGui::DragFloat("Shadow FOV", &shadowFov, 1.0f, 0.0f, 160.0f))
        {

            shadow.fov = shadowFov;
            l->set_shadow_config(shadow);
        }
        float position[3] = {shadow.target.x,
                             shadow.target.y,
                             shadow.target.z};
        if (ImGui::DragFloat3("Shadow Target", position, 0.1f))
        {
            shadow.target = glm::vec3(position[0], position[1], position[2]);
            l->set_shadow_config(shadow);
        };
        ImGui::Text("Advanced Shadow Settings:");
        float bias = shadow.bias;
        if (ImGui::DragFloat("Shadow Bias", &bias, 0.0001f, 0.0f, 1.0f))
        {
            shadow.bias = bias;
            l->set_shadow_config(shadow);
        }

        int kernel = shadow.pcfKernel;
        if (ImGui::DragInt("PC Filter Kernel", &kernel, 2, 3, 15))
        {
            shadow.pcfKernel = kernel;
            l->set_shadow_config(shadow);
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
}

void widget::draw_point_light_widget(PointLight *l, std::string label)
{
}

void widget::draw_directional_light_widget(PointLight *l, std::string label)
{
}

void widget::draw_mesh_widget(Mesh *m, std::string label)
{
}

void widget::draw_camera_widget(Camera *cam, std::string label)
{
    ImGui::Spacing();
      if (label.empty())
        ImGui::BulletText("Camera");
    else
        ImGui::BulletText((label+" camera").c_str());

    float _far = cam->get_far();
    float _near = cam->get_near();
    float fov = cam->get_field_of_view();

    if (ImGui::DragFloat("Near", &_near, 0.05f, 0.0f, 10.0))
        cam->set_near(_near);
    if (ImGui::DragFloat("Far", &_far, 0.1f, 0.0f, 9999.0f))
        cam->set_far(_far);
    if (ImGui::DragFloat("Field of view", &fov, 0.1f, 0.0f, 160.0f))
        cam->set_field_of_view(fov);

    ImGui::Spacing();
    ImGui::Separator();
}
GLSP_NAMESPACE_END
