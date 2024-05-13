/*
    This file is part of OpenGL-StarterPack (GLSP), an open source OpenGL based framework
    that facilitates and speeds up demo and project creation by offering an abstraction to 
    the basic objects of OpenGL as well as incluing the necessary libraries. 

    MIT License

	Copyright (c) 2024 Antonio Espinosa Garcia

*/
#include <GLSP/material.h>

GLSP_NAMESPACE_BEGIN

void Material::bind() const

{
    m_pipeline.shader->bind();
    setup_pipeline();
    upload_uniforms();
    bind_textures();
}

void Material::unbind() const
{
    unbind_textures();
    m_pipeline.shader->unbind();
}

void Material::upload_uniforms() const
{

    for (auto &vec2u : m_uniforms.vec2Types)
    {
        m_pipeline.shader->set_vec2(vec2u.first.c_str(), vec2u.second);
    }
    for (auto &vec3u : m_uniforms.vec3Types)
    {
        m_pipeline.shader->set_vec3(vec3u.first.c_str(), vec3u.second);
    }
    for (auto &vec4u : m_uniforms.vec4Types)
    {
        m_pipeline.shader->set_vec4(vec4u.first.c_str(), vec4u.second);
    }
    for (auto &mat4u : m_uniforms.mat4Types)
    {
        m_pipeline.shader->set_mat4(mat4u.first.c_str(), mat4u.second);
    }
    for (auto &fu : m_uniforms.floatTypes)
    {
        m_pipeline.shader->set_float(fu.first.c_str(), fu.second);
    }
    for (auto &iu : m_uniforms.intTypes)
    {
        m_pipeline.shader->set_int(iu.first.c_str(), iu.second);
    }
    for (auto &bu : m_uniforms.boolTypes)
    {
        m_pipeline.shader->set_bool(bu.first.c_str(), bu.second);
    }
}

void Material::setup_pipeline() const
{
    Renderer::enable_face_cull(m_pipeline.state.cullFace);
    Renderer::enable_depth_test(m_pipeline.state.depthTest);
    Renderer::enable_depth_writes(m_pipeline.state.depthWrites);
    Renderer::set_depth_func(m_pipeline.state.depthFunction);

    if (m_pipeline.state.blending)
    {
        Renderer::enable_blend(true);
        Renderer::set_blend_func_separate(m_pipeline.state.blendingFuncSRC, m_pipeline.state.blendingFuncDST);
        Renderer::set_blend_op(m_pipeline.state.blendingOperation);
    }
    else
    {
        Renderer::enable_blend(false);
    }
}

void Material::bind_textures() const
{
    for (auto &textureData : m_textures)
    {
        textureData.second.texture->bind(textureData.second.slot);
    }
}

void Material::unbind_textures() const
{
    for (auto &textureData : m_textures)
    {
        textureData.second.texture->unbind();
    }
}

GLSP_NAMESPACE_END