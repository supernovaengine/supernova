uniform texture2D u_heightMap;
uniform sampler u_heightMap_smp;

uniform u_vs_terrainParams {
    vec3 eyePos;
    float size;
    float maxHeight;
    float resolution;
    float textureBaseTiles; //int
    float textureDetailTiles; //int
} terrain;

// instancing part
in vec2 i_terrainnode_pos;
in float i_terrainnode_size;
in float i_terrainnode_range;
in float i_terrainnode_resolution; //int

#ifndef DEPTH_SHADER
    out vec2 v_terrainTextureCoords;
    out vec2 v_terrainTextureDetailTiled;
    // surface height in the same units as the tiled coords, for the triplanar side planes
    out float v_terrainDetailHeight;
    // eye distance counted in detail tiles, so the tiling break follows the tile size
    out float v_terrainEyeTiles;
    #ifdef HAS_NORMALS
        out vec3 v_terrainNormal;
    #endif
#endif

float morphFactor;


vec2 morphVertex(vec2 gridPos, vec2 worldPos, float morph) {
    vec2 gridDim = vec2(i_terrainnode_resolution, i_terrainnode_resolution);

    // only the odd vertices morph, so gridPos (in the patch range [-0.5, 0.5]) is moved to
    // [0, 1] and rounded to the vertex index: scaling it loses the parity in float32
    vec2 index = floor((gridPos + 0.5) * gridDim.xy + 0.5);
    vec2 fracPart = fract(index * 0.5) * 2.0 / gridDim.xy;
    return worldPos - fracPart * i_terrainnode_size * morph;
}

float getHeight(vec3 position) {
    return texture(sampler2D(u_heightMap, u_heightMap_smp), (position.xz + (terrain.size/2.0)) / terrain.size).r * terrain.maxHeight;
}

// must be called BEFORE getTerrainNormal because morphValue
vec3 getTerrainPosition(vec3 pos, mat4 modelMatrix){
    pos = i_terrainnode_size * pos;
    pos = pos + vec3(i_terrainnode_pos[0], 0.0, i_terrainnode_pos[1]);

    pos = vec3(pos.x, getHeight(pos), pos.z);

    float morphStart = 0.0;
    float morphEnd = 0.4;

    float dist = distance(terrain.eyePos, vec3(modelMatrix * vec4(pos, 1.0)));

    #ifndef DEPTH_SHADER
        v_terrainEyeTiles = dist * float(terrain.textureDetailTiles) / terrain.size;
    #endif

    float nextlevel_thresh = ((i_terrainnode_range - dist) / i_terrainnode_size * i_terrainnode_resolution / float(terrain.resolution));
    morphFactor = 1.0 - smoothstep(morphStart, morphEnd, nextlevel_thresh);

    pos.xz = morphVertex(a_position.xz, pos.xz, morphFactor);

    pos = vec3(pos.x, getHeight(pos), pos.z);

    return pos;
}

vec3 getTerrainNormal(vec3 normal, vec3 position){
    #ifdef HAS_NORMALS
        float delta = (morphFactor + 1.0) * i_terrainnode_size / float(i_terrainnode_resolution);

        vec3 dA = delta * normalize(cross(normal.yzx, normal));
        vec3 dB = delta * normalize(cross(dA, normal));
        vec3 p = position;
        vec3 pA = position + dA;
        vec3 pB = position + dB;

        float h = getHeight(position);
        float hA = getHeight(pA);
        float hB = getHeight(pB);

        p += normal * h;
        pA += normal * hA;
        pB += normal * hB;

        normal = normalize(cross(pB - p, pA - p));

        #ifndef DEPTH_SHADER
            v_terrainNormal = normal;
        #endif
    #endif

    return normal;
}

#ifndef DEPTH_SHADER
    vec2 getTerrainTiledTexture(vec3 position){
        v_terrainTextureCoords = (position.xz + (terrain.size/2.0)) / terrain.size;
        v_terrainTextureDetailTiled = v_terrainTextureCoords * float(terrain.textureDetailTiles);
        v_terrainDetailHeight = position.y / terrain.size * float(terrain.textureDetailTiles);
        return v_terrainTextureCoords * float(terrain.textureBaseTiles);
    }
#endif