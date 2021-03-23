#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

Camera::Camera(
    float aspect,
    float fov,
    float distance,
    glm::vec3 target,
    glm::vec3 rotation,
    glm::vec3 up,
    float near,
    float far
) :
    aspect(aspect),
    fov(fov),
    distance(distance),
    target(target),
    rotation(rotation),
    up(up),
    near(near),
    far(far)
{}

Camera::~Camera() {}

glm::mat4 Camera::getProjectionMatrix() {
    return glm::perspective(glm::radians(fov), aspect, near, far);
}

glm::mat4 Camera::getViewMatrix() {
    auto rot = glm::eulerAngleYX(rotation.y, rotation.x);

    auto position = glm::vec3(0.0f, 0.0f, -distance);
    position = glm::vec3(rot * glm::vec4(position, 1.0f)) + target;

    return glm::lookAt(position, target, up);
}

void Camera::addRotation(glm::vec3 r) {
    rotation += r;

    if (rotation.x > 1.5f) { rotation.x = 1.5f; }
    if (rotation.x < -1.5f) { rotation.x = -1.5f; }

    dirty = true;
}
