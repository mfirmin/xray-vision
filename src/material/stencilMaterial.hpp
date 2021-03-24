#pragma once

#include "material.hpp"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class StencilMaterial : public Material {
    public:
        StencilMaterial();
        ~StencilMaterial();

        StencilMaterial(StencilMaterial&& other) = default;
        StencilMaterial(const StencilMaterial& other) = default;

        StencilMaterial& operator=(const StencilMaterial& other) = default;
        StencilMaterial& operator=(StencilMaterial&& other) = default;

        void create() override;

        void setColor(glm::vec3 color) override {};
        void setShininess(float shininess) override {};

        void setLights(const std::vector<std::shared_ptr<Light>>& lights) override {};

        void setEmissiveColorAndStrength(glm::vec3 color, float strength) override {};
        void setEmissiveColor(glm::vec3 color) override {};
        void setEmissiveStrength(float strength) override {};
    private:
};
