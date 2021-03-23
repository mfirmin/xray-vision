#pragma once

#include "light.hpp"

#include <glm/glm.hpp>

class SpotLight : public Light {
    public:
        SpotLight(
            glm::vec3 position,
            float coneAngle,
            glm::vec3 coneDirection,
            glm::vec3 color,
            float intensity,
            float ambientCoefficient,
            float attenuation
        );

        SpotLight(SpotLight&& other) = default;
        SpotLight& operator=(SpotLight&& other) = default;

        SpotLight(const SpotLight& other) = default;
        SpotLight& operator=(const SpotLight& other) = default;

        ~SpotLight() {};

        LightInfo getLightInfo() const override;
    private:
        glm::vec3 position;
        float coneAngle;
        glm::vec3 coneDirection;
};
