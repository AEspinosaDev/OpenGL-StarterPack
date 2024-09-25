#ifndef __EXAMPLE__
#define __EXAMPLE__

#include <filesystem>

#include <GLSP/glsp.h>


USING_NAMESPACE_GLSP

class Application : public Renderer
{
public:
    Application(const char *title) : Renderer(title) {}
private:

    Camera *m_camera;
    Controller *m_controller;
    Mesh *m_mesh;

    void init();
    void update();
    void draw();

    void setup_user_interface_frame();
    void setup_window_callbacks();

    void key_callback(GLFWwindow *w, int a, int b, int c, int d);
    void mouse_callback(GLFWwindow *w, double x, double y);
    void resize_callback(GLFWwindow *w, int width, int height);
};

#endif
