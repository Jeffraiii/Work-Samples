#include "stdafx.h"
#include "DbgAssert.h"
#include "game.h"
#include "Skeleton.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>


Skeleton::Skeleton(Game* pGame)
    : mGame(pGame)
{
}

void Skeleton::ComputeGlobalInvBindPose()
{
    mGlobalInvBindPoses.resize(mBones.size());
    for (int i = 0; i < mBones.size(); ++i)
    {
        const Bone& bone = mBones[i];
        Matrix4 mat = bone.mLocalBindPose.ToMatrix();
        if (bone.mParent >= 0)
            mGlobalInvBindPoses[i] = mat * mGlobalInvBindPoses[bone.mParent];
    }
    for (int i = 0; i < mBones.size(); ++i)
    {
        mGlobalInvBindPoses[i].Invert();
    }
}

Skeleton* Skeleton::StaticLoad(const WCHAR* fileName, Game* pGame)
{
    Skeleton *pSkeleton = new Skeleton(pGame);
    if (false == pSkeleton->Load(fileName))
    {
        delete pSkeleton;
        return nullptr;
    }
    return pSkeleton;
}

bool Skeleton::Load(const WCHAR* fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        return false;
    }

    std::stringstream fileStream;
    fileStream << file.rdbuf();
    std::string contents = fileStream.str();
    rapidjson::StringStream jsonStr(contents.c_str());
    rapidjson::Document doc;
    doc.ParseStream(jsonStr);

    if (!doc.IsObject())
    {
        return false;
    }

    std::string str = doc["metadata"]["type"].GetString();
    int ver = doc["metadata"]["version"].GetInt();

    // Check the metadata
    if (!doc["metadata"].IsObject() ||
        str != "itpskel" ||
        ver != 1)
    {
        return false;
    }

    const rapidjson::Value& bonecount = doc["bonecount"];
    if (!bonecount.IsUint())
    {
        return false;
    }

    size_t count = bonecount.GetUint();

//    DbgAssert(count <= Skeleton::MAX_SKELETON_BONES, "Skeleton exceeds maximum allowed bones.");

    mBones.reserve(count);

    const rapidjson::Value& bones = doc["bones"];
    if (!bones.IsArray())
    {
        return false;
    }

    if (bones.Size() != count)
    {
        return false;
    }

    Bone temp;

    for (rapidjson::SizeType i = 0; i < count; i++)
    {
        if (!bones[i].IsObject())
        {
            return false;
        }

        const rapidjson::Value& name = bones[i]["name"];
        temp.mName = name.GetString();

        const rapidjson::Value& parent = bones[i]["parent"];
        temp.mParent = parent.GetInt();

        const rapidjson::Value& bindpose = bones[i]["bindpose"];
        if (!bindpose.IsObject())
        {
            return false;
        }

        const rapidjson::Value& rot = bindpose["rot"];
        const rapidjson::Value& trans = bindpose["trans"];

        if (!rot.IsArray() || !trans.IsArray())
        {
            return false;
        }

        temp.mLocalBindPose.mRotation.x = rot[0].GetDouble();
        temp.mLocalBindPose.mRotation.y = rot[1].GetDouble();
        temp.mLocalBindPose.mRotation.z = rot[2].GetDouble();
        temp.mLocalBindPose.mRotation.w = rot[3].GetDouble();

        temp.mLocalBindPose.mTranslation.x = trans[0].GetDouble();
        temp.mLocalBindPose.mTranslation.y = trans[1].GetDouble();
        temp.mLocalBindPose.mTranslation.z = trans[2].GetDouble();

        mBones.push_back(temp);
    }

    // Now that we have the bones
    ComputeGlobalInvBindPose();

    return true;
}

