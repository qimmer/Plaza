//
// Created by Kim on 11-10-2018.
//

#ifndef PLAZA_ANIMATION_H
#define PLAZA_ANIMATION_H

#include <Core/NativeUtils.h>

Unit(Animation)
    Component(AnimationFrame)
        Property(double, AnimationFrameDuration)
        Property(Variant, AnimationFrameValue)

    Component(AnimationTrack)
        ArrayProperty(AnimationFrame, AnimationTrackFrames)
        ReferenceProperty(Property, AnimationTrackProperty)

    Component(Animation)
        ArrayProperty(AnimationTrack, AnimationTracks)

    Function(EvaluateAnimationFrame, Variant, Entity animationTrack, double time, bool loop)

#endif //PLAZA_ANIMATION_H
