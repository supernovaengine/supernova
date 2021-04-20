@ctype vec2 Supernova::Vector2
@ctype vec3 Supernova::Vector3
@ctype vec4 Supernova::Vector4
@ctype mat4 Supernova::Matrix4

@include mesh-pbr.glsl


//--------------------------------------
@block pbr_unlit_defines
    #define HAS_UV_SET1
    //#define HAS_UV_SET2
    //#define USE_PUNCTUAL
    //#define HAS_NORMALS
    //#define HAS_NORMAL_MAP
    //#define HAS_TANGENTS
    //#define HAS_VERTEX_COLOR_VEC4
    #define MATERIAL_UNLIT
@end

@vs meshPBR_unlit_vs
    @include_block pbr_unlit_defines
    @include_block pbr_vs_attr

    void main() {
        @include_block pbr_vs_main 
    }
@end

@fs meshPBR_unlit_fs
    @include_block pbr_unlit_defines
    @include_block pbr_fs_attr

    void main() {
        @include_block pbr_fs_main 
    }
@end
//--------------------------------------


//--------------------------------------
@block pbr_defines
    #define HAS_UV_SET1
    //#define HAS_UV_SET2
    #define USE_PUNCTUAL
    #define HAS_NORMALS
    #define HAS_NORMAL_MAP
    #define HAS_TANGENTS
    #define HAS_VERTEX_COLOR_VEC4
    //#define MATERIAL_UNLIT
@end

@vs meshPBR_vs
    @include_block pbr_defines
    @include_block pbr_vs_attr

    void main() {
        @include_block pbr_vs_main 
    }
@end

@fs meshPBR_fs
    @include_block pbr_defines
    @include_block pbr_fs_attr

    void main() {
        @include_block pbr_fs_main 
    }
@end
//--------------------------------------


//--------------------------------------
@block pbr_noTan_defines
    #define HAS_UV_SET1
    //#define HAS_UV_SET2
    #define USE_PUNCTUAL
    #define HAS_NORMALS
    #define HAS_NORMAL_MAP
    //#define HAS_TANGENTS
    #define HAS_VERTEX_COLOR_VEC4
@end

@vs meshPBR_noTan_vs
    @include_block pbr_noTan_defines
    @include_block pbr_vs_attr

    void main() {
        @include_block pbr_vs_main 
    }
@end

@fs meshPBR_noTan_fs
    @include_block pbr_noTan_defines
    @include_block pbr_fs_attr

    void main() {
        @include_block pbr_fs_main 
    }
@end
//--------------------------------------


//--------------------------------------
@block pbr_noNmap_defines
    #define HAS_UV_SET1
    //#define HAS_UV_SET2
    #define USE_PUNCTUAL
    #define HAS_NORMALS
    //#define HAS_NORMAL_MAP
    #define HAS_TANGENTS
    #define HAS_VERTEX_COLOR_VEC4
@end

@vs meshPBR_noNmap_vs
    @include_block pbr_noNmap_defines
    @include_block pbr_vs_attr

    void main() {
        @include_block pbr_vs_main 
    }
@end

@fs meshPBR_noNmap_fs
    @include_block pbr_noNmap_defines
    @include_block pbr_fs_attr

    void main() {
        @include_block pbr_fs_main 
    }
@end
//--------------------------------------


//--------------------------------------
@block pbr_noNmap_noTan_defines
    #define HAS_UV_SET1
    //#define HAS_UV_SET2
    #define USE_PUNCTUAL
    #define HAS_NORMALS
    //#define HAS_NORMAL_MAP
    //#define HAS_TANGENTS
    #define HAS_VERTEX_COLOR_VEC4
    //#define MATERIAL_UNLIT
@end

@vs meshPBR_noNmap_noTan_vs
    @include_block pbr_noNmap_noTan_defines
    @include_block pbr_vs_attr

    void main() {
        @include_block pbr_vs_main 
    }
@end

@fs meshPBR_noNmap_noTan_fs
    @include_block pbr_noNmap_noTan_defines
    @include_block pbr_fs_attr

    void main() {
        @include_block pbr_fs_main 
    }
@end
//--------------------------------------


//--------------------------------------
@vs skybox_vs
    in vec3 a_position;

    out vec3 uv;

    uniform viewProjectionSky {
        mat4 u_vpMatrix;
    };

    void main(){
        uv = a_position;
        vec4 pos = u_vpMatrix * vec4(a_position, 1.0);
        gl_Position = pos.xyww;
    } 
@end

@fs skybox_fs
    out vec4 frag_color;

    in vec3 uv;

    uniform samplerCube u_skyTexture;

    void main(){    
        frag_color = texture(u_skyTexture, uv);
    }
@end
//--------------------------------------


//@program mesh mesh_vs mesh_fs

@program meshPBR_unlit meshPBR_unlit_vs meshPBR_unlit_fs

@program meshPBR meshPBR_vs meshPBR_fs

@program meshPBR_noTan meshPBR_noTan_vs meshPBR_noTan_fs
@program meshPBR_noNmap meshPBR_noNmap_vs meshPBR_noNmap_fs
@program meshPBR_noNmap_noTan meshPBR_noNmap_noTan_vs meshPBR_noNmap_noTan_fs

@program skybox skybox_vs skybox_fs