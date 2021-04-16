// Based on https://github.com/KhronosGroup/glTF-Sample-Viewer shaders

@block pbr_vs_attr
    uniform transform {
        mat4 u_modelMatrix;
        mat4 u_mvpMatrix;
    };

    in vec3 a_position;
    out vec3 v_position;

    #ifdef HAS_NORMALS
        in vec3 a_normal;
    #endif

    #ifdef HAS_TANGENTS
        in vec4 a_tangent;
    #endif

    #ifdef HAS_NORMALS
    #ifdef HAS_TANGENTS
        out mat3 v_tbn;
    #else
        out vec3 v_normal;
    #endif
    #endif

    #ifdef HAS_UV_SET1
        in vec2 a_texcoord1;
    #endif

    #ifdef HAS_UV_SET2
        in vec2 a_texcoord2;
    #endif

    out vec2 v_uv1;
    out vec2 v_uv2;

    #ifdef HAS_VERTEX_COLOR_VEC3
        in vec3 a_color;
        out vec3 v_color;
    #endif

    #ifdef HAS_VERTEX_COLOR_VEC4
        in vec4 a_color;
        out vec4 v_color;
    #endif

    vec4 getPosition(){
        vec4 pos = vec4(a_position, 1.0);
        return pos;
    }

    #ifdef HAS_NORMALS
    vec3 getNormal(){
        vec3 normal = a_normal;
        return normalize(normal);
    }
    #endif

    #ifdef HAS_TANGENTS
    vec3 getTangent(){
        vec3 tangent = a_tangent.xyz;
        return normalize(tangent);
    }
    #endif
@end

@block pbr_vs_main
    #ifndef MATERIAL_UNLIT
        vec4 pos = u_modelMatrix * getPosition();
        v_position = vec3(pos.xyz) / pos.w;
    #endif

    mat4 normalMatrix = transpose(inverse(u_modelMatrix));

    #ifdef HAS_NORMALS
    #ifdef HAS_TANGENTS
        vec3 tangent = getTangent();
        vec3 normalW = normalize(vec3(normalMatrix * vec4(getNormal(), 0.0)));
        vec3 tangentW = normalize(vec3(u_modelMatrix * vec4(tangent, 0.0)));
        vec3 bitangentW = cross(normalW, tangentW) * a_tangent.w;
        v_tbn = mat3(tangentW, bitangentW, normalW);
    #else // !HAS_TANGENTS
        v_normal = normalize(vec3(normalMatrix * vec4(getNormal(), 0.0)));
    #endif
    #endif

    v_uv1 = vec2(0.0, 0.0);
    v_uv2 = vec2(0.0, 0.0);

    #ifdef HAS_UV_SET1
        v_uv1 = a_texcoord1;
    #endif

    #ifdef HAS_UV_SET2
        v_uv2 = a_texcoord2;
    #endif

    #if defined(HAS_VERTEX_COLOR_VEC3) || defined(HAS_VERTEX_COLOR_VEC4)
        v_color = a_color;
    #endif

    gl_Position = u_mvpMatrix * getPosition();
@end


@block pbr_fs_attr
    #define NUM_DIRLIGHTS 1

    in vec3 v_position;

    #ifdef HAS_NORMALS
    #ifdef HAS_TANGENTS
        in mat3 v_tbn;
    #else
        in vec3 v_normal;
    #endif
    #endif

    in vec2 v_uv1;
    in vec2 v_uv2;

    #ifdef HAS_VERTEX_COLOR_VEC3
        in vec3 v_color;
    #endif
    #ifdef HAS_VERTEX_COLOR_VEC4
        in vec4 v_color;
    #endif

    out vec4 g_finalColor;

    uniform sampler2D u_baseColorTexture;
    uniform sampler2D u_metallicRoughnessTexture;
    uniform sampler2D u_normalTexture;
    uniform sampler2D u_occlusionTexture;
    uniform sampler2D u_emissiveTexture;

    uniform u_fs_pbrParams {
        //Metallic material
        vec4 baseColorFactor;
        float metallicFactor;
        float roughnessFactor;
        vec3 emissiveFactor;

        //Camera Position
        vec3 eyePos;
    } fs_pbrParams;

    #ifdef USE_PUNCTUAL
    uniform u_lighting{
        vec3 ambient;
        float materialShininess;
        float numDirLights;
        float numSpotLights;
        float numPointLights;
    } lighting;
    
    uniform u_dirLight{
        vec3 direction[NUM_DIRLIGHTS];
        vec3 color[NUM_DIRLIGHTS];
        float intensity[NUM_DIRLIGHTS];
    } dirLight;
    #endif

    struct MaterialInfo{
        float perceptualRoughness;      // roughness value, as authored by the model creator (input to shader)
        vec3 f0;                        // full reflectance color (n incidence angle)

        float alphaRoughness;           // roughness mapped to a more linear change in the roughness (proposed by [2])
        vec3 albedoColor;

        vec3 f90;                       // reflectance color at grazing angle
        float metallic;

        vec3 n;
        vec3 baseColor; // getBaseColor()

        //float sheenRoughnessFactor;
        //vec3 sheenColorFactor;

        //vec3 clearcoatF0;
        //vec3 clearcoatF90;
        //float clearcoatFactor;
        //vec3 clearcoatNormal;
        //float clearcoatRoughness;

        //float transmissionFactor;
    };

    struct NormalInfo {
        vec3 ng;   // Geometric normal
        vec3 n;    // Pertubed normal
        vec3 t;    // Pertubed tangent
        vec3 b;    // Pertubed bitangent
    };

    const float normalScale = 1.0;
    const float occlusionStrength = 1.0;

    const float GAMMA = 2.2;
    const float INV_GAMMA = 1.0 / GAMMA;
    const float M_PI = 3.141592653589793;

    @include funcs-pbr.glsl
    @include funcs-brdf.glsl
