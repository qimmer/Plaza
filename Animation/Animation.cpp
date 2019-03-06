//
// Created by Kim on 11-10-2018.
//

#include <Core/NativeUtils.h>
#include <Core/Algorithms.h>
#include "Animation.h"

#include <cglm/cglm.h>

struct AnimationFrame {
    Variant AnimationFrameValue;
    double AnimationFrameDuration;
};

struct AnimationTrack {
    Entity AnimationTrackProperty;
};

struct Animation {
};

#define INTERP_NUM(TYPE) \
    case TypeOf_ ## TYPE :\
    {\
       auto result = (TYPE)(((double)left.as_ ## TYPE * (1.0 - t)) + ((double)right.as_ ## TYPE * t));\
        return MakeVariant(TYPE, result);\
        break;\
    }


#define INTERP_VEC2(TYPE, ELEMTYPE) \
    case TypeOf_ ## TYPE :\
        TYPE TYPE ## value;\
        TYPE ## value.x = (ELEMTYPE)(((double)left.as_ ## TYPE.x * (1.0 - t)) + ((double)right.as_ ## TYPE.x * t));\
        TYPE ## value.y = (ELEMTYPE)(((double)left.as_ ## TYPE.y * (1.0 - t)) + ((double)right.as_ ## TYPE.y * t));\
        return MakeVariant(TYPE, TYPE ## value);\
        break;

#define INTERP_VEC3(TYPE, ELEMTYPE) \
    case TypeOf_ ## TYPE :\
        TYPE TYPE ## value;\
        TYPE ## value.x = (ELEMTYPE)(((double)left.as_ ## TYPE.x * (1.0 - t)) + ((double)right.as_ ## TYPE.x * t));\
        TYPE ## value.y = (ELEMTYPE)(((double)left.as_ ## TYPE.y * (1.0 - t)) + ((double)right.as_ ## TYPE.y * t));\
        TYPE ## value.z = (ELEMTYPE)(((double)left.as_ ## TYPE.z * (1.0 - t)) + ((double)right.as_ ## TYPE.z * t));\
        return MakeVariant(TYPE, TYPE ## value);\
        break;

#define INTERP_VEC4(TYPE, ELEMTYPE) \
    case TypeOf_ ## TYPE :\
        TYPE TYPE ## value;\
        TYPE ## value.x = (ELEMTYPE)(((double)left.as_ ## TYPE.x * (1.0 - t)) + ((double)right.as_ ## TYPE.x * t));\
        TYPE ## value.y = (ELEMTYPE)(((double)left.as_ ## TYPE.y * (1.0 - t)) + ((double)right.as_ ## TYPE.y * t));\
        TYPE ## value.z = (ELEMTYPE)(((double)left.as_ ## TYPE.z * (1.0 - t)) + ((double)right.as_ ## TYPE.z * t));\
        TYPE ## value.w = (ELEMTYPE)(((double)left.as_ ## TYPE.w * (1.0 - t)) + ((double)right.as_ ## TYPE.w * t));\
        return MakeVariant(TYPE, TYPE ## value);\
        break;

API_EXPORT Variant Interpolate(Variant left, Variant right, float t) {
    if(left.type != right.type) {
        Log(0, LogSeverity_Error, "Interpolation values have different types.");
        return Variant_Empty;
    }

    switch(left.type) {
        INTERP_VEC2(v2f, float)
        INTERP_VEC2(v2i, int)
        INTERP_VEC3(v3f, float)
        INTERP_VEC3(v3i, int)
        INTERP_VEC3(rgb8, u8)
        INTERP_VEC3(rgb32, float)
        INTERP_VEC4(v4f, float)
        INTERP_VEC4(v4i, int)
        INTERP_VEC4(rgba8, u8)
        INTERP_VEC4(rgba32, float)
        INTERP_NUM(float)
        INTERP_NUM(double)
        INTERP_NUM(u8)
        INTERP_NUM(u16)
        INTERP_NUM(u32)
        INTERP_NUM(u64)
        INTERP_NUM(s8)
        INTERP_NUM(s16)
        INTERP_NUM(s32)
        INTERP_NUM(s64)
        default:
            return left;
            break;
    }

    Log(0, LogSeverity_Error, "Cannot interpolate type: %s", GetTypeName(left.type));
    return Variant_Empty;
}

API_EXPORT Variant EvaluateAnimationFrame(Entity animationTrack, double time, bool loop) {
    auto data = GetAnimationTrackData(animationTrack);
    if(!data) {
        return Variant_Empty;
    }

    u32 numFrames = 0;
    auto frames = GetAnimationTrackFrames(animationTrack, &numFrames);
    if(!numFrames) {
        return Variant_Empty;
    }

    Entity firstFrame = 0, lastFrame = 0;
    double firstTime, lastTime;

    float t = 0.0f;
    double duration = 0.0, sum = 0.0;

    // Find accumulated duration
    for_children(frame, AnimationTrackFrames, animationTrack) {
        auto frameData = GetAnimationFrameData(frame);
        if(!frameData) continue;

        duration += frameData->AnimationFrameDuration;

        lastFrame = frame;
    }

    if(loop) {
        time = fmod(time, duration);
    } else {
        time = Min(Max(time, 0.0), duration);
    }

    // Find first frame
    for_children(frame2, AnimationTrackFrames, animationTrack) {
        auto frameData = GetAnimationFrameData(frame2);
        if(!frameData) continue;

        auto startTime = sum;
        sum += frameData->AnimationFrameDuration;

        if(!firstFrame && time >= startTime && time < sum) {
            firstFrame = frame2;
            firstTime = startTime;
            lastFrame = _entriesframe2[(_iframe2 + 1) % _countframe2];
            lastTime = sum;
            break;
        }
    }

    if(!firstFrame || !lastFrame) return Variant_Empty;

    t = (time - firstTime) / (lastTime - firstTime);

    auto firstValue = GetAnimationFrameValue(firstFrame);
    auto lastValue = GetAnimationFrameValue(lastFrame);

    auto property = data->AnimationTrackProperty;
    auto propertyType = GetPropertyType(property);
    if(firstValue.type != propertyType) {
        Log(animationTrack, LogSeverity_Error, "Animation frame values are of type %s, but property is of type %s.", GetTypeName(firstValue.type), GetTypeName(propertyType));
        return Variant_Empty;
    }

    auto interpValue = Interpolate(firstValue, lastValue, t);
    return interpValue;
}

BeginUnit(Animation)
    BeginComponent(AnimationFrame)
        RegisterProperty(double, AnimationFrameDuration)
        RegisterProperty(Variant, AnimationFrameValue)
    EndComponent()

    BeginComponent(AnimationTrack)
        RegisterArrayProperty(AnimationFrame, AnimationTrackFrames)
        RegisterReferenceProperty(Property, AnimationTrackProperty)
    EndComponent()

    BeginComponent(Animation)
        RegisterArrayProperty(AnimationTrack, AnimationTracks)
    EndComponent()
EndUnit()
