/**
 * Author: Matthew Cargill (903854144)
 * Class: ECE4122-A
 * Last Date Modified: 4/28/2026
 * Description: Header for the Camera class. This class manages the position and orientation
 * and updates based on controls
*/
#pragma once
// ─────────────────────────────────────────────────────────────────────────────
// camera.h  –  Free-look FPS camera
//
//
// Controls:
//   W / A / S / D        – forward / left / backward / right
//   Q / E                – down / up along world Y
//   Right Mouse + drag   – yaw / pitch (FPS style)
//   Scroll wheel         – change speed
//   R                    – reset to position (0, 5, 15) looking at origin
// ─────────────────────────────────────────────────────────────────────────────
#include <glm/glm.hpp>

constexpr float MIN_SPEED = 1;
constexpr float MAX_SPEED = 100;

// Direction enum used by processKeyboard()
enum class CameraDir { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

//The camera class manages the users position and what direction they are facing
//It updated these based on input from main
//It also gets the view matrix based on this
class Camera
{
private:

    glm::vec3 position;
    glm::vec3 worldUp { 0.f, 0.f, 1.f };

    float theta;   // degrees; physics convention - theta is polar angle
    float phi;   // degrees; physics convention - phi is azimuthal angle

    float speed     { 4.f };   // world units per second
    float mouseSens { 0.1f };
    float fov       { 45.f };  // vertical FOV in degrees

    // Derived basis vectors – kept up-to-date by updateVectors()
    glm::vec3 front { 0.f, 0.f, -1.f };
    glm::vec3 right { 1.f, 0.f,  0.f };
    glm::vec3 up    { 0.f, 1.f,  0.f };
    
public:
    /**
    * Constructor for the camera class. Sets the position and orientation to the default and updates the basis vectors
    */
    Camera();

    /**
    * Returns the view matrix for the camera based on the current position and orientation.
    * This is used in the vertex shader to transform world coordinates to view coordinates.
    */
    glm::mat4 getViewMatrix() const;

    float getFOVRad() {return glm::radians(fov);}

    /**
     * Takes in a direction and a delta time and updates the position of the camera based on the speed and direction.
     */
    void processKeyboard(CameraDir dir, float dt);

    /**
     * Takes in the change in mouse position and updates the orientation of the camera based on the mouse sensitivity.
     */
    void processMouseMovement(float dx, float dy, bool constrainPitch = true);

    /**
    * Resets the position and orientation of the camera to the default. This is called when the user presses the R key.
    */
    void reset();

    /**
    * Updates the front, right, and up vectors based on the current theta and phi.
    * This is called whenever the orientation of the camera changes.
    */
    void updateVectors();
  
};
