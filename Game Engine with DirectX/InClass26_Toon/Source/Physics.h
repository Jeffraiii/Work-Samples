#pragma once
#include "engineMath.h"

class CollisionComponent;

class Physics
{
public:
    class AABB
    {
    public:
        AABB()
            : mMin(Vector3::Zero)
            , mMax(Vector3::Zero)
        {}
        AABB(const Vector3& min, const Vector3& max)
            : mMin(min)
            , mMax(max)
        {}
        Vector3 mMin;
        Vector3 mMax;
    };

    class Ray
    {
    public:
        Ray()
            : mFrom(Vector3::Zero)
            , mTo(Vector3::Zero)
        {}
        Ray(const Vector3& from, const Vector3& to)
            : mFrom(from)
            , mTo(to)
        {}
        Vector3 mFrom;
        Vector3 mTo;
    };

    static bool Intersect(const AABB& a, const AABB& b, AABB* pOverlap = nullptr);
    static bool Intersect(const Ray& ray, const AABB& box, Vector3* pHitPoint = nullptr);
    static bool UnitTest();

    void AddObj(CollisionComponent* pObj);
    void RemoveObj(CollisionComponent* pObj);
    bool RayCast(const Ray& ray, Vector3* pHitPoint = nullptr);

private:
    std::vector<CollisionComponent*> mObj;
};