#if defined(__wii__) || defined(__gamecube__)

#include "opengx_shaders.h"

#define GL_GLEXT_PROTOTYPES

#include "/home/mardy/src/git/gamedev/opengx/src/arrays.h"
#include <SDL_opengl.h>
#include <algorithm>
#include <gccore.h>
#include <math.h>
#include <opengx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint32_t s_hash_loading = 0x3c0a04dd;
static const uint32_t s_hash_spheredefault = 0x48ee46db;
static const uint32_t s_hash_spheretemplate = 0xf56bd8f0;
static const uint32_t s_hash_line = 0x3a114bc7;
static const uint32_t s_hash_effect = 0x5aec570a;
static const uint32_t s_hash_font = 0x00b80591;
static const uint32_t s_hash_pageindicator = 0xfbba48b3;
static const uint32_t s_hash_effect_overlay = 0x3c996b1b;
static const uint32_t s_hash_effect_overlay_template = 0x8acf8703;
static const uint32_t s_hash_background = 0xf5e0a345;
static const uint32_t s_hash_background_template = 0x64221bbd;
static const uint32_t s_hash_icons = 0x8c3b995f;
static const uint32_t s_hash_icons_fsh = 0x3588f7f6;
static const uint32_t s_hash_decal = 0x8c3b995f;
static const uint32_t s_hash_decal_fsh = 0x8b8cf157;
static const uint32_t s_hash_shadow = 0x868fd390;
static const uint32_t s_hash_level_preview = 0x8c3b995f;
static const uint32_t s_hash_cached_screen = 0x8618793a;

static uint8_t s_func_1D_texels[256] ATTRIBUTE_ALIGN(32);

static inline GXColor gxcol_new_fv(const float *components)
{
    GXColor c = {
        (u8)(components[0] * 255.0f),
        (u8)(components[1] * 255.0f),
        (u8)(components[2] * 255.0f),
        (u8)(components[3] * 255.0f)
    };
    return c;
}

static void prepare_I8_texture(uint8_t *dst, const uint8_t *texels, int w, int h)
{
    int n_blocks_x = (w + 7) / 8;
    int n_blocks_y = (h + 3) / 4;
    /* This code assumes that the texels array size is a multiple of 32 */
    for (int block_y = 0; block_y < n_blocks_y; block_y++) {
        const uint8_t *block_row = texels + block_y * n_blocks_x * 32;
        for (int block_x = 0; block_x < n_blocks_x; block_x++) {
            for (int y = 0; y < 4; y++) {
                memcpy(dst, block_row + w * y + block_x * 8, 8);
                dst += 8;
            }
        }
    }
}

static void set_pixel_to_texture_ARGB(uint8_t *dst, GXColor color,
                                      int x, int y, int w)
{
    /* Code adapted from SDL2's libogc port */
    int16_t tex_pitch = (w + 3) / 4 * 4;
    u32 offset = (((y >> 2) << 4) * tex_pitch) +
        ((x >> 2) << 6) + (((y % 4 << 2) + x % 4) << 1);
    dst[offset] = color.a;
    dst[offset + 1] = color.r;
    dst[offset + 32] = color.g;
    dst[offset + 33] = color.b;
}

static void set_pixel_to_texture_I8(uint8_t *dst, uint8_t color,
                                    int x, int y, int w)
{
    /* Code adapted from SDL2's libogc port */
    int16_t tex_pitch = (w + 7) / 8 * 8;
    u32 offset = ((y & ~3) * tex_pitch) + ((x & ~7) << 2) + ((y & 3) << 3) + (x & 7);
    dst[offset] = color;
}

static float smoothstep(float edge0, float edge1, float x)
{
    float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

static float length(float v_x, float v_y)
{
    return sqrtf(v_x * v_x + v_y * v_y);
}

static void setup_projection(GLuint program, GLint projection_loc)
{
    float m[16];
    glGetUniformfv(program, projection_loc, m);
    /* The vertex shader receives the transposed matrix and to compensate that it computes
     * gl_Position as vector * matrix (instead of the usual matrix * vector).
     * We also need to compensate for this, so let's treat the matrix as if it
     * were a GX matrix (which is transposed in regard to OpenGL). */
    ogx_shader_set_projection_gx(reinterpret_cast<float (&)[4][4]>(*m));
}

static void setup_matrices_identity_proj(GLuint program, void *user_data)
{
    Mtx44 m;
    guMtx44Identity(m);
    ogx_shader_set_projection_gx(m);
}

#if 0
static void debug_matrix44(const Mtx44 m)
{
    fprintf(stderr,
            "%.3f %.3f %.3f %.3f\n"
            "\t%.3f %.3f %.3f %.3f\n"
            "\t%.3f %.3f %.3f %.3f\n"
            "\t%.3f %.3f %.3f %.3f\n",
            m[0][0], m[0][1], m[0][2], m[0][3],
            m[1][0], m[1][1], m[1][2], m[1][3],
            m[2][0], m[2][1], m[2][2], m[2][3],
            m[3][0], m[3][1], m[3][2], m[3][3]);
}

static void debug_matrix_gl(const float *m)
{
    fprintf(stderr,
            "%.3f %.3f %.3f %.3f\n"
            "\t%.3f %.3f %.3f %.3f\n"
            "\t%.3f %.3f %.3f %.3f\n"
            "\t%.3f %.3f %.3f %.3f\n",
            m[0], m[4], m[8], m[12],
            m[1], m[5], m[9], m[13],
            m[2], m[6], m[10], m[14],
            m[3], m[7], m[11], m[15]);
}
#endif

struct ShaderLoading {
    GLint projection_loc;
    GLint color_loc;
};

static void setup_draw_loading(GLuint program, const OgxDrawData *draw_data,
                               void *user_data)
{
    auto *data = static_cast<ShaderLoading*>(user_data);
    setup_projection(program, data->projection_loc);

    Mtx mv;
    guMtxIdentity(mv);
    ogx_shader_set_modelview_gx(mv);
#if 0
    /* FIXME: this breaks everything on a wii! */
    GX_SetCurrentMtx(GX_IDENTITY);
#endif

    float colorf[4];
    glGetUniformfv(program, data->color_loc, colorf);

    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;

    GXColor color = gxcol_new_fv(colorf);
    GX_SetNumChans(1);
    GX_SetChanMatColor(GX_COLOR0A0, color);
    GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG,
                   0, GX_DF_NONE, GX_AF_NONE);

    GX_SetTevOrder(stage, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(stage, GX_PASSCLR);
}

static void draw_done_loading(GLuint program, void *user_data)
{
    //GX_DrawDone(); //ALSO BREAKS EVERYTHING
}

struct ShaderSphereDefault {
    GLint projection_loc;
    GLint sampler_loc;
    uint8_t first_stage;
};

static void setup_draw_sphere_default(GLuint program, const OgxDrawData *draw_data,
                                      void *user_data)
{
    auto *data = static_cast<ShaderSphereDefault*>(user_data);
    setup_projection(program, data->projection_loc);
    GX_SetCurrentMtx(GX_IDENTITY);

    GLint texture_unit;
    glGetUniformiv(program, data->sampler_loc, &texture_unit);

    uint8_t tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    uint8_t tex_mtx = GX_TEXMTX0 + ogx_gpu_resources->texmtx_first++ * 3;
    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    data->first_stage = stage; /* For the cleanup function */
    GXTexObj *texture = ogx_shader_get_texobj(texture_unit);
    GX_LoadTexObj(texture, tex_map);

    /* We would really like to use an identity matrix, but unfortunately
     * (rounding issues?) the subtexture includes a row of texels from the
     * sphere at the left. This results in dark pixels drawn on the bottom left
     * of the sphere. */
    Mtx texm;
    guMtxIdentity(texm);
    texm[0][3] = 0.0005f; /* translate to the right */
    GX_LoadTexMtxImm(texm, tex_mtx, GX_MTX2x4);
    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_TEX0, tex_mtx);

    GX_SetNumChans(2);
    GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX,
                   0, GX_DF_CLAMP, GX_AF_NONE);
    GX_SetChanCtrl(GX_COLOR1A1, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX,
                   0, GX_DF_CLAMP, GX_AF_NONE);

    /* The default configuration fits us well:
     * GX_TEV_SWAP1 maps r to rgb (rrr),
     * GX_TEV_SWAP2 maps g to rgb (ggg),
     * GX_TEV_SWAP3 maps b to rgb (bbb) */
    GX_SetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP2);
    // In data: c: Texture Color b: raster value, Operation: b*c
    GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
    GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLOR0A0);

    /* Second stage for the desired color */
    stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    GX_SetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP1);
    GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_CPREV);
    GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_APREV);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLOR1A1);

    /* Third stage for the shadow */
    stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    GX_SetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP3);
    GX_SetTevColorIn(stage, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GX_SetTevAlphaIn(stage, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLORNULL);
}

