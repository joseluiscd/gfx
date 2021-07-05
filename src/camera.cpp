#include <gfx/camera.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace gfx {

void PerspectiveCameraLens::update_matrix()
{
    *proj_matrix = glm::perspective(fovy, aspect, znear, zfar);
}

void CameraRig::update_matrix()
{
    *view_matrix = glm::lookAt(v_position, v_lookat, v_up);
    n = glm::normalize(v_position - v_lookat);
    u = glm::cross(glm::vec3(0.0, 1.0, 0.0), n);
    v = glm::cross(n, u);
}

void CameraRig::pan(float angle)
{
    glm::mat4 transform = glm::translate(v_position)
        * glm::rotate(-angle, glm::vec3(0.0, 1.0, 0.0))
        * glm::translate(-v_position);

    v_lookat = glm::vec3(transform * glm::vec4(v_lookat, 1.0));

    update_matrix();
}

void CameraRig::tilt(float angle)
{
    glm::mat4 transform = glm::translate(v_position) * glm::rotate(angle, u) * glm::translate(-v_position);

    v_lookat = glm::vec3(transform * glm::vec4(v_lookat, 1.0));
    update_matrix();
}

void CameraRig::dolly(float distance)
{
    glm::mat4 transform = glm::translate(-distance * n);
    v_lookat = glm::vec3(transform * glm::vec4(v_lookat, 1.0));
    v_position = glm::vec3(transform * glm::vec4(v_position, 1.0));
    update_matrix();
}

void CameraRig::truck(float distance)
{
    glm::mat4 transform = glm::translate(distance * u);
    v_lookat = glm::vec3(transform * glm::vec4(v_lookat, 1.0));
    this->v_position = glm::vec3(transform * glm::vec4(this->v_position, 1.0));
    update_matrix();
}

void CameraRig::crane(float distance)
{
    glm::mat4 transform = glm::translate(glm::vec3(0, distance, 0));
    v_lookat = glm::vec3(transform * glm::vec4(v_lookat, 1.0));
    v_position = glm::vec3(transform * glm::vec4(v_position, 1.0));
    update_matrix();
}

void CameraRig::vertical(float distance)
{
    glm::mat4 transform = glm::translate(distance * v);
    v_lookat = glm::vec3(transform * glm::vec4(v_lookat, 1.0));
    this->v_position = glm::vec3(transform * glm::vec4(this->v_position, 1.0));
    update_matrix();
}

void CameraRig::advance(float distance)
{
    glm::mat4 transform = glm::translate(-distance * n);
    this->v_position = glm::vec3(transform * glm::vec4(this->v_position, 1.0));
    update_matrix();
}

void CameraRig::orbit(float angle)
{
    glm::mat4 transform = glm::translate(v_lookat)
        * glm::rotate(-angle, glm::vec3(0.0, 1.0, 0.0))
        * glm::translate(-v_lookat);
    v_position = glm::vec3(transform * glm::vec4(this->v_position, 1.0));
    update_matrix();
}

void CameraRig::orbit_vertical(float angle)
{
    glm::mat4 transform = glm::translate(v_lookat)
        * glm::rotate(-angle, this->u)
        * glm::translate(-v_lookat);
    v_position = glm::vec3(transform * glm::vec4(this->v_position, 1.0));
    update_matrix();
}
}