#include "application.h"
#include <glm/gtc/random.hpp>

void Application::init()
{
#pragma region INIT
    Renderer::init();

    m_camera = new Camera(m_window.extent.width, m_window.extent.height, {2.0f, 5.0f, -30.0f});
    m_camera->set_far(200.0f);
    m_controller = new Controller(m_camera);

#pragma endregion;
#pragma region SHADER PIPELINES
    // Uniform buffers creation
    m_cameraUBO = new UniformBuffer(sizeof(CameraUniforms), UBOLayout::CAMERA_LAYOUT);
    m_cameraUBO->generate();

    m_globalUBO = new UniformBuffer(sizeof(GlobalUniforms), UBOLayout::GLOBAL_LAYOUT);
    m_globalUBO->generate();

    Material *birdMaterial;
    Material *terrainMaterial;
    Material *waterMaterial;
    Material *boatMaterial;

    GraphicPipeline flockPipeline{};
    flockPipeline.shader = new Shader(RESOURCES_PATH "shaders/flock.glsl", ShaderType::UNLIT);
    birdMaterial = new Material(flockPipeline);

    GraphicPipeline tessPipeline{};
    tessPipeline.shader = new Shader(RESOURCES_PATH "shaders/surface-tess.glsl", ShaderType::LIT);
    GraphicPipeline tess1Pipeline{};
    tess1Pipeline.shader = tessPipeline.shader;
    tess1Pipeline.state.blending = true;
    terrainMaterial = new Material(tessPipeline);
    waterMaterial = new Material(tess1Pipeline);

    GraphicPipeline PBRPipeline{};
    PBRPipeline.shader = new Shader(RESOURCES_PATH "shaders/cook-torrance.glsl", ShaderType::LIT);
    boatMaterial = new Material(PBRPipeline);

    m_wavePipeline.shader = new Shader(RESOURCES_PATH "shaders/noise-gen.glsl", ShaderType::UNLIT);

    m_compute = new ComputeShader(RESOURCES_PATH "shaders/compute-path.glsl");

#pragma endregion;
#pragma region FBOs

    // Creating multisampled forward pass buffer
    TextureConfig masaaColorConfig{};
    masaaColorConfig.type = TextureType::TEXTURE_2D_MULTISAMPLE;
    masaaColorConfig.format = GL_RGBA;
    masaaColorConfig.internalFormat = GL_RGBA16;
    masaaColorConfig.dataType = GL_UNSIGNED_BYTE;

    Attachment msaaColorAttachment{};
    msaaColorAttachment.texture = new Texture(m_window.extent, masaaColorConfig);
    msaaColorAttachment.attachmentType = GL_COLOR_ATTACHMENT0;
    Attachment msaaDepthAttachment{};
    msaaDepthAttachment.isRenderbuffer = true;
    msaaDepthAttachment.renderbuffer = new Renderbuffer(GL_DEPTH24_STENCIL8);
    msaaDepthAttachment.attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;

    m_multisampledFBO = new Framebuffer(m_window.extent, {msaaColorAttachment, msaaDepthAttachment}, 8);
    m_multisampledFBO->generate();

    // Create wave compute prepass
    TextureConfig noiseTextConfig{};
    noiseTextConfig.format = GL_RED;
    noiseTextConfig.internalFormat = GL_R8;
    noiseTextConfig.useMipmaps = false;

    Attachment noiseAttachment{};
    Extent2D noiseExtent{(int)m_water.waveTextureSize, (int)m_water.waveTextureSize};
    noiseAttachment.texture = new Texture(noiseExtent, noiseTextConfig);
    noiseAttachment.attachmentType = GL_COLOR_ATTACHMENT0;

    m_noiseFBO = new Framebuffer(noiseExtent, {noiseAttachment});
    m_noiseFBO->generate();

#pragma endregion
#pragma region MESH SETUP

    // ----------- BIRDS --------------
    const size_t NUM_SEAGULLS = 30;
    std::vector<float> seagullPos;
    seagullPos.reserve(NUM_SEAGULLS * 4);
    std::vector<float> seagullUp;
    seagullUp.reserve(NUM_SEAGULLS * 4);
    std::vector<float> seagullForward;
    seagullForward.reserve(NUM_SEAGULLS * 4);
    std::vector<float> seagullVarianze;
    seagullVarianze.reserve(NUM_SEAGULLS * 4);
    for (size_t i = 0; i < NUM_SEAGULLS; i++)
    {
        // Generate random positions for birds
        glm::vec3 pos = glm::linearRand(glm::vec3(-20.f, -8.0f, -20.f), glm::vec3(20.f, 8.0f, 20.f));
        // Pos buffer
        seagullPos.push_back(pos.x);
        seagullPos.push_back(pos.y);
        seagullPos.push_back(pos.z);
        seagullPos.push_back(1.0f);
        // Up buffer
        seagullUp.push_back(0.0f);
        seagullUp.push_back(1.0f);
        seagullUp.push_back(0.0f);
        seagullUp.push_back(0.0f);
        // Forward buffer
        seagullForward.push_back(0.0f);
        seagullForward.push_back(0.0f);
        seagullForward.push_back(1.0f);
        seagullForward.push_back(0.0f);
        // Deviation buffer
        seagullVarianze.push_back(glm::linearRand(0.0,0.5)); //For speed
        seagullVarianze.push_back(glm::linearRand(0.0,2.0)); //For wings
        seagullVarianze.push_back(glm::linearRand(0.7,1.0)); //For color
        seagullVarianze.push_back(0.0f); //Unused
    }
    m_seagulls.mesh = new Mesh();

    // Not interleaved attributes because compute shader doesnt handle them well as storage buffers
    const size_t BYTES = seagullPos.size() * sizeof(float);
    
    VertexBuffer posVBO(seagullPos.data(), BYTES);
    posVBO.push_attribute_layout<float>(4); // 4 as vec4
    VertexBuffer upVBO(seagullUp.data(), BYTES);
    upVBO.push_attribute_layout<float>(4);
    VertexBuffer forwardVBO(seagullForward.data(), BYTES);
    forwardVBO.push_attribute_layout<float>(4);
    VertexBuffer varianzeVBO(seagullVarianze.data(), BYTES);
    varianzeVBO.push_attribute_layout<float>(4);

    VertexArray flockVAO;
    flockVAO.push_vertex_buffer(posVBO);
    flockVAO.push_vertex_buffer(upVBO);
    flockVAO.push_vertex_buffer(forwardVBO);
    flockVAO.push_vertex_buffer(varianzeVBO);

    m_seagulls.mesh->set_geometry(new Geometry(flockVAO, NUM_SEAGULLS, GL_POINTS));

    m_seagulls.mesh->set_material(birdMaterial);
    m_seagulls.mesh->set_position({-2.0f, 18.0f, 0.0f});
    m_seagulls.mesh->set_rotation({0.0f, 145.0f, 0.0f});

    // Setting textures for the birds
    Texture *seagullBody = new Texture();
    loaders::load_image(seagullBody, RESOURCES_PATH "textures/seagull.png");
    seagullBody->generate();
    birdMaterial->set_texture("u_bodyT", seagullBody);
    Texture *seagullWing = new Texture();
    loaders::load_image(seagullWing, RESOURCES_PATH "textures/wing.png");
    seagullWing->generate();
    birdMaterial->set_texture("u_wingT", seagullWing, 1);

    // ---------------- Terrain --------------
    m_terrain = new Mesh();
    m_terrain = Mesh::create_quad({40, 40}, GL_PATCHES);
    m_terrain->set_material(terrainMaterial);
    m_terrain->set_scale(100.0f);
    Texture *heightTerrainTexture = new Texture();
    loaders::load_image(heightTerrainTexture, RESOURCES_PATH "textures/heightTerrain.png");
    heightTerrainTexture->generate();
    terrainMaterial->set_texture("u_heightMap", heightTerrainTexture);
    TextureConfig jpgConfig{};
    jpgConfig.format = GL_RGB;
    jpgConfig.internalFormat = GL_RGB16;
    Texture *terrainTexture = new Texture(jpgConfig);
    loaders::load_image(terrainTexture, RESOURCES_PATH "textures/terrain.jpg");
    terrainTexture->generate();
    terrainMaterial->set_texture("u_albedoT", terrainTexture, 1);

    // ----------- WATER --------------
    m_water.mesh = new Mesh();
    m_water.mesh = Mesh::create_quad({40, 40}, GL_PATCHES);
    m_water.mesh->set_material(waterMaterial);
    m_water.mesh->set_scale(100.0f);
    m_water.mesh->set_position({0.0f, 1.1f, 0.0f});
    waterMaterial->set_texture("u_heightMap", m_noiseFBO->get_attachments().front().texture);

    // ---------- VIGNETTE --------
    m_vignette = Mesh::create_screen_quad();

    // --------- BOAT ---------
    m_boat = new Mesh();
    loaders::load_OBJ(m_boat, RESOURCES_PATH "meshes/boat.obj");
    m_boat->set_material(boatMaterial);
    Texture *boatTexture = new Texture();
    loaders::load_image(boatTexture, RESOURCES_PATH "textures/boatColor.png");
    boatTexture->generate();
    boatMaterial->set_texture("u_albedoT", boatTexture);
    m_boat->set_scale(0.3);
    m_boat->set_position({0.2, 0.95, -16.0});
    m_boat->set_rotation({0.0f, 45.0f, 0.0f});

#pragma endregion
}