static void draw_done_sphere_default(GLuint program, void *user_data)
{
    auto *data = static_cast<ShaderSphereDefault*>(user_data);
    GX_SetTevSwapMode(data->first_stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GX_SetTevSwapMode(data->first_stage + 1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GX_SetTevSwapMode(data->first_stage + 2, GX_TEV_SWAP0, GX_TEV_SWAP0);
    //GX_DrawDone();
}

struct ShaderSphereTemplate {
    GLint framebuffer_size_loc;
    GLint center_loc;
    GLint size_loc;
    uint8_t *texels;
};

static inline GXColor fragment_sphere_template(int x, int y, int size)
{
    /* We are translating spheretemplate.fsh into C code */
    // Distance of this point to the center
    int center = size;
    float dist = length(x - center, y - center);

    // Shadow effect
    float shadow_border = 0.005 * size;
    float center_plus_border = center + shadow_border;
    float shadow_dist = length(x - center_plus_border, y - center_plus_border);

    // Distance of this point to the highlight center
    float highlight = length(x - (center - 0.2 * size),
                             y - (center - 0.2 * size));

    // Border width for antialiased sphere border
    float border = 2.7;

    // Alpha value determines the shape of the sphere
    float base_color = smoothstep(size, size - border, dist);
    float color[4] = { base_color, base_color, base_color, base_color };
    if (shadow_dist < dist) {
        color[3] += smoothstep(size - shadow_border, size - border, shadow_dist);
    }

    // Intensity of highlight
    float intensity = 0.15;
    highlight = smoothstep(size, size / 2, highlight);

    // Color is based on original color + highlight effect
    color[2] *= intensity * highlight;

    float mixer = smoothstep(size / 2, size / 2 - border, dist);
    color[0] *= mixer;
    color[1] *= (1.0 - mixer);
    return gxcol_new_fv(color);
}

static void setup_draw_sphere_template(GLuint program,
                                       const OgxDrawData *draw_data,
                                       void *user_data)
{
    auto *data = static_cast<ShaderSphereTemplate*>(user_data);

    float center[2];
    glGetUniformfv(program, data->center_loc, center);

    float framebuffer_size[2];
    glGetUniformfv(program, data->framebuffer_size_loc, framebuffer_size);

    float size_f;
    glGetUniformfv(program, data->size_loc, &size_f);
    int size = size_f;

    /* For x and y, we have:
     * x = 2.0 * center.x / framebuffer_size.x
     *   + 2.0 * vtx.x * size / framebuffer_size.x
     *   - 1.0
     */
    float translation_x = 2.0f * center[0] / framebuffer_size[0] - 1.0f;
    float translation_y = 2.0f * center[1] / framebuffer_size[1] - 1.0f;
    float scale_x = 2.0 * size / framebuffer_size[0];
    float scale_y = 2.0 * size / framebuffer_size[1];
    Mtx mv;
    guMtxIdentity(mv);
    mv[0][0] = scale_x;
    mv[1][1] = scale_y;
    mv[2][2] = 0.0f;
    mv[0][3] = translation_x;
    mv[1][3] = translation_y;
    ogx_shader_set_modelview_gx(mv);

    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    uint8_t tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    uint8_t tex_mtx = GX_TEXMTX0 + ogx_gpu_resources->texmtx_first++ * 3;

    /* The fragment shader is very complicated, let's draw it over a texture */
    GXTexObj texture;
    uint8_t format = GX_TF_RGBA8;
    int texside = size * 2;
    uint32_t texture_size =
        GX_GetTexBufferSize(texside, texside, format, GX_FALSE, 0);
    /* TODO: free this memory */
    data->texels = new(std::align_val_t{ 32 }) uint8_t[texture_size];
    for (int x = 0; x < texside; x++) {
        for (int y = 0; y < texside; y++) {
            GXColor texel = fragment_sphere_template(x, y, size);
            set_pixel_to_texture_ARGB(data->texels, texel, x, y, texside);
        }
    }
    DCFlushRange(data->texels, texture_size);
    GX_InvalidateTexAll();
    GX_InitTexObj(&texture, data->texels, texside, texside, format,
                  GX_CLAMP, GX_CLAMP, 0);
    GX_InitTexObjLOD(&texture, GX_NEAR, GX_NEAR, 0.0F, 0.0F, 0.0F,
                     GX_FALSE, GX_FALSE, GX_ANISO_1);
    GX_LoadTexObj(&texture, tex_map);

    /* Position coordinates are for a square [-1,-1]x[1,1], therefore we use a
     * matrix to transform them to [0,0]x[1,1] */
    Mtx texm;
    guMtxIdentity(texm);
    texm[0][0] = texm[1][1] = 0.5f; /* scale */
    texm[0][3] = texm[1][3] = 0.5f; /* translation */
    GX_LoadTexMtxImm(texm, tex_mtx, GX_MTX2x4);
    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_POS, tex_mtx);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLORNULL);
    GX_SetNumChans(0);
    GX_SetTevOp(stage, GX_REPLACE);
}

struct ShaderEffect {
    GLint subtex_loc;
    GLint brightness_loc;
    GLint frame_a_loc;
    GLint frame_b_loc;
};

