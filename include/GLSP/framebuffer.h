/*
    This file is part of OpenGL-StarterPack (GLSP), an open source OpenGL based framework
    that facilitates and speeds up demo and project creation by offering an abstraction to
    the basic objects of OpenGL as well as incluing the necessary libraries.

    MIT License

    Copyright (c) 2024 Antonio Espinosa Garcia

*/
#ifndef __FRAMEBUFFER__
#define __FRAMEBUFFER__

#include <vector>
#include <GLSP/texture.h>

GLSP_NAMESPACE_BEGIN

class Renderbuffer; // Definition on the bottom

/*
Framebuffer attachment data
*/
struct Attachment
{
    // Necessary data for texture based attachment
    Texture *texture{nullptr};
    // Necessary data for renderbuffer based attachment
    Renderbuffer *renderbuffer{nullptr};
    // For color attachments
    unsigned int attachmentType{GL_COLOR_ATTACHMENT0};
    // Is a renderbuffer? Texture is unused
    bool isRenderbuffer{false};
};

/*
Abstraction of the OpenGL FBO.
*/
class Framebuffer
{

    unsigned int m_id;

    Extent2D m_extent;

    unsigned int m_samples;

    std::vector<Attachment> m_attachments;

    bool m_resizable{true};

    bool m_generated{false};

public:
    Framebuffer(Extent2D extent, std::vector<Attachment> attachments, unsigned int samples = 1) : m_extent(extent), m_attachments(attachments), m_samples(samples) {}
    ~Framebuffer() { cleanup(); }

    inline unsigned int get_id() const { return m_id; }

    inline Extent2D get_extent() const { return m_extent; }

    void set_extent(Extent2D extent);

    inline bool get_resizable() const { return m_resizable; }

    inline void set_resizable(bool o) { m_resizable = o; }

    void resize(Extent2D extent);

    std::vector<Attachment> get_attachments() const { return m_attachments; }

    /*
    Generates the OpenGl FBO
    */
    void generate();

    inline bool is_generated() const { return m_generated; }

    void bind() const;

    void unbind() const;

    inline void cleanup() { GL_CHECK(glDeleteFramebuffers(1, &m_id)); }

    /*
    Copy source framebuffer data to the destiny framebuffer. If framebuffer pointer is set to null,
    it will use the defalt framebuffer
    */
    static void blit(const Framebuffer *const src, const Framebuffer *const dst, unsigned int mask, unsigned int filter,
                     Extent2D srcExtent, Extent2D dstExtent,
                     Position2D srcOrigin = {0, 0}, Position2D dstOrigin = {0, 0});

    static void bind_default();

    static void clear_color_bit();

    static void clear_color_depth_bit();

    static void clear_depth_bit();

    static void clear_bits(unsigned int bits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    static void set_clear_color(glm::vec4 color);
};

class Renderbuffer
{
    unsigned int m_id;
    unsigned int m_interalFormat;
    bool m_generated{false};

public:
    Renderbuffer(unsigned int internatFormat)
        : m_interalFormat(internatFormat){};

    ~Renderbuffer()
    {
        GL_CHECK(glDeleteRenderbuffers(1, &m_id));
    };

    /*
    Generates the OpenGL RB0
    */
    void generate();

    inline unsigned int get_id() const { return m_id; }

    inline unsigned int get_internal_format() const { return m_interalFormat; }

    inline bool is_generated() const { return m_generated; }

    void bind() const;

    void unbind() const;
};

GLSP_NAMESPACE_END

#endif