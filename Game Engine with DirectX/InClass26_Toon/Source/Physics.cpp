#include "stdafx.h"
#include "Physics.h"
#include "Components\CollisionComponent.h"


bool Physics::Intersect(const AABB& a, const AABB& b, AABB* pOverlap)
{
    AABB overlap;
    overlap.mMin.x = max(a.mMin.x, b.mMin.x);
    overlap.mMin.y = max(a.mMin.y, b.mMin.y);
    overlap.mMin.z = max(a.mMin.z, b.mMin.z);
    overlap.mMax.x = min(a.mMax.x, b.mMax.x);
    overlap.mMax.y = min(a.mMax.y, b.mMax.y);
    overlap.mMax.z = min(a.mMax.z, b.mMax.z);
    if ((overlap.mMax.x >= overlap.mMin.x)
        && (overlap.mMax.y >= overlap.mMin.y)
        && (overlap.mMax.z >= overlap.mMin.z)
        )
    {
        if (nullptr != pOverlap)
            *pOverlap = overlap;
        return true;
    }
    return false;
}

bool Physics::Intersect(const Ray& ray, const AABB& box, Vector3* pHitPoint)
{
    static const float s_closeEnuf = 0.001f;
    Vector3 d = ray.mTo - ray.mFrom;
    float tmin = -FLT_MAX; // set to -FLT_MAX to get first hit on line
    float tmax = FLT_MAX; // set to max distance ray can travel (for segment)

                          // x axis
    if (fabsf(d.x) < s_closeEnuf)
    {
        // Ray is parallel to slab. No hit if origin not within slab
        if (ray.mFrom.x < box.mMin.x || ray.mFrom.x > box.mMax.x)
            return false;
    }
    else
    {
        float ood = 1.0f / d.x;
        float t1 = (box.mMin.x - ray.mFrom.x) * ood;
        float t2 = (box.mMax.x - ray.mFrom.x) * ood;
        // Make t1 be intersection with near plane, t2 with far plane
        if (t1 > t2)
        {
            float temp = t2;
            t2 = t1;
            t1 = temp;
        }
        // Compute the intersection of slab intersection intervals
        tmin = max(tmin, t1); // Rather than: if (t1 > tmin) tmin = t1;
        tmax = min(tmax, t2); // Rather than: if (t2 < tmax) tmax = t2;
                              // Exit with no collision as soon as slab intersection becomes empty
        if (tmin > tmax)
            return false;
    }

    // y axis
    if (fabsf(d.y) < s_closeEnuf)
    {
        // Ray is parallel to slab. No hit if origin not within slab
        if (ray.mFrom.y < box.mMin.y || ray.mFrom.y > box.mMax.y)
            return false;
    }
    else
    {
        float ood = 1.0f / d.y;
        float t1 = (box.mMin.y - ray.mFrom.y) * ood;
        float t2 = (box.mMax.y - ray.mFrom.y) * ood;
        // Make t1 be intersection with near plane, t2 with far plane
        if (t1 > t2)
        {
            float temp = t2;
            t2 = t1;
            t1 = temp;
        }
        // Compute the intersection of slab intersection intervals
        tmin = max(tmin, t1); // Rather than: if (t1 > tmin) tmin = t1;
        tmax = min(tmax, t2); // Rather than: if (t2 < tmax) tmax = t2;
                              // Exit with no collision as soon as slab intersection becomes empty
        if (tmin > tmax)
            return false;
    }

    // z axis
    if (fabsf(d.z) < s_closeEnuf)
    {
        // Ray is parallel to slab. No hit if origin not within slab
        if (ray.mFrom.z < box.mMin.z || ray.mFrom.z > box.mMax.z)
            return false;
    }
    else
    {
        float ood = 1.0f / d.z;
        float t1 = (box.mMin.z - ray.mFrom.z) * ood;
        float t2 = (box.mMax.z - ray.mFrom.z) * ood;
        // Make t1 be intersection with near plane, t2 with far plane
        if (t1 > t2)
        {
            float temp = t2;
            t2 = t1;
            t1 = temp;
        }
        // Compute the intersection of slab intersection intervals
        tmin = max(tmin, t1); // Rather than: if (t1 > tmin) tmin = t1;
        tmax = min(tmax, t2); // Rather than: if (t2 < tmax) tmax = t2;
                              // Exit with no collision as soon as slab intersection becomes empty
        if (tmin > tmax)
            return false;
    }

    if (tmin > 1.0f || tmax < 0.0f)
        return false;

    // Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
    if (nullptr != pHitPoint)
        *pHitPoint = ray.mFrom + d * tmin;

    return true;
}

