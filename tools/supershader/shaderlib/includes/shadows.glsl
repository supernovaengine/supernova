float shadowCalculationPCF(vec4 frag_pos_light_space, float NdotL, vec2 shadow_map_size) {
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
    // proj_coords is in [-1,1] range, transform to [0,1] range
    proj_coords = proj_coords * 0.5 + 0.5;
    float current_depth = proj_coords.z;
    // calculate bias (based on depth map resolution and slope)
    float bias = max(0.005 * (1.0 - NdotL), 0.0005);
    // check whether current frag pos is in shadow
    float shadow = 0.0;
    vec2 texel_size = 1.0 / shadow_map_size;
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcf_depth = decodeDepth(texture(u_shadowMap, proj_coords.xy + vec2(x, y) * texel_size)); 
            shadow += current_depth - bias > pcf_depth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(proj_coords.z > 1.0)
        shadow = 0.0;

    return shadow;
}