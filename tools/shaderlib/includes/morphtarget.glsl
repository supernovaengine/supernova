#ifdef HAS_MORPHTARGET
    in vec3 a_morphTarget0;
    #ifdef HAS_MORPHNORMAL
        in vec3 a_morphNormal0;
    #endif
    #ifdef HAS_MORPHTANGENT
        in vec3 a_morphTangent0;
    #endif
    in vec3 a_morphTarget1;
    #ifdef HAS_MORPHNORMAL
        in vec3 a_morphNormal1;
    #endif
    #ifdef HAS_MORPHTANGENT
        in vec3 a_morphTangent1;
    #endif
    in vec3 a_morphTarget2;
    #ifdef HAS_MORPHNORMAL
        in vec3 a_morphNormal2;
    #endif
    in vec3 a_morphTarget3;
    #ifdef HAS_MORPHNORMAL
        in vec3 a_morphNormal3;
    #endif
    #if !defined(HAS_MORPHNORMAL) && !defined(HAS_MORPHTANGENT)
        in vec3 a_morphTarget4;
        in vec3 a_morphTarget5;
        in vec3 a_morphTarget6;
        in vec3 a_morphTarget7;
    #endif

    uniform u_vs_morphtarget {
        vec4 morphWeights[MAX_MORPHTARGETS];
    };
#endif

vec3 getMorphPosition(vec3 pos){
    #ifdef HAS_MORPHTARGET
        pos += (morphWeights[0].x * a_morphTarget0);
        pos += (morphWeights[0].y * a_morphTarget1);
        pos += (morphWeights[0].z * a_morphTarget2);
        pos += (morphWeights[0].w * a_morphTarget3);
        #if !defined(HAS_MORPHNORMAL) && !defined(HAS_MORPHTANGENT)
            pos += (morphWeights[1].x * a_morphTarget4);
            pos += (morphWeights[1].y * a_morphTarget5);
            pos += (morphWeights[1].z * a_morphTarget6);
            pos += (morphWeights[1].w * a_morphTarget7);
        #endif
    #endif

    return pos;
}

vec3 getMorphNormal(vec3 normal){
    #ifdef HAS_MORPHNORMAL
        normal += (morphWeights[0].x * a_morphNormal0);
        normal += (morphWeights[0].y * a_morphNormal1);
        normal += (morphWeights[0].z * a_morphNormal2);
        normal += (morphWeights[0].w * a_morphNormal3);
    #endif

    return normal;
}

vec3 getMorphTangent(vec3 tangent){
    #ifdef HAS_MORPHTANGENT
        tangent += (morphWeights[0].x * a_morphTangent0);
        tangent += (morphWeights[0].y * a_morphTangent1);
    #endif

    return tangent;
}