static void setup_draw_effect(GLuint program,
                              const OgxDrawData *draw_data,
                              void *user_data)
{
    auto *data = static_cast<ShaderEffect*>(user_data);

    float subtex[2];
    glGetUniformfv(program, data->subtex_loc, subtex);

    float brightness;
    glGetUniformfv(program, data->brightness_loc, &brightness);

    GLint texture_unit_a, texture_unit_b;
    glGetUniformiv(program, data->frame_a_loc, &texture_unit_a);
    glGetUniformiv(program, data->frame_b_loc, &texture_unit_b);
    uint8_t tex_map_a = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_map_b = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    GXTexObj *texture_a = ogx_shader_get_texobj(texture_unit_a);
    GX_LoadTexObj(texture_a, tex_map_a);
    GXTexObj *texture_b = ogx_shader_get_texobj(texture_unit_b);
    GX_LoadTexObj(texture_b, tex_map_b);

    /* For x and y, we have:
     * x = 2.0 * vtx.x - 1.0
     */
    float translation_x = -1.0f;
    float translation_y = -1.0f;
    float scale_x = 2.0f;
    float scale_y = 2.0f;
    Mtx mv;
    guMtxIdentity(mv);
    mv[0][0] = scale_x;
    mv[1][1] = scale_y;
    mv[0][3] = translation_x;
    mv[1][3] = translation_y;
    ogx_shader_set_modelview_gx(mv);

    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    uint8_t tex_mtx = GX_TEXMTX0 + ogx_gpu_resources->texmtx_first++ * 3;

    /* The vertex shader obtains the texture coordinates by multiplying the
     * position by subtex; let's do the same. */
    Mtx texm;
    guMtxIdentity(texm);
    texm[0][0] = subtex[0];
    texm[1][1] = subtex[1];
    GX_LoadTexMtxImm(texm, tex_mtx, GX_MTX2x4);
    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_POS, tex_mtx);
    GX_SetNumChans(0);

    /* Blend the two textures according to the brightness; in the first stage,
     * load frame_a */
    GX_SetTevOrder(stage, tex_coord, tex_map_a, GX_COLORNULL);
    GX_SetTevColorIn(stage, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
    GX_SetTevAlphaIn(stage, GX_CA_TEXA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);

    /* In the second stage, load frame_b and blend it with frame_a */
    stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    uint8_t c = brightness * 255;
    GXColor color = { c, c, c, c };
    GX_SetTevKAlphaSel(stage, GX_TEV_KASEL_K0_A);
    GX_SetTevKColorSel(stage, GX_TEV_KCSEL_K0);
    GX_SetTevKColor(GX_KCOLOR0, color);
    GX_SetTevOrder(stage, tex_coord, tex_map_b, GX_COLORNULL);
    GX_SetTevColorIn(stage, GX_CC_CPREV, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
    GX_SetTevAlphaIn(stage, GX_CA_APREV, GX_CA_TEXA, GX_CA_KONST, GX_CA_ZERO);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
}

struct ShaderLine {
    GLint projection_loc;
};

static void setup_draw_line(GLuint program, const OgxDrawData *draw_data,
                            void *user_data)
{
    auto *data = static_cast<ShaderLine*>(user_data);
    setup_projection(program, data->projection_loc);
    GX_SetCurrentMtx(GX_IDENTITY);

    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;

    GX_SetNumChans(1);
    GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX,
                   0, GX_DF_NONE, GX_AF_NONE);
    GX_SetTevOrder(stage, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GX_SetTevOp(stage, GX_PASSCLR);
}

struct ShaderFont {
    GLint projection_loc;
    GLint transform_loc;
    GLint color_loc;
    GLint sampler_loc;
    int stage;
    int tex_coord;
};

static void setup_draw_font(GLuint program, const OgxDrawData *draw_data,
                            void *user_data)
{
    auto *data = static_cast<ShaderFont*>(user_data);

    float transform[4];
    glGetUniformfv(program, data->transform_loc, transform);

    GLint texture_unit;
    glGetUniformiv(program, data->sampler_loc, &texture_unit);
    uint8_t tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_map_indirect = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    GXTexObj *texture = ogx_shader_get_texobj(texture_unit);
    /* Load the font texture as an indirect texture map: we'll use its alpha
     * value (0-255) to lookup the actual color (which in this case is also an
     * I8 texture used for alpha only) from a 16x16 "color_remap" texture,
     * whose pixels are computed according to the function described in the
     * fragment shader. Using these coordinates:
     *  s: alpha % 16  (the "%" is implemented by tiling with GX_REPEAT)
     *  t: alpha / 16
     */
    GX_LoadTexObj(texture, tex_map_indirect);

    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    uint8_t stage_indirect = GX_INDTEXSTAGE0;

    /* Prepare the color_remap texture texels */
    uint8_t map_bytes[256];
    float e = 0.05 / transform[2];
    for (int i = 0; i < 256; i++) {
        map_bytes[i] = smoothstep(0.5 - e, 0.5 + e, i / 255.0f) * 255;
    }
    prepare_I8_texture(s_func_1D_texels, map_bytes, 16, 16);
    DCFlushRange(s_func_1D_texels, sizeof(s_func_1D_texels));
    GXTexObj color_remap;
    GX_InitTexObj(&color_remap, s_func_1D_texels, 16, 16, GX_TF_I8, GX_REPEAT, GX_REPEAT, 0);
    GX_InitTexObjLOD(&color_remap, GX_NEAR, GX_NEAR, 0.0F, 0.0F, 0.0F,
                     GX_FALSE, GX_FALSE, GX_ANISO_1);

    GX_LoadTexObj(&color_remap, tex_map);
    GX_InvalidateTexAll();
    GX_SetIndTexOrder(stage_indirect, tex_coord, tex_map_indirect);
    GX_SetIndTexCoordScale(stage_indirect, GX_ITS_1, GX_ITS_1);

    /* We need to do this dance with the exponent because the matrix does not
     * support the 1.0 value. See the documentation of GX_SetIndTexMatrix() for
     * an explanation. */
    int exponent = 3;
    int divisor = 1 << (exponent);
    float ind_mtx[2][3] = {{1.0f/divisor, 0, 0}, {0, 1.0f/(16*divisor), 0}};
	GX_SetIndTexMatrix(GX_ITM_0, ind_mtx, exponent);
    GX_SetTevIndirect(stage, stage_indirect, GX_ITF_8, GX_ITB_NONE,
                      GX_ITM_0, GX_ITW_0, GX_ITW_0, GX_FALSE, GX_FALSE, GX_ITBA_OFF);
    GX_SetNumIndStages(1);

    /* We need to set the manual scale, because otherwise GX will automatically
     * scale the coordinates according to the size of "color_remap" texture */
    GX_SetTexCoordScaleManually(tex_coord, GX_TRUE,
                                GX_GetTexObjWidth(texture),
                                GX_GetTexObjHeight(texture));

    Mtx mv;
    guMtxIdentity(mv);
    mv[0][0] = transform[2];
    mv[1][1] = transform[2];
    mv[2][2] = 0.0f;
    mv[0][3] = transform[0];
    mv[1][3] = transform[1];
    ogx_shader_set_modelview_gx(mv);

    float colorf[4];
    glGetUniformfv(program, data->color_loc, colorf);
    GXColor color = gxcol_new_fv(colorf);
    GX_SetNumChans(1);
    GX_SetChanMatColor(GX_COLOR0A0, color);
    GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG,
                   0, GX_DF_NONE, GX_AF_NONE);

    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLOR0A0);
    GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
    GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);

    /* For draw_done_font */
    data->stage = stage;
    data->tex_coord = tex_coord;
}

static void setup_matrices_font(GLuint program, void *user_data)
{
    auto *data = static_cast<ShaderFont*>(user_data);
    setup_projection(program, data->projection_loc);
}

static void draw_done_font(GLuint program, void *user_data)
{
    auto *data = static_cast<ShaderFont*>(user_data);
    GX_SetNumIndStages(0);
    GX_SetTevDirect(data->stage);
    GX_SetTexCoordScaleManually(data->tex_coord, GX_FALSE, 0, 0);
}

struct ShaderPageIndicator {
    GLint projection_loc;
    GLint center_loc;
    GLint size_loc;
    GLint filled_loc;
    int texside = 0;
    uint8_t *texels_filled = nullptr;
    uint8_t *texels_unfilled = nullptr;
};

static inline uint8_t fragment_page_indicator(int x, int y, int size,
                                              GLint filled)
{
    /* We are translating pageindicator.fsh into C code */
    int center = size / 2;
    float dist = length(x - center, y - center);
    float border = 0.8f;

    float outer_radius = size / 2.0f;
    float inner_radius = size / 2.0f - border * 2.0f;

    // Antialiased circle shape
    float intensity = smoothstep(outer_radius, outer_radius-border, dist);

    if (!filled) {
        // Remove the circle (inner radius) to leave only an outline
        intensity -= smoothstep(inner_radius, inner_radius-border, dist);
    }

    return intensity * 255;
}

