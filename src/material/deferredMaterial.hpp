#pragma once

#include "material.hpp"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

class DeferredMaterial : public Material {
    public:
        DeferredMaterial(glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0), float specularCoefficient = 0.5f, float shininess = 32.0f);
        ~DeferredMaterial();

        DeferredMaterial(DeferredMaterial&& other) = default;
        DeferredMaterial(const DeferredMaterial& other) = default;

        DeferredMaterial& operator=(const DeferredMaterial& other) = default;
        DeferredMaterial& operator=(DeferredMaterial&& other) = default;

        void create() override;

        void setShininess(float shininess) override { (void)shininess; }

        void setLights(const std::vector<std::shared_ptr<Light>>& lights) override { (void)lights; }
        void toggleBlinnPhongShading(bool value) override { (void)value; }
    private:
};
