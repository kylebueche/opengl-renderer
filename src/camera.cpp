#include "camera.h"

Camera::Camera()
{
    fov = 45.0f;
    width = 800;
    height = 600;
    nearPlane = 0.1f;
    farPlane = 100.0f;

    position = glm::vec3(0.0f, 0.0f, 10.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    
    yaw = -90.0;
    pitch = 0.0f;

    updateView();
    updateProjection();
}

void Camera::updateView()
{
    view = glm::lookAt(position, position + front, up);
}

void Camera::updateProjection()
{
    projection = glm::perspective(glm::radians(45.0f), (float) width / (float) height, nearPlane, farPlane);
}

void Camera::moveForward(float distance)
{
    glm::vec3 frontXZ = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    position += frontXZ * distance;
}

void Camera::moveBackward(float distance)
{
    glm::vec3 frontXZ = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    position -= frontXZ * distance;
}

void Camera::moveLeft(float distance)
{
    position -= glm::normalize(glm::cross(front, up)) * distance;
}

void Camera::moveRight(float distance)
{
    position += glm::normalize(glm::cross(front, up)) * distance;
}

void Camera::moveUp(float distance)
{
    position += up * distance;
}

void Camera::moveDown(float distance)
{
    position -= up * distance;
}

void Camera::rotateBy(float xAngle, float yAngle)
{
    yaw += xAngle;
    pitch += yAngle;

    pitch = std::min(std::max(pitch, -89.0f), 89.0f);
    
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
}

void Camera::zoomBy(float amount)
{
    fov += amount;
    fov = std::min(std::max(fov, 1.0f), 45.0f);
}
