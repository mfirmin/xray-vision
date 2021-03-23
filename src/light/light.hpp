#pragma once

#include <glm/glm.hpp>

typedef struct LightInfo {
    glm::vec4 position; // or direction if w = 0
    glm::vec3 color;
    float intensity;
    float ambientCoefficient;
    float attenuation;
    bool enabled;
    // spotlight only
    float coneAngle;
    glm::vec3 coneDirection;
} LightInfo;

class Light {
    public:
        Light(
            glm::vec3 color,
            float intensity,
            float ambientCoefficient,
            float attenuation
        );

        Light(Light&& other) = default;
        Light& operator=(Light&& other) = default;

        Light(const Light& other) = default;
        Light& operator=(const Light& other) = default;

        void setColor(glm::vec3 c) {
            color = c;
        }

        void setAmbientCoefficient(float ac) {
            ambientCoefficient = ac;
        }

        void setAttenuation(float a) {
            attenuation = a;
        }

        void setIntensity(float i) {
            intensity = i;
        }

        void toggle() {
            enabled = !enabled;
        }

        virtual LightInfo getLightInfo() const = 0;

        virtual ~Light() {};
    protected:
        glm::vec3 color;
        float intensity;
        float ambientCoefficient;
        float attenuation;
        bool enabled = true;
};
