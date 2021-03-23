#include "spotLight.hpp"

SpotLight::SpotLight(
    glm::vec3 position,
    float coneAngle,
    glm::vec3 coneDirection,
    glm::vec3 color,
    float intensity,
    float ambientCoefficient,
    float attenuation
) :
    Light(color, intensity, ambientCoefficient, attenuation),
    position(position),
    coneAngle(coneAngle),
    coneDirection(coneDirection)
{}

LightInfo SpotLight::getLightInfo() const {
    return {
        glm::vec4(position, 1.0f),
        color,
        intensity,
        ambientCoefficient,
        attenuation,
        enabled,
        coneAngle,
        coneDirection
    };
}