@end

@block pbr_fs_main
    vec4 baseColor = getBaseColor();

    #ifdef MATERIAL_UNLIT
        g_finalColor = (vec4(linearTosRGB(baseColor.rgb), baseColor.a));
        return;
    #endif

    vec3 v = normalize(fs_pbrParams.eyePos- v_position);
    NormalInfo normalInfo = getNormalInfo(v);
    vec3 n = normalInfo.n;
    vec3 t = normalInfo.t;
    vec3 b = normalInfo.b;

    float NdotV = clampedDot(n, v);
    float TdotV = clampedDot(t, v);
    float BdotV = clampedDot(b, v);

    MaterialInfo materialInfo;
    materialInfo.baseColor = baseColor.rgb;

    // The default index of refraction of 1.5 yields a dielectric normal incidence reflectance of 0.04.
    float ior = 1.5;
    float f0_ior = 0.04;

    materialInfo = getMetallicRoughnessInfo(materialInfo, f0_ior);

    materialInfo.perceptualRoughness = clamp(materialInfo.perceptualRoughness, 0.0, 1.0);
    materialInfo.metallic = clamp(materialInfo.metallic, 0.0, 1.0);

    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness.
    materialInfo.alphaRoughness = materialInfo.perceptualRoughness * materialInfo.perceptualRoughness;

    // Compute reflectance.
    float reflectance = max(max(materialInfo.f0.r, materialInfo.f0.g), materialInfo.f0.b);

    // Anything less than 2% is physically impossible and is instead considered to be shadowing. Compare to "Real-Time-Rendering" 4th editon on page 325.
    materialInfo.f90 = vec3(clamp(reflectance * 50.0, 0.0, 1.0));

    materialInfo.n = n;

    // LIGHTING
    vec3 f_specular = vec3(0.0);
    vec3 f_diffuse = vec3(0.0);
    vec3 f_emissive = vec3(0.0);

    #ifdef USE_IBL
        //TODO
    #else
        // Add simple ambient light
        f_diffuse += vec3(1.0, 1.0, 1.0) * pow(0.1, GAMMA) * baseColor.rgb;
    #endif

    float ao = getOcclusionTexture().r;
    f_diffuse = mix(f_diffuse, f_diffuse * ao, occlusionStrength);
    // apply ambient occlusion too all lighting that is not punctual
    f_specular = mix(f_specular, f_specular * ao, occlusionStrength);

    // Apply light sources
    #ifdef USE_PUNCTUAL
        for (int i = 0; i < NUM_DIRLIGHTS; ++i){
            vec3 pointToLight = vec3(-dirLight.direction[i]);
            float rangeAttenuation = 1.0;
            float spotAttenuation = 1.0;

            vec3 intensity = rangeAttenuation * spotAttenuation * dirLight.intensity[i] * dirLight.color[i];

            vec3 l = normalize(pointToLight);   // Direction from surface point to light
            vec3 h = normalize(l + v);          // Direction of the vector between l and v, called halfway vector
            float NdotL = clampedDot(n, l);
            float NdotV = clampedDot(n, v);
            float NdotH = clampedDot(n, h);
            float LdotH = clampedDot(l, h);
            float VdotH = clampedDot(v, h);

            if (NdotL > 0.0 || NdotV > 0.0){
                // Calculation of analytical light
                // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
                f_diffuse += intensity * NdotL *  BRDF_lambertian(materialInfo.f0, materialInfo.f90, materialInfo.albedoColor, VdotH);
                f_specular += intensity * NdotL * BRDF_specularGGX(materialInfo.f0, materialInfo.f90, materialInfo.alphaRoughness, VdotH, NdotL, NdotV, NdotH);
            }
    }
    #endif

    f_emissive = fs_pbrParams.emissiveFactor;
    f_emissive *= sRGBToLinear(getEmissiveTexture().rgb);

    vec3 color = f_emissive + f_diffuse + f_specular;

    g_finalColor = vec4(linearTosRGB(color.rgb), baseColor.a);
@end