// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "IBLEnvironment.h"

#include "Log.h"
#include "render/SystemRender.h"

#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>

using namespace doriax;

namespace {

    const float IBL_PI = 3.14159265358979323846f;

    struct Vec3f{
        float x, y, z;
    };

    // one cubemap level in linear float RGB
    struct CubeLevel{
        int size = 0;
        std::vector<float> faces[6]; // size*size*3
    };

    void cleanupBuffer(void* data){
        free(data);
    }

    const float* getSRGBToLinearLUT(){
        static float lut[256];
        static bool initialized = false;
        if (!initialized){
            for (int i = 0; i < 256; i++){
                lut[i] = powf(i / 255.0f, 2.2f);
            }
            initialized = true;
        }
        return lut;
    }

    unsigned char linearToSRGB8(float v){
        v = std::max(0.0f, std::min(1.0f, v));
        return (unsigned char)(powf(v, 1.0f / 2.2f) * 255.0f + 0.5f);
    }

    // cubemap convention (sokol/OpenGL style): 0=+X, 1=-X, 2=+Y, 3=-Y, 4=+Z, 5=-Z
    Vec3f faceTexelDir(int face, float u, float v){
        // u, v in [-1, 1]
        Vec3f dir;
        switch (face){
            case 0: dir = { 1.0f,    -v,    -u}; break;
            case 1: dir = {-1.0f,    -v,     u}; break;
            case 2: dir = {    u,  1.0f,     v}; break;
            case 3: dir = {    u, -1.0f,    -v}; break;
            case 4: dir = {    u,    -v,  1.0f}; break;
            default:dir = {   -u,    -v, -1.0f}; break;
        }
        float len = sqrtf(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
        dir.x /= len; dir.y /= len; dir.z /= len;
        return dir;
    }

    void dirToFaceUV(const Vec3f& dir, int& face, float& u, float& v){
        float ax = fabsf(dir.x);
        float ay = fabsf(dir.y);
        float az = fabsf(dir.z);

        if (ax >= ay && ax >= az){
            if (dir.x > 0.0f){
                face = 0; u = -dir.z / ax; v = -dir.y / ax;
            }else{
                face = 1; u = dir.z / ax; v = -dir.y / ax;
            }
        }else if (ay >= ax && ay >= az){
            if (dir.y > 0.0f){
                face = 2; u = dir.x / ay; v = dir.z / ay;
            }else{
                face = 3; u = dir.x / ay; v = -dir.z / ay;
            }
        }else{
            if (dir.z > 0.0f){
                face = 4; u = dir.x / az; v = -dir.y / az;
            }else{
                face = 5; u = -dir.x / az; v = -dir.y / az;
            }
        }
    }

    // bilinear fetch inside one face (clamped to edge)
    void fetchFaceBilinear(const CubeLevel& level, int face, float s, float t, float out[3]){
        int size = level.size;
        float x = s * size - 0.5f;
        float y = t * size - 0.5f;
        int x0 = (int)floorf(x);
        int y0 = (int)floorf(y);
        float fx = x - x0;
        float fy = y - y0;
        int x1 = std::min(x0 + 1, size - 1);
        int y1 = std::min(y0 + 1, size - 1);
        x0 = std::max(x0, 0);
        y0 = std::max(y0, 0);

        const float* data = level.faces[face].data();
        const float* p00 = &data[(y0 * size + x0) * 3];
        const float* p10 = &data[(y0 * size + x1) * 3];
        const float* p01 = &data[(y1 * size + x0) * 3];
        const float* p11 = &data[(y1 * size + x1) * 3];

        for (int c = 0; c < 3; c++){
            float a = p00[c] + (p10[c] - p00[c]) * fx;
            float b = p01[c] + (p11[c] - p01[c]) * fx;
            out[c] = a + (b - a) * fy;
        }
    }

    void sampleCube(const CubeLevel& level, const Vec3f& dir, float out[3]){
        int face;
        float u, v;
        dirToFaceUV(dir, face, u, v);
        fetchFaceBilinear(level, face, u * 0.5f + 0.5f, v * 0.5f + 0.5f, out);
    }

    // resample one input face to dstSize, converting to linear float RGB
    bool resampleFaceLinear(TextureData& src, int dstSize, std::vector<float>& out){
        unsigned char* data = (unsigned char*)src.getData();
        int srcWidth = src.getWidth();
        int srcHeight = src.getHeight();
        int channels = src.getChannels();

        if (!data || srcWidth <= 0 || srcHeight <= 0 || channels <= 0){
            return false;
        }

        const float* lut = getSRGBToLinearLUT();

        out.resize(dstSize * dstSize * 3);

        for (int y = 0; y < dstSize; y++){
            float t = (y + 0.5f) / dstSize;
            float sy = t * srcHeight - 0.5f;
            int y0 = std::max((int)floorf(sy), 0);
            int y1 = std::min(y0 + 1, srcHeight - 1);
            float fy = sy - floorf(sy);

            for (int x = 0; x < dstSize; x++){
                float s = (x + 0.5f) / dstSize;
                float sx = s * srcWidth - 0.5f;
                int x0 = std::max((int)floorf(sx), 0);
                int x1 = std::min(x0 + 1, srcWidth - 1);
                float fx = sx - floorf(sx);

                float* dst = &out[(y * dstSize + x) * 3];
                for (int c = 0; c < 3; c++){
                    int sc = (channels >= 3)? c : 0;
                    float p00 = lut[data[(y0 * srcWidth + x0) * channels + sc]];
                    float p10 = lut[data[(y0 * srcWidth + x1) * channels + sc]];
                    float p01 = lut[data[(y1 * srcWidth + x0) * channels + sc]];
                    float p11 = lut[data[(y1 * srcWidth + x1) * channels + sc]];
                    float a = p00 + (p10 - p00) * fx;
                    float b = p01 + (p11 - p01) * fx;
                    dst[c] = a + (b - a) * fy;
                }
            }
        }

        return true;
    }

    void downsampleLevel(const CubeLevel& src, CubeLevel& dst){
        dst.size = std::max(src.size / 2, 1);
        for (int f = 0; f < 6; f++){
            dst.faces[f].resize(dst.size * dst.size * 3);
            for (int y = 0; y < dst.size; y++){
                int sy0 = std::min(y * 2, src.size - 1);
                int sy1 = std::min(y * 2 + 1, src.size - 1);
                for (int x = 0; x < dst.size; x++){
                    int sx0 = std::min(x * 2, src.size - 1);
                    int sx1 = std::min(x * 2 + 1, src.size - 1);
                    for (int c = 0; c < 3; c++){
                        float sum = src.faces[f][(sy0 * src.size + sx0) * 3 + c] +
                                    src.faces[f][(sy0 * src.size + sx1) * 3 + c] +
                                    src.faces[f][(sy1 * src.size + sx0) * 3 + c] +
                                    src.faces[f][(sy1 * src.size + sx1) * 3 + c];
                        dst.faces[f][(y * dst.size + x) * 3 + c] = sum * 0.25f;
                    }
                }
            }
        }
    }

    // solid angle helper of a cubemap texel (https://www.rorydriscoll.com/2012/01/15/cubemap-texel-solid-angle/)
    float areaElement(float x, float y){
        return atan2f(x * y, sqrtf(x * x + y * y + 1.0f));
    }

    float texelSolidAngle(int x, int y, int size){
        float invSize = 1.0f / size;
        float u0 = 2.0f * x * invSize - 1.0f;
        float v0 = 2.0f * y * invSize - 1.0f;
        float u1 = u0 + 2.0f * invSize;
        float v1 = v0 + 2.0f * invSize;
        return areaElement(u0, v0) - areaElement(u0, v1) - areaElement(u1, v0) + areaElement(u1, v1);
    }

    float radicalInverseVdC(unsigned int bits){
        bits = (bits << 16u) | (bits >> 16u);
        bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
        bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
        bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
        bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
        return bits * 2.3283064365386963e-10f;
    }

    struct GGXSample{
        Vec3f l;        // light direction in tangent space (normal = +Z)
        float weight;   // NdotL
        float lod;      // source mip level (set later by caller using pdf)
        float pdf;
    };

    // GGX importance samples around the normal (+Z), with V = N (split-sum approximation)
    void buildGGXSamples(float roughness, int numSamples, std::vector<GGXSample>& samples){
        samples.clear();
        samples.reserve(numSamples);

        float alpha = roughness * roughness;

        for (int i = 0; i < numSamples; i++){
            float e1 = (i + 0.5f) / numSamples;
            float e2 = radicalInverseVdC((unsigned int)i);

            // GGX half vector
            float phi = 2.0f * IBL_PI * e1;
            float cosTheta = sqrtf((1.0f - e2) / (1.0f + (alpha * alpha - 1.0f) * e2));
            float sinTheta = sqrtf(1.0f - cosTheta * cosTheta);

            Vec3f h = {sinTheta * cosf(phi), sinTheta * sinf(phi), cosTheta};

            // V = N = +Z, L = reflect(-V, H)
            Vec3f l = {2.0f * h.z * h.x, 2.0f * h.z * h.y, 2.0f * h.z * h.z - 1.0f};

            if (l.z <= 0.0f)
                continue;

            float d = (cosTheta * alpha * alpha - cosTheta) * cosTheta + 1.0f;
            float D = (alpha * alpha) / (IBL_PI * d * d);
            float pdf = std::max(D * h.z / (4.0f * h.z), 0.0001f); // VdotH == NdotH when V == N

            GGXSample sample;
            sample.l = l;
            sample.weight = l.z;
            sample.lod = 0.0f;
            sample.pdf = pdf;
            samples.push_back(sample);
        }
    }

}

bool IBLEnvironment::generate(const std::string& label, std::array<TextureData,6>& faces, TextureRender& irradianceMap, TextureRender& prefilteredMap){

    // ---- build a linear float mip chain of the environment ----
    std::vector<CubeLevel> chain(SPECULAR_MIPS);
    chain[0].size = SPECULAR_BASE_SIZE;
    for (int f = 0; f < 6; f++){
        if (!resampleFaceLinear(faces[f], SPECULAR_BASE_SIZE, chain[0].faces[f])){
            Log::error("Cannot generate IBL environment of %s: invalid texture data", label.c_str());
            return false;
        }
    }
    for (int level = 1; level < SPECULAR_MIPS; level++){
        downsampleLevel(chain[level - 1], chain[level]);
    }

    // ---- lambertian (irradiance) map: cosine convolution divided by PI ----
    // use a low resolution source level to keep the convolution cheap
    int irrSrcLevel = 0;
    while (chain[irrSrcLevel].size > 32 && irrSrcLevel < SPECULAR_MIPS - 1){
        irrSrcLevel++;
    }
    const CubeLevel& irrSrc = chain[irrSrcLevel];
    int srcSize = irrSrc.size;

    // precompute source texel directions and solid angles
    std::vector<Vec3f> srcDirs(6 * srcSize * srcSize);
    std::vector<float> srcOmega(6 * srcSize * srcSize);
    for (int f = 0; f < 6; f++){
        for (int y = 0; y < srcSize; y++){
            for (int x = 0; x < srcSize; x++){
                int idx = (f * srcSize + y) * srcSize + x;
                srcDirs[idx] = faceTexelDir(f, 2.0f * (x + 0.5f) / srcSize - 1.0f, 2.0f * (y + 0.5f) / srcSize - 1.0f);
                srcOmega[idx] = texelSolidAngle(x, y, srcSize);
            }
        }
    }

    std::vector<unsigned char> irrPixels[6];
    void* irrData[6];
    size_t irrSize[6];

    for (int f = 0; f < 6; f++){
        irrPixels[f].resize(IRRADIANCE_SIZE * IRRADIANCE_SIZE * 4);
        for (int y = 0; y < IRRADIANCE_SIZE; y++){
            for (int x = 0; x < IRRADIANCE_SIZE; x++){
                Vec3f n = faceTexelDir(f, 2.0f * (x + 0.5f) / IRRADIANCE_SIZE - 1.0f, 2.0f * (y + 0.5f) / IRRADIANCE_SIZE - 1.0f);

                float irr[3] = {0.0f, 0.0f, 0.0f};
                for (int sf = 0; sf < 6; sf++){
                    const float* srcData = irrSrc.faces[sf].data();
                    for (int s = 0; s < srcSize * srcSize; s++){
                        int idx = sf * srcSize * srcSize + s;
                        const Vec3f& l = srcDirs[idx];
                        float cosTheta = n.x * l.x + n.y * l.y + n.z * l.z;
                        if (cosTheta > 0.0f){
                            float w = cosTheta * srcOmega[idx];
                            irr[0] += srcData[s * 3] * w;
                            irr[1] += srcData[s * 3 + 1] * w;
                            irr[2] += srcData[s * 3 + 2] * w;
                        }
                    }
                }

                unsigned char* dst = &irrPixels[f][(y * IRRADIANCE_SIZE + x) * 4];
                dst[0] = linearToSRGB8(irr[0] / IBL_PI);
                dst[1] = linearToSRGB8(irr[1] / IBL_PI);
                dst[2] = linearToSRGB8(irr[2] / IBL_PI);
                dst[3] = 255;
            }
        }
        irrData[f] = irrPixels[f].data();
        irrSize[f] = irrPixels[f].size();
    }

    // ---- GGX prefiltered map: one roughness per mip level ----
    const float totalOmega = 4.0f * IBL_PI / (6.0f * SPECULAR_BASE_SIZE * SPECULAR_BASE_SIZE);
    const int numSamples = 128;

    void* specData[SPECULAR_MIPS];
    size_t specSize[SPECULAR_MIPS];

    std::vector<GGXSample> samples;

    for (int level = 0; level < SPECULAR_MIPS; level++){
        int size = SPECULAR_BASE_SIZE >> level;
        size_t levelBytes = (size_t)6 * size * size * 4;
        unsigned char* levelPixels = (unsigned char*)malloc(levelBytes);
        specData[level] = levelPixels;
        specSize[level] = levelBytes;

        if (level == 0){
            // roughness zero, just re-encode the base level
            for (int f = 0; f < 6; f++){
                unsigned char* dst = levelPixels + (size_t)f * size * size * 4;
                const float* src = chain[0].faces[f].data();
                for (int i = 0; i < size * size; i++){
                    dst[i * 4] = linearToSRGB8(src[i * 3]);
                    dst[i * 4 + 1] = linearToSRGB8(src[i * 3 + 1]);
                    dst[i * 4 + 2] = linearToSRGB8(src[i * 3 + 2]);
                    dst[i * 4 + 3] = 255;
                }
            }
            continue;
        }

        float roughness = (float)level / (SPECULAR_MIPS - 1);
        buildGGXSamples(roughness, numSamples, samples);

        // pick a source mip per sample to avoid undersampling (solid angle heuristic)
        for (auto& sample : samples){
            float omegaSample = 1.0f / (numSamples * sample.pdf);
            float lod = 0.5f * log2f(omegaSample / totalOmega);
            sample.lod = std::max(0.0f, std::min(lod, (float)(SPECULAR_MIPS - 1)));
        }

        for (int f = 0; f < 6; f++){
            unsigned char* dst = levelPixels + (size_t)f * size * size * 4;
            for (int y = 0; y < size; y++){
                for (int x = 0; x < size; x++){
                    Vec3f n = faceTexelDir(f, 2.0f * (x + 0.5f) / size - 1.0f, 2.0f * (y + 0.5f) / size - 1.0f);

                    // tangent base around n
                    Vec3f up = (fabsf(n.z) < 0.999f)? Vec3f{0.0f, 0.0f, 1.0f} : Vec3f{1.0f, 0.0f, 0.0f};
                    Vec3f tx = {up.y * n.z - up.z * n.y, up.z * n.x - up.x * n.z, up.x * n.y - up.y * n.x};
                    float txLen = sqrtf(tx.x*tx.x + tx.y*tx.y + tx.z*tx.z);
                    tx.x /= txLen; tx.y /= txLen; tx.z /= txLen;
                    Vec3f ty = {n.y * tx.z - n.z * tx.y, n.z * tx.x - n.x * tx.z, n.x * tx.y - n.y * tx.x};

                    float color[3] = {0.0f, 0.0f, 0.0f};
                    float totalWeight = 0.0f;

                    for (const auto& sample : samples){
                        Vec3f l = {
                            tx.x * sample.l.x + ty.x * sample.l.y + n.x * sample.l.z,
                            tx.y * sample.l.x + ty.y * sample.l.y + n.y * sample.l.z,
                            tx.z * sample.l.x + ty.z * sample.l.y + n.z * sample.l.z
                        };

                        int srcLevel = (int)(sample.lod + 0.5f);
                        float c[3];
                        sampleCube(chain[srcLevel], l, c);

                        color[0] += c[0] * sample.weight;
                        color[1] += c[1] * sample.weight;
                        color[2] += c[2] * sample.weight;
                        totalWeight += sample.weight;
                    }

                    if (totalWeight > 0.0f){
                        color[0] /= totalWeight;
                        color[1] /= totalWeight;
                        color[2] /= totalWeight;
                    }

                    unsigned char* px = &dst[(y * size + x) * 4];
                    px[0] = linearToSRGB8(color[0]);
                    px[1] = linearToSRGB8(color[1]);
                    px[2] = linearToSRGB8(color[2]);
                    px[3] = 255;
                }
            }
        }
    }

    // ---- create GPU textures ----
    bool irrCreated = irradianceMap.createTexture(
            label + "|irradiance", IRRADIANCE_SIZE, IRRADIANCE_SIZE,
            ColorFormat::RGBA, TextureType::TEXTURE_CUBE, 6, irrData, irrSize,
            TextureFilter::LINEAR, TextureFilter::LINEAR, TextureWrap::CLAMP_TO_EDGE, TextureWrap::CLAMP_TO_EDGE);

    bool specCreated = prefilteredMap.createTextureCubeWithMips(
            label + "|prefiltered", SPECULAR_BASE_SIZE,
            ColorFormat::RGBA, SPECULAR_MIPS, specData, specSize,
            TextureFilter::LINEAR_MIPMAP_LINEAR, TextureFilter::LINEAR, TextureWrap::CLAMP_TO_EDGE, TextureWrap::CLAMP_TO_EDGE);

    // must come after texture creation: in the synchronous path scheduleCleanup
    // frees immediately, and the data is only consumed inside sg_make_image
    for (int level = 0; level < SPECULAR_MIPS; level++){
        SystemRender::scheduleCleanup(cleanupBuffer, specData[level]);
    }

    if (!irrCreated || !specCreated){
        Log::error("Cannot create IBL environment maps of %s", label.c_str());
        irradianceMap.destroyTexture();
        prefilteredMap.destroyTexture();
        return false;
    }

    return true;
}
