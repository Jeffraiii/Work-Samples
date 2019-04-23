#include "stdafx.h"
#include "Physics.h"
#include "Components/CollisionComponent.h"

Physics::Physics()
{
}


Physics::~Physics()
{
}

bool Physics::Intersect(const AABB & a, const AABB & b, AABB * pOverlap)
{
	bool intersect = (a.mMin.x <= b.mMax.x && a.mMax.x >= b.mMin.x) &&
		(a.mMin.y <= b.mMax.y && a.mMax.y >= b.mMin.y) &&
		(a.mMin.z <= b.mMax.z && a.mMax.z >= b.mMin.z);
	if (intersect) {
		pOverlap->mMin = Vector3(max(a.mMin.x, b.mMin.x), max(a.mMin.y, b.mMin.y), max(a.mMin.z, b.mMin.z));
		pOverlap->mMax = Vector3(min(a.mMax.x, b.mMax.x), min(a.mMax.y, b.mMax.y), min(a.mMax.z, b.mMax.z));
	}
	else {
		pOverlap->mMin = Vector3::One;
		pOverlap->mMax = Vector3::Zero;
	}
	return intersect;
}

bool Physics::Intersect(const Ray & ray, const AABB & box, Vector3 * pHitPoint)
{
	float tmin = 0.0f;
	float tmax = FLT_MAX;
	Vector3 d = ray.mTo - ray.mFrom;
	{
		if (abs(d.x) < DBL_EPSILON) {
			if (ray.mFrom.x < box.mMin.x || ray.mFrom.x > box.mMax.x) return false;
		}
		else {
			float ood = 1.0f / d.x;
			float t1 = (box.mMin.x - ray.mFrom.x) * ood;
			float t2 = (box.mMax.x - ray.mFrom.x) * ood;
			if (t1 > t2) {
				float temp = t1;
				t1 = t2;
				t2 = temp;
			}
			tmin = max(tmin, t1); // Rather than: if (t1 > tmin) tmin = t1;
			tmax = min(tmax, t2); // Rather than: if (t2 < tmax) tmax = t2;
			if (tmin > tmax) return false;
			if (d.x < 0) {
				if (ray.mTo.x > box.mMax.x) return false;
			}
			else if (d.x > 0) {
				if (ray.mTo.x < box.mMin.x) return false;
			}
		}
	}
	{
		if (abs(d.y) < DBL_EPSILON) {
			if (ray.mFrom.y < box.mMin.y || ray.mFrom.y > box.mMax.y) return false;
		}
		else {
			float ood = 1.0f / d.y;
			float t1 = (box.mMin.y - ray.mFrom.y) * ood;
			float t2 = (box.mMax.y - ray.mFrom.y) * ood;
			if (t1 > t2) {
				float temp = t1;
				t1 = t2;
				t2 = temp;
			}
			tmin = max(tmin, t1); // Rather than: if (t1 > tmin) tmin = t1;
			tmax = min(tmax, t2); // Rather than: if (t2 < tmax) tmax = t2;
			if (tmin > tmax) return false;
			if (d.y < 0) {
				if (ray.mTo.y > box.mMax.y) return false;
			}
			else if (d.y > 0) {
				if (ray.mTo.y < box.mMin.y) return false;
			}
		}
	}
	{
		if (abs(d.z) < DBL_EPSILON) {
			if (ray.mFrom.z < box.mMin.z || ray.mFrom.z > box.mMax.z) return false;
		}
		else {
			float ood = 1.0f / d.z;
			float t1 = (box.mMin.z - ray.mFrom.z) * ood;
			float t2 = (box.mMax.z - ray.mFrom.z) * ood;
			if (t1 > t2) {
				float temp = t1;
				t1 = t2;
				t2 = temp;
			}
			tmin = max(tmin, t1); // Rather than: if (t1 > tmin) tmin = t1;
			tmax = min(tmax, t2); // Rather than: if (t2 < tmax) tmax = t2;
			if (tmin > tmax) return false;
			if (d.z < 0) {
				if (ray.mTo.z > box.mMax.z) return false;
			}
			else if (d.z > 0) {
				if (ray.mTo.z < box.mMin.z) return false;
			}
		}
	}
	*pHitPoint = ray.mFrom + d * tmin;
	return true;
}

