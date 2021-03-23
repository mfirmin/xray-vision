#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

// Forward declare dependencies to reduce compilation-unit dependencies
class Light;
class Material;
class Mesh;

enum class MaterialType {
    standard,
    deferred,
    deferred_pbr
};

class Model {
    public:
        // ctor. Takes in rvalue pointer references.
        Model(std::shared_ptr<Mesh> mesh, std::unique_ptr<Material>&& material);
        // Move ctor
        Model(Model&& other);
        // Move assignment operator
        Model& operator=(Model&& other);

        // Explicitly delete these as we cannot trivially
        // copy a class which has unique pointers
        // Alternatively, we could do a deep copy on mesh and material
        // But there's no way to simply copy the pointers, unless we use shared_ptrs.
        Model(const Model& other) = delete;
        Model& operator=(const Model& other) = delete;

        // Cannot use = default; as the compiler will complain about calling the destructor
        // of incomplete types
        // Therefore we have to define this in the .cpp file (even if it does nothing)
        ~Model();

        void setColor(glm::vec3 color);
        void setEmissiveColorAndStrength(glm::vec3 color, float strength);
        void setMetalness(float metalness);
        void setRoughness(float roughness);

        void setEmissiveColor(glm::vec3 color);
        void setEmissiveStrength(float strength);
        void toggleEmissive(bool value);
        void toggleBlinnPhongShading(bool value);

        void setLights(const std::vector<std::shared_ptr<Light>>& lights);
        void setProjectionAndViewMatrices(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
        void draw(MaterialType type) const;

        void setPosition(glm::vec3 p) {
            position = p;
            dirty = true;
        }

        void setRotation(glm::vec3 r) {
            rotation = r;
            dirty = true;
        }

        void setScale(glm::vec3 s) {
            scale = s;
            dirty = true;
        }

        void setScale(float s) {
            scale = glm::vec3(s, s, s);
            dirty = true;
        }

        void applyModelMatrix();

        void addMaterial(MaterialType type, std::unique_ptr<Material>&& mat);
    private:
        glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

        bool dirty = true;

        // Meshes can be shared between models
        std::shared_ptr<Mesh> mesh;
        // Materials must be unique (for now?)

        std::unordered_map<MaterialType, std::unique_ptr<Material>> materials = {};
};