bool Physics::UnitTest()
{
    struct TestAABB
    {
        AABB    a;
        AABB    b;
        AABB    overlap;
    };
    const TestAABB testAABB[] =
    {
        { 
            AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f)),
            AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f))
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-110.0f, -10.0f, -10.0f), Vector3(-90.0f, 10.0f, 10.0f)),
            AABB(Vector3(-100.0f, -10.0f, -10.0f), Vector3(-90.0f, 10.0f, 10.0f))
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(90.0f, -10.0f, -10.0f), Vector3(110.0f, 10.0f, 10.0f)),
            AABB(Vector3(90.0f, -10.0f, -10.0f), Vector3(100.0f, 10.0f, 10.0f))
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, -110.0f, -10.0f), Vector3(10.0f, -90.0f, 10.0f)),
            AABB(Vector3(-10.0f, -100.0f, -10.0f), Vector3(10.0f, -90.0f, 10.0f))
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, 90.0f, -10.0f), Vector3(10.0f, 110.0f, 10.0f)),
            AABB(Vector3(-10.0f, 90.0f, -10.0f), Vector3(10.0f, 100.0f, 10.0f))
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, -10.0f, -110.0f), Vector3(10.0f, 10.0f, -90.0f)),
            AABB(Vector3(-10.0f, -10.0f, -100.0f), Vector3(10.0f, 10.0f, -90.0f))
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, -10.0f, 90.0f), Vector3(10.0f, 10.0f, 110.0f)),
            AABB(Vector3(-10.0f, -10.0f, 90.0f), Vector3(10.0f, 10.0f, 100.0f))
        },

        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-120.0f, -10.0f, -10.0f), Vector3(-110.0f, 10.0f, 10.0f)),
            AABB(Vector3::One, Vector3::Zero)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(110.0f, -10.0f, -10.0f), Vector3(120.0f, 10.0f, 10.0f)),
            AABB(Vector3::One, Vector3::Zero)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, -120.0f, -10.0f), Vector3(10.0f, -110.0f, 10.0f)),
            AABB(Vector3::One, Vector3::Zero)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, 110.0f, -10.0f), Vector3(10.0f, 120.0f, 10.0f)),
            AABB(Vector3::One, Vector3::Zero)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, -10.0f, -120.0f), Vector3(10.0f, -10.0f, -110.0f)),
            AABB(Vector3::One, Vector3::Zero)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            AABB(Vector3(-10.0f, -10.0f, 110.0f), Vector3(10.0f, 10.0f, 120.0f)),
            AABB(Vector3::One, Vector3::Zero)
        },
    };

    bool isOk = true;
    int numAABB = sizeof(testAABB) / sizeof(testAABB[0]);
    for (int i = 0; i < numAABB; ++i)
    {
        AABB overlap;
        bool intersects = Intersect(testAABB[i].a, testAABB[i].b, &overlap);
        Vector3 delta = testAABB[i].overlap.mMax - testAABB[i].overlap.mMin;
        bool shouldIntersect = delta.x >= 0.0f;
        isOk &= intersects == shouldIntersect;
        if (intersects)
        {
            delta = testAABB[i].overlap.mMax - overlap.mMax;
            isOk &= Math::IsCloseEnuf(delta.x, 0.0f);
            isOk &= Math::IsCloseEnuf(delta.y, 0.0f);
            isOk &= Math::IsCloseEnuf(delta.z, 0.0f);
            delta = testAABB[i].overlap.mMin - overlap.mMin;
            isOk &= Math::IsCloseEnuf(delta.x, 0.0f);
            isOk &= Math::IsCloseEnuf(delta.y, 0.0f);
            isOk &= Math::IsCloseEnuf(delta.z, 0.0f);
        }
    }

    struct TestRay
    {
        AABB    box;
        Ray     ray;
        bool    hit;
        Vector3 point;
    };
    const TestRay testRay[] =
    {
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            Ray(Vector3(-110.0f, 0.0f, 0.0f), Vector3(-90.0f, 0.0f, 0.0f)),
            true,   Vector3(-100.0f, 0.0f, 0.0f)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            Ray(Vector3(0.0f, -110.0f, 0.0f), Vector3(0.0f, -90.0f, 0.0f)),
            true,   Vector3(0.0f, -100.0f, 0.0f)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            Ray(Vector3(0.0f, 0.0f, -110.0f), Vector3(0.0f, 0.0f, -90.0f)),
            true,   Vector3(0.0f, 0.0f, -100.0f)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            Ray(Vector3(110.0f, 0.0f, 0.0f), Vector3(90.0f, 0.0f, 0.0f)),
            true,   Vector3(100.0f, 0.0f, 0.0f)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            Ray(Vector3(0.0f, 110.0f, 0.0f), Vector3(0.0f, 90.0f, 0.0f)),
            true,   Vector3(0.0f, 100.0f, 0.0f)
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            Ray(Vector3(0.0f, 0.0f, 110.0f), Vector3(0.0f, 0.0f, 90.0f)),
            true,   Vector3(0.0f, 0.0f, 100.0f)
        },

        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            Ray(Vector3(-120.0f, 0.0f, 0.0f), Vector3(-110.0f, 0.0f, 0.0f)),
            false,  Vector3::Zero
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            Ray(Vector3(0.0f, -120.0f, 0.0f), Vector3(0.0f, -110.0f, 0.0f)),
            false,  Vector3::Zero
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            Ray(Vector3(0.0f, 0.0f, -120.0f), Vector3(0.0f, 0.0f, -110.0f)),
            false,  Vector3::Zero
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            Ray(Vector3(120.0f, 0.0f, 0.0f), Vector3(110.0f, 0.0f, 0.0f)),
            false,  Vector3::Zero
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            Ray(Vector3(0.0f, 120.0f, 0.0f), Vector3(0.0f, 110.0f, 0.0f)),
            false,  Vector3::Zero
        },
        {
            AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
            Ray(Vector3(0.0f, 0.0f, 120.0f), Vector3(0.0f, 0.0f, 110.0f)),
            false,  Vector3::Zero
        },
    };

    int numRay = sizeof(testRay) / sizeof(testRay[0]);
    for (int i = 0; i < numRay; ++i)
    {
        Vector3 hitPoint;
        bool intersects = Intersect(testRay[i].ray, testRay[i].box, &hitPoint);
        isOk &= intersects == testRay[i].hit;
        if (intersects)
        {
            Vector3 delta = testRay[i].point - hitPoint;
            isOk &= Math::IsCloseEnuf(delta.x, 0.0f);
            isOk &= Math::IsCloseEnuf(delta.y, 0.0f);
            isOk &= Math::IsCloseEnuf(delta.z, 0.0f);
        }
    }

    return isOk;
}

void Physics::AddObj(CollisionComponent* pObj)
{
    mObj.push_back(pObj);
}

void Physics::RemoveObj(CollisionComponent* pObj)
{
    for (auto it = mObj.begin(); it != mObj.end(); ++it)
    {
        CollisionComponent* pFind = *it;
        if (pFind == pObj)
        {
            mObj.erase(it);
            return;
        }
    }
}

bool Physics::RayCast(const Ray& ray, Vector3* pHitPoint)
{
    bool hit = false;
    float bestDist = 1e20f;
    for (const auto pObj : mObj)
    {
        Vector3 point;
        if (Intersect(ray, pObj->GetAABB(), &point))
        {
            hit = true;
            float distSq = (point - ray.mFrom).LengthSq();
            if (distSq < bestDist)
            {
                bestDist = distSq;
                *pHitPoint = point;
            }
        }
    }
    return hit;
}