static void setup_draw_page_indicator(GLuint program, const OgxDrawData *draw_data,
                                      void *user_data)
{
    auto *data = static_cast<ShaderPageIndicator*>(user_data);
    setup_projection(program, data->projection_loc);

    float center[2];
    glGetUniformfv(program, data->center_loc, center);

    float size;
    glGetUniformfv(program, data->size_loc, &size);

    GLint filled;
    glGetUniformiv(program, data->filled_loc, &filled);

    /* For x and y, we have:
     * x = center.x + vtx.x * size
     */
    float translation_x = center[0];
    float translation_y = center[1];
    float scale_x = size;
    float scale_y = size;
    Mtx mv;
    guMtxIdentity(mv);
    mv[0][0] = scale_x;
    mv[1][1] = scale_y;
    mv[2][2] = 0.0f;
    mv[0][3] = translation_x;
    mv[1][3] = translation_y;
    ogx_shader_set_modelview_gx(mv);

    /* The fragment shader is very complicated, let's draw it over a texture */
    uint8_t tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    uint8_t tex_mtx = GX_TEXMTX0 + ogx_gpu_resources->texmtx_first++ * 3;
    GXTexObj texture;
    uint8_t format = GX_TF_I8;
    int texside = size;
    uint32_t texture_size =
        GX_GetTexBufferSize(texside, texside, format, GX_FALSE, 0);
    if (data->texside != texside) {
        if (data->texels_filled) {
            fprintf(stderr, "WARNING: requested a different size (%d)"
                    " for the page indicator\n", texside);
            delete[] data->texels_filled;
            delete[] data->texels_unfilled;
        }
        data->texels_filled = new(std::align_val_t{ 32 }) uint8_t[texture_size];
        data->texels_unfilled = new(std::align_val_t{ 32 }) uint8_t[texture_size];
        data->texside = texside;
    }
    uint8_t *texels = filled ? data->texels_filled : data->texels_unfilled;
    for (int x = 0; x < texside; x++) {
        for (int y = 0; y < texside; y++) {
            uint8_t texel = fragment_page_indicator(x, y, texside, filled);
            set_pixel_to_texture_I8(texels, texel, x, y, texside);
        }
    }
    DCFlushRange(texels, texture_size);
    GX_InvalidateTexAll();
    GX_InitTexObj(&texture, texels, texside, texside, format,
                  GX_CLAMP, GX_CLAMP, 0);
    GX_InitTexObjLOD(&texture, GX_NEAR, GX_NEAR, 0.0F, 0.0F, 0.0F,
                     GX_FALSE, GX_FALSE, GX_ANISO_1);
    GX_LoadTexObj(&texture, tex_map);

    /* Position coordinates are for a square [-0.5,-0.5]x[0.5,0.5], therefore
     * we use a matrix to transform them to [0,0]x[1,1] */
    Mtx texm;
    guMtxIdentity(texm);
    texm[0][3] = texm[1][3] = 0.5f; /* translation */
    GX_LoadTexMtxImm(texm, tex_mtx, GX_MTX2x4);
    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_POS, tex_mtx);
    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    GX_SetNumChans(0);

    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLORNULL);
    GX_SetTevOp(stage, GX_REPLACE);
}

struct ShaderEffectOverlay {
    GLint subtex_loc;
    GLint sampler_loc;
};

static void setup_draw_effect_overlay(GLuint program,
                                      const OgxDrawData *draw_data,
                                      void *user_data)
{
    auto *data = static_cast<ShaderEffectOverlay*>(user_data);

    float subtex[2];
    glGetUniformfv(program, data->subtex_loc, subtex);

    GLint texture_unit;
    glGetUniformiv(program, data->sampler_loc, &texture_unit);
    uint8_t tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    GXTexObj *texture = ogx_shader_get_texobj(texture_unit);
    GX_LoadTexObj(texture, tex_map);

    /* For x and y, we have:
     * x = 2.0 * vtx.x - 1.0
     */
    Mtx mv;
    guMtxIdentity(mv);
    mv[0][0] = 2.0f;
    mv[1][1] = 2.0f;
    mv[0][3] = -1.0f;
    mv[1][3] = -1.0f;
    ogx_shader_set_modelview_gx(mv);

    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    uint8_t tex_mtx = GX_TEXMTX0 + ogx_gpu_resources->texmtx_first++ * 3;

    /* The vertex shader obtains the texture coordinates by multiplying the
     * position by subtex; let's do the same. */
    Mtx texm;
    guMtxIdentity(texm);
    texm[0][0] = subtex[0];
    texm[1][1] = subtex[1];
    GX_LoadTexMtxImm(texm, tex_mtx, GX_MTX2x4);
    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_POS, tex_mtx);
    GX_SetNumChans(0);

    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLORNULL);
    GX_SetTevOp(stage, GX_REPLACE);
}

struct ShaderEffectOverlayTemplate {
    GLint portrait_loc;
    uint8_t *texels = nullptr;
};

static void setup_draw_effect_overlay_template(GLuint program,
                                               const OgxDrawData *draw_data,
                                               void *user_data)
{
    auto *data = static_cast<ShaderEffectOverlayTemplate*>(user_data);

    GLint portrait;
    glGetUniformiv(program, data->portrait_loc, &portrait);

    /* For x and y, we have:
     * x = 2.0 * vtx.x - 1.0
     */
    Mtx mv;
    guMtxIdentity(mv);
    mv[0][0] = 2.0f;
    mv[1][1] = 2.0f;
    mv[0][3] = -1.0f;
    mv[1][3] = -1.0f;
    ogx_shader_set_modelview_gx(mv);

    /* The fragment shader draws every every odd line with a different alpha.
     * Let's prepare a 2x1 texture. */
    uint8_t tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    uint8_t tex_mtx = GX_TEXMTX0 + ogx_gpu_resources->texmtx_first++ * 3;
    uint8_t format = GX_TF_I8;
    int tex_width = 2, tex_height = 1;
    if (!data->texels) {
        data->texels = new(std::align_val_t{ 32 }) uint8_t[32];
        set_pixel_to_texture_I8(data->texels, 0.76 * 255, 0, 0, tex_width);
        set_pixel_to_texture_I8(data->texels, 0.65 * 255, 1, 0, tex_height);
        DCFlushRange(data->texels, 32);
        GX_InvalidateTexAll();
    }
    GXTexObj texture;
    GX_InitTexObj(&texture, data->texels, tex_width, tex_height, format,
                  GX_REPEAT, GX_REPEAT, 0);
    GX_InitTexObjLOD(&texture, GX_NEAR, GX_NEAR, 0.0F, 0.0F, 0.0F,
                     GX_FALSE, GX_FALSE, GX_ANISO_1);
    GX_LoadTexObj(&texture, tex_map);

    /* The vertex shader obtains the texture coordinates by multiplying the
     * position by subtex; let's do the same. */
    Mtx texm;
    guMtxIdentity(texm);
    texm[0][0] = 640 / 2;
    texm[1][1] = 480 / 2;
    if (!portrait) {
        /* Swap S and T coordinates  */
        texm[0][1] = texm[1][1];
        texm[1][0] = texm[0][0];
        texm[0][0] = 0.0f;
        texm[1][1] = 0.0f;
    }
    GX_LoadTexMtxImm(texm, tex_mtx, GX_MTX2x4);
    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_POS, tex_mtx);
    GX_SetNumChans(0);

    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;

    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLORNULL);
    GX_SetTevOp(stage, GX_REPLACE);
}

struct ShaderBackground {
    GLint screensize_loc;
    GLint color_loc;
    GLint sampler_loc;
    uint8_t stage;
};

static void setup_draw_background(GLuint program,
                                  const OgxDrawData *draw_data,
                                  void *user_data)
{
    auto *data = static_cast<ShaderBackground*>(user_data);

    float screensize[2];
    glGetUniformfv(program, data->screensize_loc, screensize);

    GLint texture_unit;
    glGetUniformiv(program, data->sampler_loc, &texture_unit);
    uint8_t tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    GXTexObj *texture = ogx_shader_get_texobj(texture_unit);
    GX_LoadTexObj(texture, tex_map);

    GX_SetCurrentMtx(GX_IDENTITY);

    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    uint8_t tex_mtx = GX_TEXMTX0 + ogx_gpu_resources->texmtx_first++ * 3;

    /* The vertex shader obtains the texture coordinates like this:
     * tex = ((pos + vec2(1.0)) / 2.0) * screensize;
     * and inverts the y coordinate (1 - y). */
    float translation_x = screensize[0] / 2.0f;
    float translation_y = screensize[1] / 2.0f;
    float scale_x = screensize[0] / 2.0f;
    float scale_y = -screensize[1] / 2.0f;
    Mtx texm;
    guMtxIdentity(texm);
    texm[0][0] = scale_x;
    texm[1][1] = scale_y;
    texm[0][3] = translation_x;
    texm[1][3] = translation_y;
    GX_LoadTexMtxImm(texm, tex_mtx, GX_MTX2x4);
    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_POS, tex_mtx);

    float colorf[4];
    glGetUniformfv(program, data->color_loc, colorf);
    GXColor color = gxcol_new_fv(colorf);
    GX_SetNumChans(1);
    GX_SetChanMatColor(GX_COLOR0A0, color);
    GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG,
                   0, GX_DF_NONE, GX_AF_NONE);

    GX_SetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP1);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLOR0A0);
    GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_RASC, GX_CC_ZERO);
    GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    data->stage = stage;
}