void Application::update()
{
    // Handle KB IO
    if (!user_interface_wants_to_handle_input())
        m_controller->handle_keyboard(m_window.ptr, 0, 0, m_time.delta);

    update_uniforms();

    m_compute->bind();
    if (m_seagulls.mesh->get_geometry()->is_buffer_loaded())
    {
        VertexArray seagullsVAO = m_seagulls.mesh->get_geometry()->get_VAO();
        seagullsVAO.get_vertex_buffers().front().bind_base(GL_SHADER_STORAGE_BUFFER, 0); // Bind position vbo
        seagullsVAO.get_vertex_buffers()[2].bind_base(GL_SHADER_STORAGE_BUFFER, 1);  // Bind forward vbo
        seagullsVAO.get_vertex_buffers().back().bind_base(GL_SHADER_STORAGE_BUFFER, 2);  // Bind velosity diff vbo
        Extent3D workgroups{(int)m_seagulls.mesh->get_geometry()->get_vertex_count(),
                            1,
                            1};
        m_compute->dispatch(workgroups, true, GL_SHADER_STORAGE_BARRIER_BIT);
    }
    m_compute->unbind();
}

#pragma region DRAWING
void Application::draw()
{
    // Prepass for computing noise and store it in a texture
    m_noiseFBO->bind();
    Framebuffer::set_clear_color(glm::vec4(0.0f));
    Framebuffer::clear_color_bit();
    Renderer::enable_blend(false);
    Renderer::resize_viewport({(int)m_water.waveTextureSize, (int)m_water.waveTextureSize});

    m_wavePipeline.shader->bind();
    m_vignette->draw(false);
    m_wavePipeline.shader->unbind();

    // Multisampled draw pass
    m_multisampledFBO->bind();
    Framebuffer::set_clear_color(m_scene.background);
    Framebuffer::clear_color_depth_bit();
    Renderer::resize_viewport(m_window.extent);

    m_terrain->draw();
    m_water.mesh->draw();
    m_seagulls.mesh->draw();
    m_boat->draw();

    // Blit msaa to default backbuffer
    Framebuffer::blit(m_multisampledFBO, nullptr,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST, m_window.extent, m_window.extent);
}
#pragma endregion
#pragma region UNIFORM UPDATE
void Application::update_uniforms()
{

    // Setup UBOs
    CameraUniforms camu;
    camu.vp = m_camera->get_projection() * m_camera->get_view();
    camu.v = m_camera->get_view();
    m_cameraUBO->upload_data(sizeof(CameraUniforms), &camu);

    GlobalUniforms globu;
    globu.ambient = {m_scene.ambientColor,
                     m_scene.ambientIntensity};
    glm::vec3 vLightPos = camu.v * glm::vec4(m_scene.lightPos, 1.0);
    globu.lightPos = {vLightPos, m_scene.lightIntensity};
    globu.lightColor = {m_scene.lightColor, m_scene.fogIntensity};
    m_globalUBO->upload_data(sizeof(GlobalUniforms), &globu);

    // Update material uniforms
    MaterialUniforms terrainU;
    terrainU.mat4Types["u_model"] = m_terrain->get_model_matrix();
    terrainU.vec3Types["u_albedo"] = glm::vec3(1.0);
    terrainU.floatTypes["u_height"] = 1.5;
    terrainU.boolTypes["u_useAlbedoT"] = true;
    terrainU.floatTypes["u_opacity"] = 1.0f;
    terrainU.floatTypes["u_roughness"] = 0.8f;
    terrainU.floatTypes["u_metalness"] = 0.0f;
    terrainU.floatTypes["u_maxResolution"] = 64;
    terrainU.floatTypes["u_minResolution"] = 16;
    terrainU.boolTypes["u_useDynamicRes"] = false;
    terrainU.floatTypes["u_maxDistance"] = m_water.maxDistance;
    terrainU.floatTypes["u_minDistance"] = m_water.minDistance;
    terrainU.vec2Types["u_tile0"] = glm::vec2(1.0f);
    terrainU.vec2Types["u_tile1"] = glm::vec2(10.0f);
    m_terrain->get_material()->set_uniforms(terrainU);
    MaterialUniforms flockU;
    flockU.mat4Types["u_model"] = m_seagulls.mesh->get_model_matrix();
    flockU.floatTypes["u_birdSize"] = m_seagulls.birdSize;
    flockU.floatTypes["u_wingLength"] = m_seagulls.wingLength;
    flockU.floatTypes["u_time"] = m_time.current;
    flockU.floatTypes["u_angularSpeed"] = m_seagulls.wingSpeed;
    m_seagulls.mesh->get_material()->set_uniforms(flockU);
    MaterialUniforms seaU;
    seaU.mat4Types["u_model"] = m_water.mesh->get_model_matrix();
    seaU.floatTypes["u_height"] = m_water.heightPower;
    seaU.floatTypes["u_maxResolution"] = m_water.maxResolution;
    seaU.floatTypes["u_minResolution"] = m_water.minDistance;
    seaU.floatTypes["u_maxDistance"] = m_water.maxDistance;
    seaU.floatTypes["u_minDistance"] = m_water.minDistance;
    seaU.vec3Types["u_albedo"] = m_water.tint;
    seaU.floatTypes["u_roughness"] = 0.2f;
    seaU.floatTypes["u_metalness"] = 0.5f;
    seaU.floatTypes["u_opacity"] = 0.7f;
    seaU.boolTypes["u_useAlbedoT"] = false;
    seaU.boolTypes["u_useDynamicRes"] = m_water.dynamicResolution;
    seaU.vec2Types["u_tile0"] = glm::vec2(2.0f);
    seaU.vec2Types["u_tile1"] = glm::vec2(1.0f);
    m_water.mesh->get_material()->set_uniforms(seaU);
    MaterialUniforms boatU;
    boatU.mat4Types["u_model"] = m_boat->get_model_matrix();
    boatU.vec3Types["u_albedo"] = glm::vec3(1.0f);
    boatU.floatTypes["u_roughness"] = 0.8f;
    boatU.floatTypes["u_metalness"] = 0.0f;
    boatU.floatTypes["u_opacity"] = 1.0f;
    boatU.boolTypes["u_useAlbedoT"] = true;
    boatU.vec2Types["u_tile"] = glm::vec2(1.0f);
    m_boat->get_material()->set_uniforms(boatU);

    m_wavePipeline.shader->bind();
    m_wavePipeline.shader->set_float("u_time", m_time.current);
    m_wavePipeline.shader->unbind();

    m_compute->bind();
    m_compute->set_float("u_time", m_time.current);
    m_compute->set_float("u_speed", m_seagulls.speed);
    m_compute->unbind();
#pragma endregion
}
