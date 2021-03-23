#include "light.hpp"

Light::Light(
    glm::vec3 color,
    float intensity,
    float ambientCoefficient,
    float attenuation
) :
    color(color),
    intensity(intensity),
    ambientCoefficient(ambientCoefficient),
    attenuation(attenuation)
{}