static void draw_done_background(GLuint program, void *user_data)
{
    auto *data = static_cast<ShaderBackground*>(user_data);
    GX_SetTevSwapMode(data->stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
}

struct ShaderBackgroundTemplate {
    GLint screensize_loc;
    GLint randomsize_loc;
    GLint sampler_loc;
    GXTexObj vignette;
    uint8_t *vignette_texels = nullptr;
    GXTexObj highlight;
    uint8_t *highlight_texels = nullptr;
    uint8_t stage;
};

static inline uint8_t fragment_background_vignette(float pos_x, float pos_y)
{
    /* We are translating the vignette part from backgroundtemplate.fsh into C
     * code */

    // Add vignette effect
    float hvignette = smoothstep(2.5, 0.5, fabs(pos_x));
    float vvignette = smoothstep(2.4, 0.5, fabs(pos_y));

    return hvignette * vvignette * 255;
}

static inline uint8_t fragment_background_highlight(float pos_x, float pos_y)
{
    /* We are translating the highlight part from backgroundtemplate.fsh into C
     * code */

    // Add highlight effect
    float highlight = smoothstep(1.0, 0.0, length(pos_x + 0.2, pos_y + 0.3));

    return highlight * 0.2 * 255;
}

static void setup_draw_background_template(GLuint program,
                                           const OgxDrawData *draw_data,
                                           void *user_data)
{
    auto *data = static_cast<ShaderBackgroundTemplate*>(user_data);

    if (!data->vignette_texels) {
        /* Build a texture for the vignette and for the highlight effects;
         * since we are going to scale them, they don't necessarily need to
         * match the screen size */
        int texside = 256;
        uint8_t format = GX_TF_I8;
        uint32_t texture_size =
            GX_GetTexBufferSize(texside, texside, format, GX_FALSE, 0);
        data->vignette_texels = new(std::align_val_t{ 32 }) uint8_t[texture_size];
        data->highlight_texels = new(std::align_val_t{ 32 }) uint8_t[texture_size];
        for (int x = 0; x < texside; x++) {
            for (int y = 0; y < texside; y++) {
                float pos_x = float(x * 2) / texside - 1.0f;
                float pos_y = float(y * 2) / texside - 1.0f;

                uint8_t color = fragment_background_vignette(pos_x, pos_y);
                set_pixel_to_texture_I8(data->vignette_texels, color, x, y, texside);

                color = fragment_background_highlight(pos_x, pos_y);
                set_pixel_to_texture_I8(data->highlight_texels, color, x, y, texside);
            }
        }
        DCFlushRange(data->vignette_texels, texture_size);
        GX_InitTexObj(&data->vignette, data->vignette_texels, texside, texside, format,
                      GX_CLAMP, GX_CLAMP, 0);
        GX_InitTexObjLOD(&data->vignette, GX_NEAR, GX_NEAR, 0.0F, 0.0F, 0.0F,
                         GX_FALSE, GX_FALSE, GX_ANISO_2);

        DCFlushRange(data->highlight_texels, texture_size);
        GX_InitTexObj(&data->highlight, data->highlight_texels, texside, texside, format,
                      GX_CLAMP, GX_CLAMP, 0);
        GX_InitTexObjLOD(&data->highlight, GX_NEAR, GX_NEAR, 0.0F, 0.0F, 0.0F,
                         GX_FALSE, GX_FALSE, GX_ANISO_2);

        GX_InvalidateTexAll();
    }

    float screensize[2];
    glGetUniformfv(program, data->screensize_loc, screensize);

    float randomsize[2];
    glGetUniformfv(program, data->randomsize_loc, randomsize);

    GLint texture_unit;
    glGetUniformiv(program, data->sampler_loc, &texture_unit);
    uint8_t tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    GXTexObj *texture = ogx_shader_get_texobj(texture_unit);
    GXTexObj texture_wrapped;
    memcpy(&texture_wrapped, texture, sizeof(GXTexObj));
    GX_InitTexObjWrapMode(&texture_wrapped, GX_REPEAT, GX_REPEAT);
    GX_LoadTexObj(&texture_wrapped, tex_map);

    GX_SetCurrentMtx(GX_IDENTITY);

    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    uint8_t tex_mtx = GX_TEXMTX0 + ogx_gpu_resources->texmtx_first++ * 3;

    /* The vertex shader obtains the texture coordinates like this:
     * tex = ((pos + vec2(1.0)) / 2.0) * screensize / randomsize;
     */
    float tmp_x = screensize[0] / (randomsize[0] * 2.0f);
    float tmp_y = screensize[1] / (randomsize[1] * 2.0f);
    float translation_x = tmp_x;
    float translation_y = tmp_y;
    float scale_x = tmp_x;
    float scale_y = tmp_y;
    Mtx texm;
    guMtxIdentity(texm);
    texm[0][0] = scale_x;
    texm[1][1] = scale_y;
    texm[0][3] = translation_x;
    texm[1][3] = translation_y;
    GX_LoadTexMtxImm(texm, tex_mtx, GX_MTX2x4);
    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_POS, tex_mtx);

    GX_SetNumChans(0);

    GX_SetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP1);
    uint8_t c = 0.06 * 255;
    GXColor color = { c, c, c, 255 };
    GX_SetTevKColor(GX_KCOLOR0, color);
    GX_SetTevKColorSel(stage, GX_TEV_KCSEL_K0);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLORNULL);
    GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_ZERO);
    GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    /* Set GX_TB_ADDHALF, though the shader adds 0.6 really */
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ADDHALF, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    data->stage = stage;

    /* Second stage: apply vignette */
    stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    tex_mtx = GX_TEXMTX0 + ogx_gpu_resources->texmtx_first++ * 3;
    GX_LoadTexObj(&data->vignette, tex_map);
    texm[0][0] = texm[1][1] = texm[0][3] = texm[1][3] = 0.5f;
    GX_LoadTexMtxImm(texm, tex_mtx, GX_MTX2x4);
    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_POS, tex_mtx);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLORNULL);
    GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_CPREV, GX_CC_ZERO);
    GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);

    /* Second stage: apply highlight (the 0.2 factor has been premultiplied
     * into the highlight texture) */
    stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    GX_LoadTexObj(&data->highlight, tex_map);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLORNULL);
    GX_SetTevColorIn(stage, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
}

