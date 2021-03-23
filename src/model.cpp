#include "model.hpp"

#include "material/material.hpp"
#include "mesh.hpp"

#include <GL/glew.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

#include <iostream>

// ctor - Takes in unique_ptr references and moves them.
// Question: Is it better to do the std::move here, or do it when we call the
// ctor in the first place?
Model::Model(std::shared_ptr<Mesh> m, std::unique_ptr<Material>&& mat) :
    // std::move is still required here, otherwise it would just be a copy, defeating the whole point
    // of passing mesh and material by rvalue ref in the first place
    mesh(m)
{
    mat->create();
    materials.emplace(MaterialType::standard, std::move(mat));
}

Model::~Model() {}

Model::Model(Model&& other) {
    mesh = std::move(other.mesh);
    materials = std::move(other.materials);
    rotation = std::move(other.rotation);
    scale = std::move(other.scale);
    position = std::move(other.position);
    dirty = std::move(other.dirty);
}

Model& Model::operator=(Model&& other) {
    mesh = std::move(other.mesh);
    materials = std::move(other.materials);
    rotation = std::move(other.rotation);
    scale = std::move(other.scale);
    position = std::move(other.position);
    dirty = std::move(other.dirty);

    return *this;
}

void Model::addMaterial(MaterialType type, std::unique_ptr<Material>&& mat) {
    mat->create();
    materials.emplace(type, std::move(mat));
}

void Model::setColor(glm::vec3 color) {
    for (auto& m : materials) {
        m.second->setColor(color);
    }
}

void Model::setMetalness(float metalness) {
    for (auto& m : materials) {
        m.second->setMetalness(metalness);
    }
}

void Model::setRoughness(float roughness) {
    for (auto& m : materials) {
        m.second->setRoughness(roughness);
    }
}

void Model::toggleEmissive(bool value) {
    for (auto& m : materials) {
        m.second->toggleEmissive(value);
    }
}

void Model::toggleBlinnPhongShading(bool value) {
    for (auto& m : materials) {
        m.second->toggleBlinnPhongShading(value);
    }
}

void Model::setEmissiveColor(glm::vec3 color) {
    for (auto& m : materials) {
        m.second->setEmissiveColor(color);
    }
}

void Model::setEmissiveStrength(float strength) {
    for (auto& m : materials) {
        m.second->setEmissiveStrength(strength);
    }
}

void Model::setEmissiveColorAndStrength(glm::vec3 color, float strength) {
    for (auto& m : materials) {
        m.second->setEmissiveColorAndStrength(color, strength);
    }
}

void Model::setLights(const std::vector<std::shared_ptr<Light>>& lights) {
    for (auto& m : materials) {
        m.second->setLights(lights);
    }
}

void Model::setProjectionAndViewMatrices(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
    for (auto& m : materials) {
        m.second->setProjectionAndViewMatrices(projectionMatrix, viewMatrix);
    }
}

void Model::applyModelMatrix() {
    if (!dirty) {
        return;
    }

    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrix = modelMatrix * glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);

    for (auto& m : materials) {
        m.second->setModelMatrix(modelMatrix);
    }

    dirty = false;
}

void Model::draw(MaterialType type) const {
    if (materials.count(type) == 0) {
        return;
    }

    materials.at(type)->setUniforms();
    glUseProgram(materials.at(type)->getProgram());

    auto side = materials.at(type)->getSide();

    // TODO: Support both sides
    if (side == Side::BACK) {
        glCullFace(GL_FRONT);
    } else {
        glCullFace(GL_BACK);
    }

    glBindVertexArray(mesh->getVertexArrayObject());
    glDrawArrays(GL_TRIANGLES, 0, mesh->getVertexCount());

    // set back to BACK
    glCullFace(GL_BACK);

    glUseProgram(0);
}
