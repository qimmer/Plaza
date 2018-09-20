
API_EXPORT v3f GetCameraPickRayPoint(Entity camera, v2f viewportOffset, float d) {
    auto data = GetCamera(camera);

    float mouseXNDC = viewportOffset.x * 2.0f - 1.0f;
    float mouseYNDC = (1.0f - viewportOffset.y) * 2.0f - 1.0f;
    float mousePosNDC[4] = { mouseXNDC, mouseYNDC, d, 1.0f };

    float point[4];
    glm_mat4_mulv((vec4*)&data->CameraInvViewProjectionMatrix, mousePosNDC, point);

    return {point[0] / point[3], point[1] / point[3], point[2] / point[3]};
}
