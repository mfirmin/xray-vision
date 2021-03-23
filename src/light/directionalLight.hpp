#pragma once

#include "light.hpp"

#include <glm/glm.hpp>

class DirectionalLight : public Light {
    public:
        DirectionalLight(
            glm::vec3 direction,
            glm::vec3 color,
            float intensity,
            float ambientCoefficient
        );

        DirectionalLight(DirectionalLight&& other) = default;
        DirectionalLight& operator=(DirectionalLight&& other) = default;

        DirectionalLight(const DirectionalLight& other) = default;
        DirectionalLight& operator=(const DirectionalLight& other) = default;

        ~DirectionalLight() {};

        LightInfo getLightInfo() const override;
    private:
        glm::vec3 direction;
};
