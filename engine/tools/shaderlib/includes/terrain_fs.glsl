uniform texture2D u_blendMap;
uniform texture2D u_terrainDetailR;
uniform texture2D u_terrainDetailG;
uniform texture2D u_terrainDetailB;
uniform sampler u_blendMap_smp;
uniform sampler u_terrainDetailR_smp;
uniform sampler u_terrainDetailG_smp;
uniform sampler u_terrainDetailB_smp;

in vec2 v_terrainTextureCoords;
in vec2 v_terrainTextureDetailTiled;
in float v_terrainDetailHeight;
in float v_terrainEyeTiles;
#ifdef HAS_NORMALS
    in vec3 v_terrainNormal;
#endif

// A coarser second tiling rate fades in over this many detail tiles of distance
const float TERRAIN_TILE_BREAK_NEAR = 6.0;
const float TERRAIN_TILE_BREAK_FAR = 40.0;
const float TERRAIN_TILE_BREAK_RATE = 0.371;
const float TERRAIN_TILE_BREAK_MIX = 0.5;

// Slope, as 1.0 - abs(normal.y), over which the side projections take over from the flat one
const float TERRAIN_TRIPLANAR_START = 0.15;
const float TERRAIN_TRIPLANAR_END = 0.6;

// How hard layer height biases the blend. 0.0 leaves the plain blend map weights
const float TERRAIN_HEIGHT_CONTRAST = 6.0;

// Detail alpha carries layer height and biases the blend toward the taller layer, so it takes
// the contact zone. An opaque detail has no height and blends on its map weight alone.
vec4 getTerrainColor(vec4 color){
    vec4 blendMapColor = texture(sampler2D(u_blendMap, u_blendMap_smp), v_terrainTextureCoords);

    // One tiling rate repeats visibly in the distance, so a coarser one fades in over it
    vec2 farUV = v_terrainTextureDetailTiled * TERRAIN_TILE_BREAK_RATE;
    float farAmount = smoothstep(TERRAIN_TILE_BREAK_NEAR, TERRAIN_TILE_BREAK_FAR, v_terrainEyeTiles) * TERRAIN_TILE_BREAK_MIX;

    vec4 detailR = mix(texture(sampler2D(u_terrainDetailR, u_terrainDetailR_smp), v_terrainTextureDetailTiled),
                       texture(sampler2D(u_terrainDetailR, u_terrainDetailR_smp), farUV), farAmount);
    vec4 detailG = mix(texture(sampler2D(u_terrainDetailG, u_terrainDetailG_smp), v_terrainTextureDetailTiled),
                       texture(sampler2D(u_terrainDetailG, u_terrainDetailG_smp), farUV), farAmount);
    vec4 detailB = mix(texture(sampler2D(u_terrainDetailB, u_terrainDetailB_smp), v_terrainTextureDetailTiled),
                       texture(sampler2D(u_terrainDetailB, u_terrainDetailB_smp), farUV), farAmount);

    #ifdef HAS_NORMALS
        // A flat projection stretches over a cliff, so steep ground moves to the side planes
        vec3 normal = normalize(v_terrainNormal);
        float sideAmount = smoothstep(TERRAIN_TRIPLANAR_START, TERRAIN_TRIPLANAR_END, 1.0 - abs(normal.y));
        vec2 sideWeight = vec2(abs(normal.x), abs(normal.z));
        sideWeight = sideWeight / max(sideWeight.x + sideWeight.y, 0.0001);

        // The side U axis follows the face direction, or opposite faces come out mirrored
        float faceX = (normal.x < 0.0) ? -1.0 : 1.0;
        float faceZ = (normal.z < 0.0) ? 1.0 : -1.0;
        vec2 sideXUV = vec2(v_terrainTextureDetailTiled.y * faceX, v_terrainDetailHeight);
        vec2 sideZUV = vec2(v_terrainTextureDetailTiled.x * faceZ, v_terrainDetailHeight);

        detailR = mix(detailR,
            texture(sampler2D(u_terrainDetailR, u_terrainDetailR_smp), sideXUV) * sideWeight.x +
            texture(sampler2D(u_terrainDetailR, u_terrainDetailR_smp), sideZUV) * sideWeight.y, sideAmount);
        detailG = mix(detailG,
            texture(sampler2D(u_terrainDetailG, u_terrainDetailG_smp), sideXUV) * sideWeight.x +
            texture(sampler2D(u_terrainDetailG, u_terrainDetailG_smp), sideZUV) * sideWeight.y, sideAmount);
        detailB = mix(detailB,
            texture(sampler2D(u_terrainDetailB, u_terrainDetailB_smp), sideXUV) * sideWeight.x +
            texture(sampler2D(u_terrainDetailB, u_terrainDetailB_smp), sideZUV) * sideWeight.y, sideAmount);
    #endif

    // The base layer has no height map of its own, so it stands as solid as an opaque detail
    float weightBase = max(1.0 - (blendMapColor.r + blendMapColor.g + blendMapColor.b), 0.0);
    float weightR = blendMapColor.r * pow(detailR.a, TERRAIN_HEIGHT_CONTRAST);
    float weightG = blendMapColor.g * pow(detailG.a, TERRAIN_HEIGHT_CONTRAST);
    float weightB = blendMapColor.b * pow(detailB.a, TERRAIN_HEIGHT_CONTRAST);

    vec4 result = (color * weightBase + detailR * weightR + detailG * weightG + detailB * weightB) /
                  max(weightBase + weightR + weightG + weightB, 0.0001);

    // Height belongs to the blend, not the surface: the material keeps its own alpha
    result.a = color.a;

    return result;
}
