#include "stdafx.h"
#include "BoneTransform.h"

BoneTransform BoneTransform::Interpolate(
    const BoneTransform& a, const BoneTransform& b, float f)
{
    BoneTransform xform;
    xform.mRotation = Slerp(a.mRotation, b.mRotation, f);
    xform.mTranslation = Lerp(a.mTranslation, b.mTranslation, f);
    return xform;
}