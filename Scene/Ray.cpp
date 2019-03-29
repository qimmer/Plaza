//
// Created by Kim on 31-10-2018.
//

#include "Ray.h"
#include <Rendering/Renderable.h>
#include <Rendering/RenderContext.h>
#include <Rendering/SceneRenderer.h>
#include <Input/InputContext.h>
#include <Foundation/AppLoop.h>
#include "Frustum.h"
#include "Transform.h"
#include <cglm/cglm.h>
#include <Rendering/RenderTarget.h>
#include <Foundation/AppNode.h>
#include <Foundation/Visibility.h>

struct EvaluatedRay {
    int sign[3];
    v3f invdir, dir;
    v3f orig;
};

#define NUMDIM    3
#define RIGHT    0
#define LEFT    1
#define MIDDLE    2

char HitBoundingBox(
        float *minB,
        float *maxB,        /*box */
        float *origin,
        float *dir,        /*ray */
        float *coord)                /* hit point */
{
    char inside = true;
    char quadrant[NUMDIM];
    register int i;
    int whichPlane;
    float maxT[NUMDIM];
    float candidatePlane[NUMDIM];

/* Find candidate planes; this loop can be avoided if
   rays cast all from the eye(assume perpsective view) */
    for (
            i = 0;
            i < NUMDIM; i++)
        if (origin[i] < minB[i]) {
            quadrant[i] = LEFT;
            candidatePlane[i] = minB[i];
            inside = false;
        } else if (origin[i] > maxB[i]) {
            quadrant[i] = RIGHT;
            candidatePlane[i] = maxB[i];
            inside = false;
        } else {
            quadrant[i] = MIDDLE;
        }

/* Ray origin inside bounding box */
    if (inside) {
        coord = origin;
        return (true);
    }


/* Calculate T distances to candidate planes */
    for (
            i = 0;
            i < NUMDIM; i++)
        if (quadrant[i] != MIDDLE && dir[i] != 0.)
            maxT[i] = (candidatePlane[i] - origin[i]) / dir[i];
        else
            maxT[i] = -1.;

/* Get largest of the maxT's for final choice of intersection */
    whichPlane = 0;
    for (
            i = 1;
            i < NUMDIM; i++)
        if (maxT[whichPlane] < maxT[i])
            whichPlane = i;

/* Check final candidate actually inside box */
    if (maxT[whichPlane] < 0.) return (false);
    for (
            i = 0;
            i < NUMDIM; i++)
        if (whichPlane != i) {
            coord[i] = origin[i] + maxT[whichPlane] * dir[i];
            if (coord[i] < minB[i] || coord[i] > maxB[i])
                return (false);
        } else {
            coord[i] = candidatePlane[i];
        }
    return (true);                /* ray hits box */
}

static inline bool RayAABBIntersect(EvaluatedRay *r, v3f *bounds) {
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    tmin = (bounds[r->sign[0]].x - r->orig.x) * r->invdir.x;
    tmax = (bounds[1 - r->sign[0]].x - r->orig.x) * r->invdir.x;
    tymin = (bounds[r->sign[1]].y - r->orig.y) * r->invdir.y;
    tymax = (bounds[1 - r->sign[1]].y - r->orig.y) * r->invdir.y;

    if ((tmin > tymax) || (tymin > tmax))
        return false;
    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    tzmin = (bounds[r->sign[2]].z - r->orig.z) * r->invdir.z;
    tzmax = (bounds[1 - r->sign[2]].z - r->orig.z) * r->invdir.z;

    if ((tmin > tzmax) || (tzmin > tmax))
        return false;
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    return true;
}

static inline bool TraceRenderable(Entity renderable, EvaluatedRay *ray, v3f *hitPoint) {
    auto renderableData = GetRenderableData(renderable);
    if(!renderableData) return false;

    return HitBoundingBox(
            &renderableData->RenderableAABBMin.x,
            &renderableData->RenderableAABBMax.x,
            &ray->orig.x,
            &ray->dir.x,
            &hitPoint->x);
}

