uniform sampler2D u_blendMap;
uniform sampler2D u_terrainDetailR;
uniform sampler2D u_terrainDetailG;
uniform sampler2D u_terrainDetailB;

in vec2 v_terrainTextureCoords;
in vec2 v_terrainTextureDetailTiled;

vec4 getTerrainColor(vec4 color){
    vec4 blendMapColor = texture(u_blendMap, v_terrainTextureCoords);
    float backTextureAmount = 1.0 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
    color = color * backTextureAmount;

    color = color + texture(u_terrainDetailR, v_terrainTextureDetailTiled) * blendMapColor.r;
    color = color + texture(u_terrainDetailG, v_terrainTextureDetailTiled) * blendMapColor.g;
    color = color + texture(u_terrainDetailB, v_terrainTextureDetailTiled) * blendMapColor.b;

    return color;
}