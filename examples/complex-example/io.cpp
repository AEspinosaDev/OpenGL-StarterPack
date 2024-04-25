#include "application.h"

void Application::setup_window_callbacks()
{
    glfwSetWindowUserPointer(m_window.ptr, this);

    glfwSetKeyCallback(m_window.ptr, [](GLFWwindow *w, int key, int scancode, int action, int mods)
                       { static_cast<Application *>(glfwGetWindowUserPointer(w))->key_callback(w, key, scancode, action, mods); });
    glfwSetCursorPosCallback(m_window.ptr, [](GLFWwindow *w, double x, double y)
                             { static_cast<Application *>(glfwGetWindowUserPointer(w))->mouse_callback(w, x, y); });
    glfwSetFramebufferSizeCallback(m_window.ptr, [](GLFWwindow *w, int width, int height)
                                   { static_cast<Application *>(glfwGetWindowUserPointer(w))->resize_callback(w, width, height); });
}
void Application::key_callback(GLFWwindow *w, int a, int b, int c, int d)
{
    if (glfwGetKey(m_window.ptr, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        m_window.should_close(true);
    }

    if (glfwGetKey(m_window.ptr, GLFW_KEY_F11) == GLFW_PRESS)
    {
        m_window.set_fullscreen(m_window.fullscreen ? false : true);
    }
}
void Application::mouse_callback(GLFWwindow *w, double x, double y)
{
    if (!user_interface_wants_to_handle_input())
        m_controller->handle_mouse(w, x, y);
}
void Application::resize_callback(GLFWwindow *w, int width, int height)
{
    m_camera->set_projection(width, height);
    resize({width, height});
    m_multisampledFBO->resize({width, height});
}


