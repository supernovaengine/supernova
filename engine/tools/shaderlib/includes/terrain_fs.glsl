uniform texture2D u_blendMap;
uniform texture2D u_blendMap1;
uniform texture2D u_blendMap2;
uniform texture2DArray u_terrainDetail;
uniform sampler u_blendMap_smp;
uniform sampler u_blendMap1_smp;
uniform sampler u_blendMap2_smp;
uniform sampler u_terrainDetail_smp;

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

// Each blend map weights three layers of the detail array in its RGB
const int TERRAIN_BLENDMAPS = 3;

vec2 terrainFarUV;
float terrainFarAmount;
// Only the painted layers are fetched, and a branch around a fetch has no derivatives of
// its own, so the gradients are taken here where every layer still shares them
vec2 terrainFlatDX;
vec2 terrainFlatDY;
#ifdef HAS_NORMALS
    vec2 terrainSideXUV;
    vec2 terrainSideZUV;
    vec2 terrainSideXDX;
    vec2 terrainSideXDY;
    vec2 terrainSideZDX;
    vec2 terrainSideZDY;
    vec2 terrainSideWeight;
    float terrainSideAmount;
#endif

// A layer costs four fetches, so the projections are resolved once for all of them
void setupTerrainDetail(){
    terrainFarUV = v_terrainTextureDetailTiled * TERRAIN_TILE_BREAK_RATE;
    terrainFarAmount = smoothstep(TERRAIN_TILE_BREAK_NEAR, TERRAIN_TILE_BREAK_FAR, v_terrainEyeTiles) * TERRAIN_TILE_BREAK_MIX;
    terrainFlatDX = dFdx(v_terrainTextureDetailTiled);
    terrainFlatDY = dFdy(v_terrainTextureDetailTiled);

    #ifdef HAS_NORMALS
        vec3 normal = normalize(v_terrainNormal);
        terrainSideAmount = smoothstep(TERRAIN_TRIPLANAR_START, TERRAIN_TRIPLANAR_END, 1.0 - abs(normal.y));
        terrainSideWeight = vec2(abs(normal.x), abs(normal.z));
        terrainSideWeight = terrainSideWeight / max(terrainSideWeight.x + terrainSideWeight.y, 0.0001);

        // The side U axis follows the face direction, or opposite faces come out mirrored
        float faceX = (normal.x < 0.0) ? -1.0 : 1.0;
        float faceZ = (normal.z < 0.0) ? 1.0 : -1.0;
        terrainSideXUV = vec2(v_terrainTextureDetailTiled.y * faceX, v_terrainDetailHeight);
        terrainSideZUV = vec2(v_terrainTextureDetailTiled.x * faceZ, v_terrainDetailHeight);

        float heightDX = dFdx(v_terrainDetailHeight);
        float heightDY = dFdy(v_terrainDetailHeight);
        terrainSideXDX = vec2(terrainFlatDX.y * faceX, heightDX);
        terrainSideXDY = vec2(terrainFlatDY.y * faceX, heightDY);
        terrainSideZDX = vec2(terrainFlatDX.x * faceZ, heightDX);
        terrainSideZDY = vec2(terrainFlatDY.x * faceZ, heightDY);
    #endif
}

// A flat projection stretches over a cliff, so steep ground moves to the side planes
vec4 getTerrainLayer(float layer){
    vec4 color = mix(
        textureGrad(sampler2DArray(u_terrainDetail, u_terrainDetail_smp), vec3(v_terrainTextureDetailTiled, layer), terrainFlatDX, terrainFlatDY),
        textureGrad(sampler2DArray(u_terrainDetail, u_terrainDetail_smp), vec3(terrainFarUV, layer),
                    terrainFlatDX * TERRAIN_TILE_BREAK_RATE, terrainFlatDY * TERRAIN_TILE_BREAK_RATE), terrainFarAmount);

    #ifdef HAS_NORMALS
        vec4 side = textureGrad(sampler2DArray(u_terrainDetail, u_terrainDetail_smp), vec3(terrainSideXUV, layer), terrainSideXDX, terrainSideXDY) * terrainSideWeight.x +
                    textureGrad(sampler2DArray(u_terrainDetail, u_terrainDetail_smp), vec3(terrainSideZUV, layer), terrainSideZDX, terrainSideZDY) * terrainSideWeight.y;
        color = mix(color, side, terrainSideAmount);
    #endif

    return color;
}

// Detail alpha carries layer height and biases the blend toward the taller layer, so it takes
// the contact zone. An opaque detail has no height and blends on its map weight alone.
vec4 getTerrainColor(vec4 color){
    setupTerrainDetail();

    vec3 blend[TERRAIN_BLENDMAPS];
    blend[0] = texture(sampler2D(u_blendMap, u_blendMap_smp), v_terrainTextureCoords).rgb;
    blend[1] = texture(sampler2D(u_blendMap1, u_blendMap1_smp), v_terrainTextureCoords).rgb;
    blend[2] = texture(sampler2D(u_blendMap2, u_blendMap2_smp), v_terrainTextureCoords).rgb;

    // The base holds whatever weight the layers leave unclaimed, and stands as solid as an
    // opaque detail
    float weightBase = 1.0;
    vec4 sum = vec4(0.0);
    float total = 0.0;

    for (int m = 0; m < TERRAIN_BLENDMAPS; m++){
        for (int c = 0; c < 3; c++){
            float mapWeight = blend[m][c];
            weightBase -= mapWeight;
            if (mapWeight > 0.0){
                vec4 layer = getTerrainLayer(float(m * 3 + c));
                float weight = mapWeight * pow(layer.a, TERRAIN_HEIGHT_CONTRAST);
                sum += layer * weight;
                total += weight;
            }
        }
    }

    weightBase = max(weightBase, 0.0);
    sum += color * weightBase;
    total += weightBase;

    vec4 result = sum / max(total, 0.0001);

    // Height belongs to the blend, not the surface: the material keeps its own alpha
    result.a = color.a;

    return result;
}
