#pragma once

#include <glm/glm.hpp>

constexpr float DEFAULT_NEAR = 0.01f;
constexpr float DEFAULT_FAR = 100.0f;

// TODO: Orthographic Camera support
class Camera {
    public:
        Camera(
            float aspect,
            float fov,
            float distance,
            glm::vec3 target,
            glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
            float near = DEFAULT_NEAR,
            float far = DEFAULT_FAR
        );

        Camera(Camera&& other) = default;
        Camera(const Camera& other) = default;

        Camera& operator=(const Camera& other) = default;
        Camera& operator=(Camera&& other) = default;

        ~Camera();

        void setRotation(glm::vec3 r) {
            rotation = r;
            dirty = true;
        }

        void setDistance(float d) {
            distance = d;
            dirty = true;
        }

        bool isDirty() {
            return dirty;
        }

        void setDirty(bool d) {
            dirty = d;
        }

        void addRotation(glm::vec3 r);

        glm::mat4 getProjectionMatrix();
        glm::mat4 getViewMatrix();
    private:
        float aspect;
        float fov;

        float distance;
        glm::vec3 target;
        glm::vec3 rotation;
        glm::vec3 up;

        float near;
        float far;

        bool dirty = true;
};
