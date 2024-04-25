/*
    This file is part of OpenGL-StarterPack (GLSP), an open source OpenGL based framework
    that facilitates and speeds up demo and project creation by offering an abstraction to 
    the basic objects of OpenGL as well as incluing the necessary libraries. 

    MIT License

	Copyright (c) 2024 Antonio Espinosa Garcia

*/
#ifndef __LOADERS__
#define __LOADERS__

#include <unordered_map>
#include <algorithm>
#include <tiny_obj_loader.h>
#include <tinyply.h>
#include <stb_image.h>
#include <GLSP/mesh.h>
#include <GLSP/utils.h>

GLSP_NAMESPACE_BEGIN

namespace loaders
{
    void load_OBJ(Mesh *const mesh, const char *fileName, bool importMaterials = false, bool calculateTangents = false);

    void load_PLY(Mesh *const mesh, const char *fileName, bool preload = true, bool verbose = false, bool calculateTangents = false);

    void load_image(Texture* const texture, const char *fileName);
    
}

GLSP_NAMESPACE_END

#endif