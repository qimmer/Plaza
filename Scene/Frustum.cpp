
static void UpdateViewMatrix(Entity camera) {
    auto data = GetCamera(camera);

    m4x4f viewMat;
    m4x4f globalMat = GetGlobalTransform(camera);
    glm_mat4_inv((vec4*)&globalMat.x.x, (vec4*)&viewMat.x.x);
    SetCameraViewMatrix(camera, viewMat);
}

static void UpdateInvProjectionMatrix(Entity camera, m4x4f oldValue, m4x4f newValue) {
    auto data = GetCamera(camera);
    m4x4f viewProjMat, invViewProjMat;

    glm_mat4_mul((vec4*)&data->CameraProjectionMatrix, (vec4*)&data->CameraViewMatrix, (vec4*)&viewProjMat);
    glm_mat4_inv((vec4*)&viewProjMat, (vec4*)&invViewProjMat);

    SetCameraInvViewProjectionMatrix(camera, invViewProjMat);
}

LocalFunction(OnGlobalTransformChanged, void, Entity entity, m4x4f oldValue, m4x4f newValue) {
    if(HasComponent(entity, ComponentOf_Camera())) {
        UpdateViewMatrix(entity);
    }
}
LocalFunction(OnCameraAdded, void, Entity component, Entity entity) {
    UpdateViewMatrix(entity);
}

DefineService(Camera)
    RegisterSubscription(GlobalTransformChanged, OnGlobalTransformChanged, 0)
    RegisterSubscription(CameraAdded, OnCameraAdded, 0)
    RegisterSubscription(CameraProjectionMatrixChanged, UpdateInvProjectionMatrix, 0)
    RegisterSubscription(CameraViewMatrixChanged, UpdateInvProjectionMatrix, 0)
EndService()