bool Physics::UnitTest()
{
	struct TestAABB
	{
		AABB a;
		AABB b;
		AABB overlap;
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
	for (TestAABB t : testAABB) {
		AABB* ab = new AABB();
		Intersect(t.a, t.b, ab);
		bool res = (ab->mMax.x == t.overlap.mMax.x) && (ab->mMax.y == t.overlap.mMax.y) &&
			(ab->mMax.z == t.overlap.mMax.z) && (ab->mMin.x == t.overlap.mMin.x) && (ab->mMin.y == t.overlap.mMin.y)
			&& (ab->mMin.z == t.overlap.mMin.z);

		if (!res) {
			return false;
		}
	}
	struct TestRay
	{
		AABB box;
		Ray ray;
		bool hit;
		Vector3 point;
	};
	const TestRay testRay[] =
	{
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	Ray(Vector3(-110.0f, 0.0f, 0.0f), Vector3(-90.0f, 0.0f, 0.0f)),
	true, Vector3(-100.0f, 0.0f, 0.0f)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	Ray(Vector3(0.0f, -110.0f, 0.0f), Vector3(0.0f, -90.0f, 0.0f)),
	true, Vector3(0.0f, -100.0f, 0.0f)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	Ray(Vector3(0.0f, 0.0f, -110.0f), Vector3(0.0f, 0.0f, -90.0f)),
	true, Vector3(0.0f, 0.0f, -100.0f)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	Ray(Vector3(110.0f, 0.0f, 0.0f), Vector3(90.0f, 0.0f, 0.0f)),
	true, Vector3(100.0f, 0.0f, 0.0f)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	Ray(Vector3(0.0f, 110.0f, 0.0f), Vector3(0.0f, 90.0f, 0.0f)),
	true, Vector3(0.0f, 100.0f, 0.0f)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	Ray(Vector3(0.0f, 0.0f, 110.0f), Vector3(0.0f, 0.0f, 90.0f)),
	true, Vector3(0.0f, 0.0f, 100.0f)
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	Ray(Vector3(-120.0f, 0.0f, 0.0f), Vector3(-110.0f, 0.0f, 0.0f)),
	false, Vector3::Zero
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	Ray(Vector3(0.0f, -120.0f, 0.0f), Vector3(0.0f, -110.0f, 0.0f)),
	false, Vector3::Zero
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	Ray(Vector3(0.0f, 0.0f, -120.0f), Vector3(0.0f, 0.0f, -110.0f)),
	false, Vector3::Zero
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	Ray(Vector3(120.0f, 0.0f, 0.0f), Vector3(110.0f, 0.0f, 0.0f)),
	false, Vector3::Zero
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	Ray(Vector3(0.0f, 120.0f, 0.0f), Vector3(0.0f, 110.0f, 0.0f)),
	false, Vector3::Zero
	},
	{
	AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
	Ray(Vector3(0.0f, 0.0f, 120.0f), Vector3(0.0f, 0.0f, 110.0f)),
	false, Vector3::Zero
	},
	};
	for (TestRay t : testRay) {
		Vector3* p = new Vector3();
		bool b = Intersect(t.ray, t.box, p);
		if (b != t.hit) return false;
		bool res = (p->x == t.point.x) && (p->y == t.point.y) && (p->z == t.point.z);
		if (!res) return false;
	}
	return true;
}

void Physics::AddObj(CollisionComponent * pObj)
{
	mObj.push_back(pObj);
}

void Physics::RemoveObj(CollisionComponent * pObj)
{
	std::vector<CollisionComponent*>::iterator it = std::find(mObj.begin(), mObj.end(), pObj);
	if (it != mObj.end()) {
		mObj.erase(it);
	}
}

bool Physics::RayCast(const Ray & ray, Vector3 * pHitPoint)
{
	Vector3 closest;
	int count = 0;
	bool collide = false;
	for (CollisionComponent* c : mObj) {
		if (Intersect(ray, c->GetAABB(), pHitPoint)) {
			collide = true;
			if (count == 0) {
				closest = *pHitPoint;
			}
			else {
				if ((ray.mFrom - *pHitPoint).Length() < (ray.mFrom - closest).Length()) {
					closest = *pHitPoint;
				}
			}
		}
		count++;
	}
	*pHitPoint = closest;
	return collide;
}
