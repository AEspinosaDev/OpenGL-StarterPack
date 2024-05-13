/*
    This file is part of OpenGL-StarterPack (GLSP), an open source OpenGL based framework
    that facilitates and speeds up demo and project creation by offering an abstraction to
    the basic objects of OpenGL as well as incluing the necessary libraries.

    MIT License

    Copyright (c) 2024 Antonio Espinosa Garcia

*/
#ifndef __RENDERER__
#define __RENDERER__

#include <GLSP/core.h>
#include <GLSP/utils.h>
#include <GLSP/framebuffer.h>

GLSP_NAMESPACE_BEGIN

/*
Stores window related data
*/
struct Window
{
    Extent2D extent{800, 600};
    Position2D position{50, 50};
    const char *title;
    GLFWwindow *ptr{nullptr};
    bool fullscreen{false};

    inline void set_fullscreen(bool op)
    {
        fullscreen = op;
        if (!fullscreen)
        {
            glfwSetWindowMonitor(ptr, NULL, position.x, position.y, extent.width, extent.height, GLFW_DONT_CARE);
        }
        else
        {
            const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwSetWindowMonitor(ptr, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
        }
    }
    inline void should_close(bool op)
    {
        glfwSetWindowShouldClose(ptr, op);
    }
};

/*
OpenGL context creation settings
*/
struct ContextSettings
{
    int OpenGLMajor{4};
    int OpenGLMinor{6};
    int OpenGLProfile{GLFW_OPENGL_CORE_PROFILE};
};

struct RendererSettings
{
    bool vSync{true};
    int framerateCap{-1};
    bool userInterface{true};
    bool depthTest{true};
    bool depthWrites{true};
    bool blending{true};
};

/*
Core class. Implements all basic render functionality as the render loop and OpenGL context creation.
Should be inherited if user wants more complex functionality.
*/
class Renderer
{
protected:
    const ContextSettings m_context{};
    RendererSettings m_settings{};

    Window m_window{};

    struct Time
    {
        double delta{0.0};
        double last{0.0};
        double current{0.0};
        int framerate{0};
    };
    Time m_time{};

    utils::EventDispatcher m_cleanupQueue;

    void create_context();
    void tick();
    void cleanup();
    /*
    Override function in order to initiate desired funcitonality. Call parent function if want to use events functionality.
    */
    virtual void init();
    /*
   Override function in order to customize update.
   */
    virtual void update();
    /*
   Override function in order to customize render funcitonality.
   */
    virtual void draw();
    /*
    Setup GLFW window callbacks here
     */
    virtual void setup_window_callbacks();

public:
    Renderer(const char *title) { m_window.title = title; }
    Renderer(Window &window) { m_window = window; }
    Renderer(Window &window, ContextSettings &contextSettings, RendererSettings &settings) : m_window(window), m_context(contextSettings), m_settings(settings) {}

    void run();

#pragma region GETTERS & SETTERS
    inline RendererSettings get_settings() const
    {
        return m_settings;
    }
    inline void set_settings(RendererSettings &s)
    {
        m_settings = s;
    }
    inline Time get_time() const
    {
        return m_time;
    }
    inline void set_v_sync(bool op)
    {
        glfwSwapInterval(op);
        m_settings.vSync = op;
    }
#pragma endregion
    /*
    Use as callback
    */
    inline virtual void resize(Extent2D extent, Position2D origin = {0, 0})
    {
        m_window.extent = extent;
        Renderer::resize_viewport(extent, origin);
    }
    inline static void resize_viewport(Extent2D extent, Position2D origin = {0, 0})
    {
        GL_CHECK(glViewport(origin.x, origin.y, extent.width, extent.height));
    }
    inline static void enable_depth_test(bool op)
    {
        op ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
    }

    inline static void enable_depth_writes(bool op)
    {
        GL_CHECK(glDepthMask(op));
    }
    inline static void set_depth_func(DepthFuncType func)
    {
        GL_CHECK(glDepthFunc(func));
    }
    inline static void enable_blend(bool op)
    {
        op ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    }
    inline static void set_blend_func(BlendFuncType src, BlendFuncType dst)
    {
        GL_CHECK(glBlendFunc(src, dst));
    }
    inline static void set_blend_func_separate(BlendFuncType src, BlendFuncType dst, BlendFuncType srcA = ONE, BlendFuncType dstA = ONE)
    {
        GL_CHECK(glBlendFuncSeparate(src, dst, srcA, dstA));
    }
    inline static void set_blend_func_separate(Framebuffer *fbo, BlendFuncType src, BlendFuncType dst, BlendFuncType srcA = ONE, BlendFuncType dstA = ONE)
    {
        GL_CHECK(glBlendFuncSeparatei(fbo->get_id(), src, dst, srcA, dstA));
    }
    inline static void set_blend_op(BlendOperationType op)
    {
        glBlendEquation(ADD);
    }
    inline static void enable_face_cull(bool op)
    {
        glCullFace(op);
    }
    inline static void set_polygon_mode(unsigned int face = GL_FRONT_AND_BACK, unsigned int mode = GL_FILL)
    {
        GL_CHECK(glPolygonMode(face, mode));
    }

#pragma region USER INTERFACE
    inline bool user_interface_wants_to_handle_input()
    {
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse || io.WantCaptureKeyboard)
            return true;
        else
            return false;
    }
    /*
    Override if custom init functionality needed. Predetermined graphic user interface backend is IMGUI.
    */
    virtual void init_user_interface();
    /*
    Override to add IMGUI windows and widgets.Call ImGui::NewFrame at start and ImGui::Render at the end. Don't forget to call parent function for IMGUI frame set up
    */
    virtual void setup_user_interface_frame();

    virtual void upload_user_interface_render_data();

#pragma endregion
};
GLSP_NAMESPACE_END
#endif