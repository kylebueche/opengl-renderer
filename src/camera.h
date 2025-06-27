#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class PlayerCamera
{
    public:
        glm::vec3 position;
        glm::vec3 rotation;
};

#endif
