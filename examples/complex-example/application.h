#ifndef __APP__
#define __APP__

#include <filesystem>

#include <GLSP/glsp.h>

USING_NAMESPACE_GLSP

class Application : public Renderer
{
public:
    Application(const char *title) : Renderer(title) {}
    Application(Window window) : Renderer(window) {}

private:
#pragma region SCENE

    Camera *m_camera;
    Controller *m_controller;

    /*
   Structurte representing all data needed for managing the scene
   */
    struct SceneSettings
    {
        glm::vec3 ambientColor{glm::vec4(1.0f)};
        float ambientIntensity{0.5f};
        glm::vec3 lightPos{5.0f, 10.0f, 5.0f};
        float lightIntensity{1.0f};
        glm::vec3 lightColor{glm::vec3(1.0f)};
        float fogIntensity{10.0f};
        glm::vec4 background{0.56f, 0.79f, 0.88f, 1.0f};
    };
    /*
    Structurte representing all data needed for drawing and managing the bird flock
    */
    struct BirdFlock
    {
        Mesh *mesh{nullptr};
        float birdSize{2.f};
        float wingLength{0.9};
        float wingSpeed{2.8f};
        float speed{1.4f};
    };
     /*
    Structurte representing all data needed for drawing and managing the water surface
    */
    struct Water{
        Mesh *mesh{nullptr};
        bool dynamicResolution{true};
        float maxResolution{64};
        float minResolution{16};
        float maxDistance{30};
        float minDistance{8};
        float heightPower{0.01};
        unsigned int waveTextureSize = 2048;
        glm::vec3 tint{0.2,0.59,0.66};
    };

    SceneSettings m_scene{};

    BirdFlock m_seagulls{};

    Water m_water{};

    Mesh *m_terrain;

    Mesh* m_vignette;

    Mesh* m_boat;



#pragma endregion
#pragma region GRAPHICS

    struct CameraUniforms
    {
        glm::mat4 vp;
        glm::mat4 v;
    };
    struct GlobalUniforms
    {
        glm::vec4 ambient; //w intensity
        glm::vec4 lightPos; // w intensity
        glm::vec4 lightColor; //w fogIntensity
    };

    enum UBOLayout
    {
        CAMERA_LAYOUT = 0,
        GLOBAL_LAYOUT = 1,
    };
    UniformBuffer *m_cameraUBO;
    UniformBuffer *m_globalUBO;

    GraphicPipeline m_wavePipeline{};  //No need to embed it in a material, low level functionality
    ComputeShader* m_compute;

    Framebuffer *m_multisampledFBO;
    Framebuffer *m_noiseFBO;

#pragma endregion

    void init();
    void update();
    void draw();

    void setup_user_interface_frame();
    void setup_window_callbacks();

    void key_callback(GLFWwindow *w, int a, int b, int c, int d);
    void mouse_callback(GLFWwindow *w, double x, double y);
    void resize_callback(GLFWwindow *w, int width, int height);

    void update_uniforms();
};

#endif
