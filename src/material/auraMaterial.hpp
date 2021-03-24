#pragma once

#include "material.hpp"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class AuraMaterial : public Material {
    public:
        AuraMaterial(glm::vec3 color = glm::vec3(1.0, 1.0, 0.0));
        ~AuraMaterial();

        AuraMaterial(AuraMaterial&& other) = default;
        AuraMaterial(const AuraMaterial& other) = default;

        AuraMaterial& operator=(const AuraMaterial& other) = default;
        AuraMaterial& operator=(AuraMaterial&& other) = default;

        void create() override;

        void setColor(glm::vec3 color) override {};
        void setShininess(float shininess) override {};

        void setLights(const std::vector<std::shared_ptr<Light>>& lights) override {};

        void setEmissiveColorAndStrength(glm::vec3 color, float strength) override {};
        void setEmissiveColor(glm::vec3 color) override {};
        void setEmissiveStrength(float strength) override {};

    private:
        glm::vec3 color;
};