static void draw_done_background_template(GLuint program, void *user_data)
{
    auto *data = static_cast<ShaderBackground*>(user_data);
    GX_SetTevSwapMode(data->stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
}

struct ShaderIcons {
    GLint projection_loc;
    GLint color_loc;
    GLint sampler_loc;
    uint8_t stage;
};

static void setup_draw_icons(GLuint program, const OgxDrawData *draw_data,
                             void *user_data)
{
    auto *data = static_cast<ShaderIcons*>(user_data);
    setup_projection(program, data->projection_loc);

    GX_SetCurrentMtx(GX_IDENTITY);

    GLint texture_unit;
    glGetUniformiv(program, data->sampler_loc, &texture_unit);
    uint8_t tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    GXTexObj *texture = ogx_shader_get_texobj(texture_unit);
    GX_LoadTexObj(texture, tex_map);

    float colorf[4];
    glGetUniformfv(program, data->color_loc, colorf);
    GXColor color = gxcol_new_fv(colorf);
    GX_SetNumChans(1);
    GX_SetChanMatColor(GX_COLOR0A0, color);
    GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG,
                   0, GX_DF_CLAMP, GX_AF_NONE);


    /* The fragment shader does:
     * gl_FragColor.rgba = color * (alpha + 2.0 * glow)
     *    + vec4(1.0) * 0.03 * highlight * (alpha + glow);
     * which we can rewrite as:
     * gl_FragColor.rgba =
     *    alpha * (color + 0.03 * highlight) +
     *    glow * (2.0 * color + 0.03 * highlight)
     */
    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    data->stage = stage;
    /* In the first stage, we compute "color + 0.03 * highlight" */
    uint8_t k = 0.03f * 255;
    GXColor konst = { k, k, k, k };
    GX_SetTevKAlphaSel(stage, GX_TEV_KASEL_K0_A);
    GX_SetTevKColorSel(stage, GX_TEV_KCSEL_K0);
    GX_SetTevKColor(GX_KCOLOR0, konst);
    GX_SetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP3);
    GX_SetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE);
    // In data: c: Texture Color b: raster value, Operation: b*c
    GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_RASC);
    GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_RASA);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVREG0);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVREG0);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLOR0A0);
    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

    /* Second stage:  compute "2.0 * color + 0.03 * highlight" by adding
     * "color" to the value from the previous stage */
    stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    GX_SetTevColorIn(stage, GX_CC_RASC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
    GX_SetTevAlphaIn(stage, GX_CA_RASA, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevOrder(stage, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);

    /* Third stage: compute "glow * (2.0 * color + 0.03 * highlight)" */
    stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    GX_SetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP2);
    GX_SetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN);
    GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_CPREV, GX_CC_ZERO);
    GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_TEXA, GX_CA_APREV, GX_CA_ZERO);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLORNULL);

    /* Fourth stage: compute the final color: to the color from the previous
     * stage we add the "alpha * (color + 0.03 * highlight)" part. */
    stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    GX_SetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP1);
    GX_SetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_RED);
    GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_CPREV);
    GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_APREV);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLORNULL);
}

static void draw_done_icons(GLuint program, void *user_data)
{
    auto *data = static_cast<ShaderBackground*>(user_data);
    for (int i = 0; i < 4; i++) {
        GX_SetTevSwapMode(data->stage + i, GX_TEV_SWAP0, GX_TEV_SWAP0);
    }
    /* Restore the swap tables to how GX_Init() sets it */
    GX_SetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    GX_SetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    GX_SetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
}

struct ShaderDecal {
    GLint projection_loc;
    GLint opacity_loc;
    GLint sampler_loc;
};

static void setup_draw_decal(GLuint program, const OgxDrawData *draw_data,
                             void *user_data)
{
    auto *data = static_cast<ShaderDecal*>(user_data);
    setup_projection(program, data->projection_loc);
    GX_SetCurrentMtx(GX_IDENTITY);

    GLint texture_unit;
    glGetUniformiv(program, data->sampler_loc, &texture_unit);
    uint8_t tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    GXTexObj *texture = ogx_shader_get_texobj(texture_unit);
    GX_LoadTexObj(texture, tex_map);

    float opacity;
    glGetUniformfv(program, data->opacity_loc, &opacity);
    GX_SetNumChans(0);

    /* The fragment shader does:
     * gl_FragColor = vec4(1.0, 1.0, 1.0, opacity*texture2D(sampler, tex).a);
     * So, it's a white pixel with alpha taken from the texture and multiplied
     * by the value of "opacity".
     */
    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    uint8_t k = opacity * 255;
    GXColor konst = { 0, 0, 0, k };
    GX_SetTevKAlphaSel(stage, GX_TEV_KASEL_K0_A);
    GX_SetTevKColor(GX_KCOLOR0, konst);
    GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
    GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLORNULL);
    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
}

struct ShaderShadow {
    GLint projection_loc;
};

static void setup_draw_shadow(GLuint program, const OgxDrawData *draw_data,
                              void *user_data)
{
    auto *data = static_cast<ShaderShadow*>(user_data);
    setup_projection(program, data->projection_loc);
    GX_SetCurrentMtx(GX_IDENTITY);

    GX_SetNumChans(1);
    GX_SetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX,
                   0, GX_DF_CLAMP, GX_AF_NONE);

    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    /* Color is black, alpha comes from the "opacity" attribute, which we pass
     * as GX_VA_CLR0, and it's multiplied by 0.1 */
    uint8_t k = 0.1 * 255;
    GXColor konst = { 0, 0, 0, k };
    GX_SetTevKAlphaSel(stage, GX_TEV_KASEL_K0_A);
    GX_SetTevKColor(GX_KCOLOR0, konst);
    GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
    GX_SetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_RASA, GX_CA_KONST, GX_CA_ZERO);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevOrder(stage, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
}

struct ShaderLevelPreview {
    GLint projection_loc;
    GLint sampler_loc;
};

static void setup_draw_level_preview(GLuint program,
                                     const OgxDrawData *draw_data,
                                     void *user_data)
{
    auto *data = static_cast<ShaderLevelPreview*>(user_data);
    setup_projection(program, data->projection_loc);
    GX_SetCurrentMtx(GX_IDENTITY);

    GLint texture_unit;
    glGetUniformiv(program, data->sampler_loc, &texture_unit);
    uint8_t tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    GXTexObj *texture = ogx_shader_get_texobj(texture_unit);
    GX_LoadTexObj(texture, tex_map);

    GX_SetNumChans(0);

    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    uint8_t k = 0.4 * 255;
    GXColor konst = { 0, 0, 0, k };
    GX_SetTevKAlphaSel(stage, GX_TEV_KASEL_K0_A);
    GX_SetTevKColor(GX_KCOLOR0, konst);
    GX_SetTevColorIn(stage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
    GX_SetTevAlphaIn(stage, GX_CA_KONST, GX_CA_TEXA, GX_CA_TEXA, GX_CA_ZERO);
    GX_SetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE,
                     GX_TEVPREV);
    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLORNULL);
    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
}

struct ShaderCachedScreen {
    GLint offset_loc;
    GLint subsize_loc;
    GLint sampler_loc;
};

static void setup_draw_cached_screen(GLuint program,
                                     const OgxDrawData *draw_data,
                                     void *user_data)
{
    auto *data = static_cast<ShaderCachedScreen*>(user_data);

    float offset[2];
    glGetUniformfv(program, data->offset_loc, offset);

    float subsize[2];
    glGetUniformfv(program, data->subsize_loc, subsize);

    GLint texture_unit;
    glGetUniformiv(program, data->sampler_loc, &texture_unit);
    uint8_t tex_map = GX_TEXMAP0 + ogx_gpu_resources->texmap_first++;
    uint8_t tex_coord = GX_TEXCOORD0 + ogx_gpu_resources->texcoord_first++;
    GXTexObj *texture = ogx_shader_get_texobj(texture_unit);
    GX_LoadTexObj(texture, tex_map);

    /* The vertex shader transforms the vertex coordinates like this:
     *   pos.x = vtx.x + offset.x * 2.0
     *   (same for y)
     */
    Mtx mv;
    guMtxIdentity(mv);
    mv[0][3] = offset[0] * 2.0f;
    mv[1][3] = offset[1] * 2.0f;
    mv[2][2] = 0.0f; /* z is set to 0 */
    ogx_shader_set_modelview_gx(mv);

    uint8_t stage = GX_TEVSTAGE0 + ogx_gpu_resources->tevstage_first++;
    uint8_t tex_mtx = GX_TEXMTX0 + ogx_gpu_resources->texmtx_first++ * 3;

    /* The vertex shader obtains the texture coordinates like this:
     *   tex.x = 0.5 + 0.5 * vtx.x * subsize.x
     * (same for y). */
    float translation_x = 0.5 * subsize[0];
    float translation_y = 0.5 * subsize[1];
    float scale_x = subsize[0] * 0.5f;
    float scale_y = subsize[1] * 0.5f;
    Mtx texm;
    guMtxIdentity(texm);
    texm[0][0] = scale_x;
    texm[1][1] = scale_y;
    texm[0][3] = translation_x;
    texm[1][3] = translation_y;
    GX_LoadTexMtxImm(texm, tex_mtx, GX_MTX2x4);
    GX_SetTexCoordGen(tex_coord, GX_TG_MTX2x4, GX_TG_POS, tex_mtx);

    GX_SetNumChans(0);

    GX_SetTevOrder(stage, tex_coord, tex_map, GX_COLORNULL);
    GX_SetTevOp(stage, GX_REPLACE);
}

