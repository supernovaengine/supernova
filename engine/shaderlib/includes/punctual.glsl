struct Light{
    int type;

    vec3 direction;
    vec3 color;
    vec3 position;

    float range;
    float intensity;

    float innerConeCos;
    float outerConeCos;

    vec3 spotUp;
    float spotMaskAspect;

    bool shadows;
    int shadowMapIndex;
    int numShadowCascades;
};

const int LightType_Directional = 0;
const int LightType_Point = 1;
const int LightType_Spot = 2;

// https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_lights_punctual/README.md#range-property
float getRangeAttenuation(float range, float distance)
{
    if (range <= 0.0)
    {
        // negative range means unlimited
        return 1.0 / pow(distance, 2.0);
    }
    return max(min(1.0 - pow(distance / range, 4.0), 1.0), 0.0) / pow(distance, 2.0);
}

// https://github.com/KhronosGroup/glTF/blob/master/extensions/2.0/Khronos/KHR_lights_punctual/README.md#inner-and-outer-cone-angles
float getSpotAttenuation(vec3 pointToLight, vec3 spotDirection, float outerConeCos, float innerConeCos)
{
    float actualCos = dot(normalize(spotDirection), normalize(-pointToLight));
    if (actualCos > outerConeCos)
    {
        if (actualCos < innerConeCos)
        {
            return smoothstep(outerConeCos, innerConeCos, actualCos);
        }
        return 1.0;
    }
    return 0.0;
}

#ifndef DIRECTIONAL_LIGHTS_ONLY
float getSpotMaskAttenuation(Light light, vec3 pointToLight, int lightIndex)
{
    vec3 forward = normalize(light.direction);
    vec3 up = light.spotUp - forward * dot(light.spotUp, forward);
    float upLength = length(up);
    if (upLength <= 0.0001)
    {
        return 0.0;
    }
    up /= upLength;
    vec3 right = normalize(cross(forward, up));

    vec3 ray = normalize(-pointToLight);
    float forwardDistance = dot(ray, forward);
    if (forwardDistance <= 0.0)
    {
        return 0.0;
    }

    float outerSin = sqrt(max(1.0 - light.outerConeCos * light.outerConeCos, 0.0));
    float outerTan = outerSin / max(light.outerConeCos, 0.0001);
    vec2 halfExtents = outerTan * vec2(light.spotMaskAspect, 1.0);
    vec2 projected = vec2(dot(ray, right), dot(ray, up)) / forwardDistance;
    vec2 maskPosition = projected / max(halfExtents, vec2(0.0001));

    if (any(greaterThan(abs(maskPosition), vec2(1.0))))
    {
        return 0.0;
    }

    // Doriax texture coordinates already match the uploaded image orientation.
    vec2 localUV = maskPosition * 0.5 + 0.5;
    ivec2 atlasDimensionsI = textureSize(
        sampler2D(u_spotMaskAtlas, u_spotMaskAtlas_smp),
        0);
    vec2 atlasDimensions = vec2(atlasDimensionsI);
    vec2 tileDimensions = vec2(atlasDimensions.x / float(MAX_LIGHTS), atlasDimensions.y);
    vec2 inset = 0.5 / tileDimensions;
    localUV = clamp(localUV, inset, 1.0 - inset);

    vec2 atlasUV = vec2(
        (float(lightIndex) + localUV.x) / float(MAX_LIGHTS),
        localUV.y
    );
    return texture(sampler2D(u_spotMaskAtlas, u_spotMaskAtlas_smp), atlasUV).r;
}
#endif

vec3 getLighIntensity(Light light, vec3 pointToLight, int lightIndex)
{
    float rangeAttenuation = 1.0;
    float spotAttenuation = 1.0;

    if (light.type != LightType_Directional)
    {
        rangeAttenuation = getRangeAttenuation(light.range, length(pointToLight));
    }
    #ifndef DIRECTIONAL_LIGHTS_ONLY
    if (light.type == LightType_Spot)
    {
        if (light.spotMaskAspect > 0.0)
        {
            spotAttenuation = getSpotMaskAttenuation(light, pointToLight, lightIndex);
        }
        else
        {
            spotAttenuation = getSpotAttenuation(pointToLight, light.direction, light.outerConeCos, light.innerConeCos);
        }
    }
    #endif

    return rangeAttenuation * spotAttenuation * light.intensity * light.color;
}
