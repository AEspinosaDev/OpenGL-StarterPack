/*
    This file is part of OpenGL-StarterPack (GLSP), an open source OpenGL based framework
    that facilitates and speeds up demo and project creation by offering an abstraction to 
    the basic objects of OpenGL as well as incluing the necessary libraries. 

    MIT License

	Copyright (c) 2024 Antonio Espinosa Garcia

*/
#include <GLSP/core.h>

void GLFW_check_error()
{
    const char *description;
    int code = glfwGetError(&description);
    std::cout << description << std::endl;
}

void GLclearError()
{
    while (glGetError() != GL_NO_ERROR)
        ;
}

bool GLlogCall(const char *function, const char *file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ")" << function << " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}