Component(Ray)
    Property(v3f, RayOrigin)
    Property(v3f, RayDirection)

Component(PickRay)
    Property(v2f, PickRayViewportOffset)
    ReferenceProperty(Frustum, PickRayFrustum)