/*
    This file is part of OpenGL-StarterPack (GLSP), an open source OpenGL based framework
    that facilitates and speeds up demo and project creation by offering an abstraction to
    the basic objects of OpenGL as well as incluing the necessary libraries.

    MIT License

    Copyright (c) 2024 Antonio Espinosa Garcia

*/
/*

WIP ...

*/

#ifndef __WIDGETS__
#define __WIDGETS__

#include <GLSP/core.h>
#include <GLSP/mesh.h>
#include <GLSP/camera.h>
#include <GLSP/light.h>

GLSP_NAMESPACE_BEGIN

/*
Offers built up ImGui menu functionality
*/
namespace widget
{
    void draw_transform_widget(Object3D *obj, std::string label = "");
    void draw_light_widget(Light *l, std::string label = "");
    void draw_point_light_widget(PointLight *l, std::string label = "");
    void draw_directional_light_widget(PointLight *l, std::string label = "");
    void draw_mesh_widget(Mesh *m, std::string label = "");
    void draw_camera_widget(Camera *cam, std::string label = "");
}
GLSP_NAMESPACE_END

#endif