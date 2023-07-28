// AABB_transform.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <array>
#include <random>
#include <iostream>

#define GLM_FORCE_XYZW_ONLY
#include <glm.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat3x4;

inline std::array<vec3, 8> TransformBBPoints(const mat3x4& transform, const vec3& maxPoint, const vec3& minPoint)
{
    const vec3 size = maxPoint - minPoint;

    auto basisVector = [&](int i)
        {
            return vec3(transform[0][i], transform[1][i], transform[2][i]) * size[i];
        };

    vec3 basis[3] = { basisVector(0), basisVector(1), basisVector(2) };
    vec3 out[4];

    out[0] = vec4(minPoint, 1.f) * transform;
    out[1] = out[0] + basis[0];
    out[2] = out[0] + basis[1];
    out[3] = out[1] + basis[1];

    return { out[0], out[1], out[2], out[3],
             out[0] + basis[2], out[1] + basis[2], out[2] + basis[2], out[3] + basis[2] };
}

std::array<vec3, 8> GetBBPoints(const vec3& maxPoint, const vec3& minPoint)
{
    return { minPoint,
             vec3(maxPoint.x, minPoint.y, minPoint.z),
             vec3(minPoint.x, maxPoint.y, minPoint.z),
             vec3(maxPoint.x, maxPoint.y, minPoint.z),
             vec3(minPoint.x, minPoint.y, maxPoint.z),
             vec3(maxPoint.x, minPoint.y, maxPoint.z),
             vec3(minPoint.x, maxPoint.y, maxPoint.z),
             maxPoint
    };
}

void test(const mat3x4& transform, const vec3& maxPoint, const vec3& minPoint)
{
    auto outPoints = TransformBBPoints(transform, maxPoint, minPoint);
    auto pts = GetBBPoints(maxPoint, minPoint);

    auto epsEqual = [](float a, float b)
        {
            constexpr float eps = FLT_EPSILON * 8;
            return fabsf(a - b) <= eps * (fabsf(a) + fabsf(b));
        };

    for (size_t i = 0; i < 8; ++i)
    {
        const auto& oPoint = outPoints[i];
        const auto& tPoint = vec4(pts[i], 1.f) * transform;

        if (!(epsEqual(oPoint.x, tPoint.x) && epsEqual(oPoint.y, tPoint.y) && epsEqual(oPoint.z, tPoint.z)))
        {
            std::cout << "Error detected!\n";
            abort();
        }
    }
}

int main()
{
    std::random_device rd;   // non-deterministic generator
    std::mt19937 gen(rd());  // to seed mersenne twister.

    auto igen = [&]() { return gen() - (gen.max() / 2); };

    for (size_t i = 0; i < 10000; ++i)
    {
        const mat3x4 xfrm(igen(), igen(), igen(), igen(),
                          igen(), igen(), igen(), igen(),
                          igen(), igen(), igen(), igen());
        const vec3 minPoint(igen(), igen(), igen());

        test(xfrm, minPoint + vec3(gen(), gen(), gen()), minPoint);
    }
}
