/**
 * Author: Matthew Cargill (903854144)
 * Class: ECE4122-A
 * Last Date Modified: 4/28/2026
 * Description: Implementation for the Camera class. This class manages the position and orientation
 * and updates based on controls
*/
#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>


/**
* Constructor for the camera class. Sets the position and orientation to the default and updates the basis vectors
*/
Camera::Camera()
{
    reset();
    updateVectors();
}

/**
* Returns the view matrix for the camera based on the current position and orientation. 
* This is used in the vertex shader to transform world coordinates to view coordinates.
*/
glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

/**
* Takes in a direction and a delta time and updates the position of the camera based on the speed and direction.
*/
//needs normalized vectors
void Camera::processKeyboard(CameraDir dir, float dt)
{
    float dist = speed * dt;
    switch (dir) 
    {
        case CameraDir::UP:
            position += dist * worldUp;
            break;
        case CameraDir::DOWN:
            position -= dist * worldUp;
            break;
        case CameraDir::LEFT:
            position -= dist * right;
            break;
        case CameraDir::RIGHT:
            position += dist * right;
            break;
        case CameraDir::FORWARD:
            position += dist * front;
            break;
        case CameraDir::BACKWARD:
            position -= dist * front;
            break;
    }
}

/**
* Takes in the change in mouse position and updates the orientation of the camera based on the mouse sensitivity.
*/
//when the mouse has moved a positive dy, it has moved down on screen - so theta increases
//when dx is pos, it's moved right, that's in the dir of decreasing phi
void Camera::processMouseMovement(float dx, float dy, bool constrainPitch)
{
    phi -= dx*mouseSens;
    theta += dy*mouseSens;

    theta = (theta <= 179) ? theta : 179;
    theta = (theta >= 1) ? theta : 1;
    
    updateVectors();
}

/**
* Resets the position and orientation of the camera to the default. This is called when the user presses the R key.
*/
//look in pos x direction, so start at minus x position
void Camera::reset()
{
    position = { 0.0f, -15.0f, 1.0f };
    theta = 90;
    phi = 90;
    fov = 45;
    speed = 8;

    updateVectors();
}

/**
* Updates the front, right, and up vectors based on the current theta and phi. 
* This is called whenever the orientation of the camera changes.
*/
//these should be normalized
void Camera::updateVectors()
{
    front.x = sin(glm::radians(theta)) * cos(glm::radians(phi));
    front.y = sin(glm::radians(theta)) * sin(glm::radians(phi));
    front.z = cos(glm::radians(theta));
    //front is already normalized

    right = normalize(cross(front, worldUp));

    up = cross(right, front);
    //up is already normalized
}

