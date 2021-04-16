@block texture_vs_attr
    in vec2 a_texcoord1;
    out vec2 v_uv1;
@end

@block texture_vs_main
    v_uv1 = a_texcoord1;
@end

@block texture_fs_attr
    in vec2 v_uv1;
    uniform sampler2D u_baseColorTexture;

    const float GAMMA = 2.2;
    const float INV_GAMMA = 1.0 / GAMMA;

    vec3 linearTosRGB(vec3 color){
        return pow(color, vec3(INV_GAMMA));
    }

    vec3 sRGBToLinear(vec3 srgbIn){
        return vec3(pow(srgbIn.xyz, vec3(GAMMA)));
    }

    vec4 sRGBToLinear(vec4 srgbIn){
        return vec4(sRGBToLinear(srgbIn.xyz), srgbIn.w);
    }
@end

@block texture_fs_main
    frag_color = sRGBToLinear(texture(u_baseColorTexture, v_uv1)) * frag_color;
    frag_color = vec4(linearTosRGB(frag_color.rgb), frag_color.a);
@end