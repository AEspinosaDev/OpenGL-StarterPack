/*
    This file is part of OpenGL-StarterPack (GLSP), an open source OpenGL based framework
    that facilitates and speeds up demo and project creation by offering an abstraction to 
    the basic objects of OpenGL as well as incluing the necessary libraries. 

    MIT License

	Copyright (c) 2024 Antonio Espinosa Garcia

*/
#include <GLSP/renderer.h>

GLSP_NAMESPACE_BEGIN

void Renderer::run()
{
    create_context();
    init();
    tick();
    cleanup();
}

void Renderer::create_context()
{
    if (!glfwInit())
    {
        GLFW_CHECK();
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_context.OpenGLMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_context.OpenGLMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, m_context.OpenGLProfile);

    m_window.ptr = glfwCreateWindow(m_window.extent.width, m_window.extent.height, m_window.title, NULL, NULL);
    if (!m_window.ptr)
    {
        glfwTerminate();
        GLFW_CHECK();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(m_window.ptr);

     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(m_window.ptr);
        glfwTerminate();
        throw new GLSPException("Failed to initialize OpenGL context\n");
    }

    glfwSwapInterval(m_settings.vSync);
}

void Renderer::init()
{
    setup_window_callbacks();

    Renderer::enable_depth_test(m_settings.depthTest);
    Renderer::enable_depth_writes(m_settings.depthWrites);

    if (m_settings.userInterface)
    {
        init_user_interface();
    }
}

void Renderer::tick()
{
    while (!glfwWindowShouldClose(m_window.ptr))
    {

        m_time.current = glfwGetTime();
        m_time.delta = m_time.current - m_time.last;
        m_time.last = m_time.current;
        m_time.framerate = int(1.0 / m_time.delta);

        update();

        if (m_settings.userInterface)
            setup_user_interface_frame();

        draw();

        if (m_settings.userInterface)
            upload_user_interface_render_data();

        glfwSwapBuffers(m_window.ptr);

        glfwPollEvents();
    }
}

void Renderer::update()
{
}

void Renderer::draw()
{
}

void Renderer::setup_window_callbacks()
{
}

void Renderer::init_user_interface()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL(m_window.ptr, true);

    ImGui_ImplOpenGL3_Init("#version 460");

    m_cleanupQueue.push_function([=]
                                 { ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(); });
}

void Renderer::setup_user_interface_frame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}

void Renderer::upload_user_interface_render_data()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::cleanup()
{
    if (!m_cleanupQueue.functions.empty())
        m_cleanupQueue.flush();

    glfwDestroyWindow(m_window.ptr);
    glfwTerminate();
}
GLSP_NAMESPACE_END
