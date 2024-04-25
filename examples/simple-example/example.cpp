#include "example.h"

void Application::init()
{
    Renderer::init();

    m_camera = new Camera(m_window.extent.width, m_window.extent.height, {0.0f, .5f, -5.0f});
    m_controller = new Controller(m_camera);

    m_mesh = new Mesh();
    loaders::load_OBJ(m_mesh, RESOURCES_PATH "meshes/cube.obj");

    GraphicPipeline unlitPipeline{};
    unlitPipeline.shader = new Shader(RESOURCES_PATH "shaders/basic.glsl", ShaderType::UNLIT);
    Material *material = new Material(unlitPipeline);
    m_mesh->set_material(material);
}

void Application::update()
{
    if (!user_interface_wants_to_handle_input())
        m_controller->handle_keyboard(m_window.ptr, 0, 0, m_time.delta);

    const float DELTA_DEG = 45.0F;
    m_mesh->set_rotation({0.0f, m_mesh->get_rotation().y + DELTA_DEG * m_time.delta, 0.0f});
}

void Application::draw()
{
    Framebuffer::clear_color_depth_bit();

    resize_viewport(m_window.extent);

    MaterialUniforms uniforms;
    uniforms.mat4Types["u_MVP"] = m_camera->get_projection() * m_camera->get_view() * m_mesh->get_model_matrix();
    uniforms.vec3Types["u_color"] = glm::vec3(1.0);
    m_mesh->get_material()->set_uniforms(uniforms);

    m_mesh->draw();
}

void Application::setup_user_interface_frame()
{
    Renderer::setup_user_interface_frame();
    ImGui::NewFrame();
    ImGui::ShowDemoWindow(&m_settings.userInterface);
    ImGui::Render();
}

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
 void  Application::key_callback(GLFWwindow *w, int a, int b, int c, int d)
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
    void  Application::mouse_callback(GLFWwindow *w, double x, double y)
    {
        if (!user_interface_wants_to_handle_input())
            m_controller->handle_mouse(w, x, y);
    }
    void  Application::resize_callback(GLFWwindow *w, int width, int height)
    {
        m_camera->set_projection(width, height);
        resize({width, height});
    }

