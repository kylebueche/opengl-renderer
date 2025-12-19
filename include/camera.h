#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>

class Camera
{
    public:
        float fov;
        int width;
        int height;
        float nearPlane;
        float farPlane;

        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;

        float yaw;
        float pitch;

        glm::mat4 view;
        glm::mat4 projection;

        Camera();
        void updateView();
        void updateProjection();
        void moveForward(float distance);
        void moveBackward(float distance);
        void moveLeft(float distance);
        void moveRight(float distance);
        void moveUp(float distance);
        void moveDown(float distance);
        void rotateBy(float xAngle, float yAngle);
        void zoomBy(float amount);

};

#endif
