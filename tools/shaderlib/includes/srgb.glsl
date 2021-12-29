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