static inline Entity RayTrace(Entity ray, TraceRay *data) {
	auto transformData = GetTransformData(ray);

    float minDistance = FLT_MAX;
    Entity pickedRenderable = 0;

    int sign[3];
    v3f invdir = {-transformData->TransformGlobalMatrix[2].x, -transformData->TransformGlobalMatrix[2].y, -transformData->TransformGlobalMatrix[2].z};
    sign[0] = (invdir.x < 0);
    sign[1] = (invdir.y < 0);
    sign[2] = (invdir.z < 0);

    EvaluatedRay evaluatedRay = {
            {sign[0], sign[1], sign[2]},
            invdir,
            {transformData->TransformGlobalMatrix[2].x, transformData->TransformGlobalMatrix[2].y, transformData->TransformGlobalMatrix[2].z},
            {transformData->TransformGlobalMatrix[3].x, transformData->TransformGlobalMatrix[3].y, transformData->TransformGlobalMatrix[3].z}
    };

    v3f finalHitPoint = {transformData->TransformGlobalMatrix[3].x, transformData->TransformGlobalMatrix[3].y, transformData->TransformGlobalMatrix[3].z};
    finalHitPoint.x += transformData->TransformGlobalMatrix[2].x;
    finalHitPoint.y += transformData->TransformGlobalMatrix[2].y;
    finalHitPoint.z += transformData->TransformGlobalMatrix[2].z;

    auto scene = GetAppNodeRoot(ray);
    for_entity_abstract(candidate, data2, data->TraceRayComponent) {
        v3f hitPoint;

        if(GetAppNodeRoot(candidate) != scene || GetHierarchiallyHidden(candidate)) continue;

        if (TraceRenderable(candidate, &evaluatedRay, &hitPoint)) {
            auto distance = glm_vec_distance(&hitPoint.x, &evaluatedRay.orig.x);

            if(distance < minDistance) {
                minDistance = distance;
                pickedRenderable = candidate;
                finalHitPoint = hitPoint;
            }
        }
    }

    SetTraceRayPoint(ray, finalHitPoint);

    return pickedRenderable;
}

static void UpdatePickRay(Entity ray, PickRay *data) {
    auto sceneRendererData = GetSceneRendererData(data->PickRaySceneRenderer);
    if(!sceneRendererData) return;

    auto frustumData = GetFrustumData(sceneRendererData->SceneRendererCamera);
    if(!frustumData) return;

    auto mouseLocation = GetInputContextCursorPosition(sceneRendererData->SceneRendererTarget);
    auto renderTargetSize = GetRenderTargetSize(sceneRendererData->SceneRendererTarget);

    v3f location;
    location.x = (float)mouseLocation.x / sceneRendererData->SceneRendererViewport.z;
    location.y = (float)(renderTargetSize.y - mouseLocation.y) / sceneRendererData->SceneRendererViewport.w;

    v3f worldSpaceNear;
    v3f worldSpaceFar;
    v3f rayDirection;

    v4f viewport = {
        sceneRendererData->SceneRendererViewport.x * renderTargetSize.x,
        sceneRendererData->SceneRendererViewport.y * renderTargetSize.y,
        sceneRendererData->SceneRendererViewport.z * renderTargetSize.x,
        sceneRendererData->SceneRendererViewport.w * renderTargetSize.y,
    };

    location.z = 0.0f;
    glm_unprojecti(&location.x, (vec4*)&frustumData->FrustumInvViewProjectionMatrix[0].x, &viewport.x, &worldSpaceNear.x);

    location.z = 1.0f;
    glm_unprojecti(&location.x, (vec4*)&frustumData->FrustumInvViewProjectionMatrix[0].x, &viewport.x, &worldSpaceFar.x);

    glm_vec_sub(&worldSpaceFar.x, &worldSpaceNear.x, &rayDirection.x);

    LookAt(ray, worldSpaceNear, rayDirection);
}

LocalFunction(OnAppUpdate, void, Entity appLoop) {
    for_entity(ray, ComponentOf_PickRay()) {
        UpdatePickRay(ray, data);
    }

    for_entity(ray, data2, TraceRay) {
        SetTraceRayRenderable(ray, RayTrace(ray, data2));
    }
}

BeginUnit(Ray)
    BeginComponent(Ray)
        RegisterBase(Transform)
    EndComponent()

    BeginComponent(TraceRay)
        RegisterBase(Ray)
        RegisterReferenceProperty(Renderable, TraceRayRenderable)
        RegisterProperty(v3f, TraceRayPoint)
        RegisterReferenceProperty(Component, TraceRayComponent)
    EndComponent()

    BeginComponent(DetailedTraceRay)
        RegisterBase(TraceRay)
        RegisterProperty(v3f, DetailedTraceRayPoint)
        RegisterProperty(v3f, DetailedTraceRayNormal)
        RegisterProperty(v3f, DetailedTraceRayPrimitiveIndex)
    EndComponent()

    BeginComponent(PickRay)
        RegisterBase(Ray)
        RegisterReferenceProperty(SceneRenderer, PickRaySceneRenderer)
    EndComponent()

    RegisterSubscription(GetPropertyChangedEvent(PropertyOf_AppLoopFrame()), OnAppUpdate, AppLoopOf_RayPicking())

    SetAppLoopOrder(AppLoopOf_RayPicking(), AppLoopOrder_BoundsUpdate + 1.0f);
EndUnit()