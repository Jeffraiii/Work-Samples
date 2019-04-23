#pragma once
#include "engineMath.h"
#include <vector>
class CollisionComponent;
class Physics
{
public:
	Physics();
	~Physics();
	class AABB
	{
	public:
		AABB() {};
		AABB(Vector3 min, Vector3 max) {
			mMin = min;
			mMax = max;
		}
		~AABB() {};
		Vector3 mMin;
		Vector3 mMax;
	private:

	};

	class Ray
	{
	public:
		Ray() {};
		~Ray() {};
		Ray(Vector3 from, Vector3 to) {
			mFrom = from;
			mTo = to;
		}
		Vector3 mFrom;
		Vector3 mTo;
	private:

	};

	static bool Intersect(const AABB& a, const AABB& b,
		AABB* pOverlap = nullptr);
	static bool Intersect(const Ray& ray, const AABB& box,
		Vector3* pHitPoint = nullptr);
	static bool UnitTest();
	std::vector<CollisionComponent*> mObj;
	void AddObj(CollisionComponent* pObj);
	void RemoveObj(CollisionComponent* pObj);
	bool RayCast(const Ray& ray, Vector3* pHitPoint = nullptr);
};