static bool shader_compile(GLuint shader)
{
    uint32_t source_hash = ogx_shader_get_source_hash(shader);

    fprintf(stderr, "%s:%d shader %x hash %08x\n", __FILE__, __LINE__, shader, source_hash);
    if (source_hash == s_hash_loading) {
        /* loading.vsh */
        ogx_shader_add_uniforms(shader, 2,
                                "projection", GL_FLOAT_MAT4,
                                "color", GL_FLOAT_VEC4);
        ogx_shader_add_attributes(shader, 1,
                                  "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS);
    } else if (source_hash == s_hash_spheredefault) {
        /* spheredefault.vsh */
        ogx_shader_add_uniforms(shader, 2,
            "projection", GL_FLOAT_MAT4,
            "sampler", GL_SAMPLER_2D);
        ogx_shader_add_attributes(shader, 4,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS,
            "texcoord", GL_FLOAT_VEC2, GX_VA_TEX0,
            "color", GL_FLOAT_VEC3, GX_VA_CLR0,
            "desired", GL_FLOAT_VEC3, GX_VA_CLR0);
    } else if (source_hash == s_hash_spheretemplate) {
        /* spheretemplate.vsh */
        ogx_shader_add_uniforms(shader, 3,
            "framebuffer_size", GL_FLOAT_VEC2,
            "center", GL_FLOAT_VEC2,
            "size", GL_FLOAT);
        ogx_shader_add_attributes(shader, 1,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS);
    } else if (source_hash == s_hash_effect) {
        /* effect.vsh */
        ogx_shader_add_uniforms(shader, 6,
            "subtex", GL_FLOAT_VEC2,
            "brightness", GL_FLOAT,
            "portrait", GL_BOOL,
            "time", GL_FLOAT,
            "frame_a", GL_SAMPLER_2D,
            "frame_b", GL_SAMPLER_2D);
        ogx_shader_add_attributes(shader, 1,
            "coord", GL_FLOAT_VEC4, GX_VA_POS);
    } else if (source_hash == s_hash_line) {
        /* line.vsh */
        ogx_shader_add_uniforms(shader, 1,
            "projection", GL_FLOAT_MAT4);
        ogx_shader_add_attributes(shader, 2,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS,
            "color", GL_FLOAT_VEC4, GX_VA_CLR0);
    } else if (source_hash == s_hash_font) {
        /* font.vsh */
        ogx_shader_add_uniforms(shader, 4,
            "projection", GL_FLOAT_MAT4,
            "transform", GL_FLOAT_VEC4,
            "color", GL_FLOAT_VEC4,
            "sampler", GL_SAMPLER_2D);
        ogx_shader_add_attributes(shader, 2,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS,
            "texcoord", GL_FLOAT_VEC2, GX_VA_TEX0);
    } else if (source_hash == s_hash_pageindicator) {
        /* pageindicator.vsh */
        ogx_shader_add_uniforms(shader, 4,
            "projection", GL_FLOAT_MAT4,
            "center", GL_FLOAT_VEC2,
            "size", GL_FLOAT,
            "filled", GL_BOOL);
        ogx_shader_add_attributes(shader, 1,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS);
    } else if (source_hash == s_hash_effect_overlay) {
        /* effectoverlay.vsh */
        ogx_shader_add_uniforms(shader, 2,
            "subtex", GL_FLOAT_VEC2,
            "sampler", GL_SAMPLER_2D);
        ogx_shader_add_attributes(shader, 1,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS);
    } else if (source_hash == s_hash_effect_overlay_template) {
        /* effectoverlaytemplate.vsh */
        ogx_shader_add_uniforms(shader, 1,
            "portrait", GL_BOOL);
        ogx_shader_add_attributes(shader, 1,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS);
    } else if (source_hash == s_hash_background) {
        /* background.vsh */
        ogx_shader_add_uniforms(shader, 3,
            "screensize", GL_FLOAT_VEC2,
            "color", GL_FLOAT_VEC4,
            "sampler", GL_SAMPLER_2D);
        ogx_shader_add_attributes(shader, 1,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS);
    } else if (source_hash == s_hash_background_template) {
        /* backgroundtemplate.vsh */
        ogx_shader_add_uniforms(shader, 4,
            "screensize", GL_FLOAT_VEC2,
            "randomsize", GL_FLOAT_VEC2,
            "time", GL_FLOAT,
            "sampler", GL_SAMPLER_2D);
        ogx_shader_add_attributes(shader, 1,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS);
    } else if (source_hash == s_hash_icons_fsh) {
        /* icons.fsh */
        ogx_shader_add_uniforms(shader, 3,
            "projection", GL_FLOAT_MAT4,
            "color", GL_FLOAT_VEC4,
            "sampler", GL_SAMPLER_2D);
        ogx_shader_add_attributes(shader, 2,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS,
            "texcoord", GL_FLOAT_VEC2, GX_VA_TEX0);
    } else if (source_hash == s_hash_decal_fsh) {
        /* decal.fsh */
        ogx_shader_add_uniforms(shader, 3,
            "projection", GL_FLOAT_MAT4,
            "opacity", GL_FLOAT,
            "sampler", GL_SAMPLER_2D);
        ogx_shader_add_attributes(shader, 2,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS,
            "texcoord", GL_FLOAT_VEC2, GX_VA_TEX0);
    } else if (source_hash == s_hash_shadow) {
        /* shadow.vsh */
        ogx_shader_add_uniforms(shader, 1,
            "projection", GL_FLOAT_MAT4);
        ogx_shader_add_attributes(shader, 2,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS,
            /* NOTE: this is a trick: we need to pass a single float, and the
             * only GX attribute that accepts values with a single component is
             * texture coordinates (for 1D textures). In the shader code  */
            "opacity", GL_FLOAT, GX_VA_CLR0);
    } else if (source_hash == s_hash_level_preview) {
        /* levelpreview.vsh */
        ogx_shader_add_uniforms(shader, 2,
            "projection", GL_FLOAT_MAT4,
            "sampler", GL_SAMPLER_2D);
        ogx_shader_add_attributes(shader, 2,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS,
            "texcoord", GL_FLOAT_VEC2, GX_VA_TEX0);
    } else if (source_hash == s_hash_cached_screen) {
        /* cachedscreen.vsh */
        ogx_shader_add_uniforms(shader, 3,
            "offset", GL_FLOAT_VEC2,
            "subsize", GL_FLOAT_VEC2,
            "sampler", GL_SAMPLER_2D);
        ogx_shader_add_attributes(shader, 1,
            "vtxcoord", GL_FLOAT_VEC4, GX_VA_POS);
    }
    return true;
}

static GLenum link_program(GLuint program)
{
    GLuint shaders[2];
    int count = 0;
    glGetAttachedShaders(program, 2, &count, shaders);
    uint32_t vertex_shader_hash = ogx_shader_get_source_hash(shaders[0]);
    uint32_t fragment_shader_hash = ogx_shader_get_source_hash(shaders[1]);
    if (vertex_shader_hash == s_hash_loading) {
        auto *data = new ShaderLoading;
        data->projection_loc = glGetUniformLocation(program, "projection");
        data->color_loc = glGetUniformLocation(program, "color");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) { delete static_cast<ShaderLoading*>(data); });
        ogx_shader_program_set_setup_draw_cb(program, setup_draw_loading);
        ogx_shader_program_set_draw_done_cb(program, draw_done_loading);
    } else if (vertex_shader_hash == s_hash_spheredefault) {
        auto *data = new ShaderSphereDefault;
        data->projection_loc = glGetUniformLocation(program, "projection");
        data->sampler_loc = glGetUniformLocation(program, "sampler");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) { delete static_cast<ShaderSphereDefault*>(data); });
        ogx_shader_program_set_setup_draw_cb(program, setup_draw_sphere_default);
        ogx_shader_program_set_draw_done_cb(program, draw_done_sphere_default);
    } else if (vertex_shader_hash == s_hash_spheretemplate) {
        auto *data = new ShaderSphereTemplate;
        data->framebuffer_size_loc = glGetUniformLocation(program, "framebuffer_size");
        data->center_loc = glGetUniformLocation(program, "center");
        data->size_loc = glGetUniformLocation(program, "size");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) {
            delete static_cast<ShaderSphereTemplate*>(data); });
        ogx_shader_program_set_setup_draw_cb(program,
                                             setup_draw_sphere_template);
        ogx_shader_program_set_setup_matrices_cb(program,
                                                 setup_matrices_identity_proj);
    } else if (vertex_shader_hash == s_hash_effect) {
        auto *data = new ShaderEffect;
        data->subtex_loc = glGetUniformLocation(program, "subtex");
        data->brightness_loc = glGetUniformLocation(program, "brightness");
        data->frame_a_loc = glGetUniformLocation(program, "frame_a");
        data->frame_b_loc = glGetUniformLocation(program, "frame_b");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) {
            delete static_cast<ShaderEffect*>(data); });
        ogx_shader_program_set_setup_draw_cb(program, setup_draw_effect);
        ogx_shader_program_set_setup_matrices_cb(program,
                                                 setup_matrices_identity_proj);
    } else if (vertex_shader_hash == s_hash_line) {
        auto *data = new ShaderLine;
        data->projection_loc = glGetUniformLocation(program, "projection");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) { delete static_cast<ShaderLine*>(data); });
        ogx_shader_program_set_setup_draw_cb(program, setup_draw_line);
    } else if (vertex_shader_hash == s_hash_font) {
        auto *data = new ShaderFont;
        data->projection_loc = glGetUniformLocation(program, "projection");
        data->transform_loc = glGetUniformLocation(program, "transform");
        data->color_loc = glGetUniformLocation(program, "color");
        data->sampler_loc = glGetUniformLocation(program, "sampler");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) { delete static_cast<ShaderFont*>(data); });
        ogx_shader_program_set_setup_draw_cb(program, setup_draw_font);
        ogx_shader_program_set_setup_matrices_cb(program, setup_matrices_font);
        ogx_shader_program_set_draw_done_cb(program, draw_done_font);
    } else if (vertex_shader_hash == s_hash_pageindicator) {
        auto *data = new ShaderPageIndicator;
        data->projection_loc = glGetUniformLocation(program, "projection");
        data->center_loc = glGetUniformLocation(program, "center");
        data->size_loc = glGetUniformLocation(program, "size");
        data->filled_loc = glGetUniformLocation(program, "filled");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) { delete static_cast<ShaderPageIndicator*>(data); });
        ogx_shader_program_set_setup_draw_cb(program, setup_draw_page_indicator);
    } else if (vertex_shader_hash == s_hash_effect_overlay) {
        auto *data = new ShaderEffectOverlay;
        data->subtex_loc = glGetUniformLocation(program, "subtex");
        data->sampler_loc = glGetUniformLocation(program, "sampler");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) {
            delete static_cast<ShaderEffectOverlay*>(data); });
        ogx_shader_program_set_setup_draw_cb(program, setup_draw_effect_overlay);
        ogx_shader_program_set_setup_matrices_cb(program,
                                                 setup_matrices_identity_proj);
    } else if (vertex_shader_hash == s_hash_effect_overlay_template) {
        auto *data = new ShaderEffectOverlayTemplate;
        data->portrait_loc = glGetUniformLocation(program, "portrait");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) {
            delete static_cast<ShaderEffectOverlayTemplate*>(data); });
        ogx_shader_program_set_setup_draw_cb(program,
            setup_draw_effect_overlay_template);
        ogx_shader_program_set_setup_matrices_cb(program,
                                                 setup_matrices_identity_proj);
    } else if (vertex_shader_hash == s_hash_background) {
        auto *data = new ShaderBackground;
        data->screensize_loc = glGetUniformLocation(program, "screensize");
        data->color_loc = glGetUniformLocation(program, "color");
        data->sampler_loc = glGetUniformLocation(program, "sampler");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) {
            delete static_cast<ShaderBackground*>(data); });
        ogx_shader_program_set_setup_draw_cb(program, setup_draw_background);
        ogx_shader_program_set_setup_matrices_cb(program,
                                                 setup_matrices_identity_proj);
        ogx_shader_program_set_draw_done_cb(program, draw_done_background);
    } else if (vertex_shader_hash == s_hash_background_template) {
        auto *data = new ShaderBackgroundTemplate;
        data->screensize_loc = glGetUniformLocation(program, "screensize");
        data->randomsize_loc = glGetUniformLocation(program, "randomsize");
        data->sampler_loc = glGetUniformLocation(program, "sampler");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) {
            delete static_cast<ShaderBackgroundTemplate*>(data); });
        ogx_shader_program_set_setup_draw_cb(program,
                                             setup_draw_background_template);
        ogx_shader_program_set_setup_matrices_cb(program,
                                                 setup_matrices_identity_proj);
        ogx_shader_program_set_draw_done_cb(program,
                                            draw_done_background_template);
    } else if (vertex_shader_hash == s_hash_icons &&
               fragment_shader_hash == s_hash_icons_fsh) {
        auto *data = new ShaderIcons;
        data->projection_loc = glGetUniformLocation(program, "projection");
        data->color_loc = glGetUniformLocation(program, "color");
        data->sampler_loc = glGetUniformLocation(program, "sampler");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) {
            delete static_cast<ShaderIcons*>(data); });
        ogx_shader_program_set_setup_draw_cb(program, setup_draw_icons);
        ogx_shader_program_set_draw_done_cb(program, draw_done_icons);
    } else if (vertex_shader_hash == s_hash_decal &&
               fragment_shader_hash == s_hash_decal_fsh) {
        auto *data = new ShaderDecal;
        data->projection_loc = glGetUniformLocation(program, "projection");
        data->opacity_loc = glGetUniformLocation(program, "opacity");
        data->sampler_loc = glGetUniformLocation(program, "sampler");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) {
            delete static_cast<ShaderDecal*>(data); });
        ogx_shader_program_set_setup_draw_cb(program, setup_draw_decal);
    } else if (vertex_shader_hash == s_hash_shadow) {
        auto *data = new ShaderShadow;
        data->projection_loc = glGetUniformLocation(program, "projection");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) {
            delete static_cast<ShaderShadow*>(data); });
        ogx_shader_program_set_setup_draw_cb(program, setup_draw_shadow);
    } else if (vertex_shader_hash == s_hash_level_preview) {
        auto *data = new ShaderLevelPreview;
        data->projection_loc = glGetUniformLocation(program, "projection");
        data->sampler_loc = glGetUniformLocation(program, "sampler");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) {
            delete static_cast<ShaderLevelPreview*>(data); });
        ogx_shader_program_set_setup_draw_cb(program, setup_draw_level_preview);
    } else if (vertex_shader_hash == s_hash_cached_screen) {
        auto *data = new ShaderCachedScreen;
        data->offset_loc = glGetUniformLocation(program, "offset");
        data->subsize_loc = glGetUniformLocation(program, "subsize");
        data->sampler_loc = glGetUniformLocation(program, "sampler");
        ogx_shader_program_set_user_data(program, data,
                                         [](void *data) {
            delete static_cast<ShaderCachedScreen*>(data); });
        ogx_shader_program_set_setup_draw_cb(program, setup_draw_cached_screen);
        ogx_shader_program_set_setup_matrices_cb(program,
                                                 setup_matrices_identity_proj);
    }
    return GL_NO_ERROR;
}

static void shader_source(GLuint shader, GLsizei count,
                          const GLchar *const*string, const GLint *length)
{
    fprintf(stderr, "%s:%d shader %x source %s\n", __FILE__, __LINE__, shader, string[0]);
}

static const OgxProgramProcessor s_processor = {
    .compile_shader = shader_compile,
    .shader_source = shader_source,
    .link_program = link_program,
};

void setup_opengx_shaders()
{
    ogx_shader_register_program_processor(&s_processor);
}

#endif // __wii__ || __gamecube__
