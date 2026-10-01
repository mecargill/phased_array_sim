/**
 * Author: Matthew Cargill (903854144)
 * Class: ECE4122-A
 * Last Date Modified: 4/28/2026
 * Description: Header file for visualization. These functions take care of openGL stuff as well as the GUI.
*/
#pragma once
#include <GLFW/glfw3.h>

enum class RenderMode { SINGLE_THREAD, MULTI_THREAD, CUDA };

extern RenderMode renderMode;
extern GLFWwindow *g_window;
extern GLuint g_vao, g_vbo, g_ebo;

/**
* Initializes the visualization, including setting up OpenGL, creating the window, and initializing ImGui.
* 
*/
bool initVisualization();

/**
* Takes care of starting the ImGui frame and capturing the state of the GUI (ie whether sliders were changed, etc.)
* 
*/
void captureGUIState();

/**
* Updates internal variables based on the state of the GUI. Returns whether any variables were changed, which is used to determine whether to recalculate the radiation pattern.
*
*/
bool updateVarsFromGUI();

/**
* Draws the GUI. This should be called after drawing the radiation pattern, but before swapping buffers.
*
*/
void drawGUI();

/**
* Cleans up the visualization, including destroying the window, cleaning up ImGui, and cleaning up any CUDA memory.
*
*/
void visualizationCleanup();

/**
* Clears the screen and sets the background color. This should be called at the beginning of each frame before drawing anything.
*
*/
void clearScreen();

/**
* Calculates the radiation pattern based on the current parameters. This will update the VBO with new data, which will be drawn in drawRadiation(). 
* The method of calculation (single thread, multi thread, or CUDA) is determined by the renderMode variable.
*
*/
void calcRadiationPattern();

/**
* Draws the radiation pattern, the array, and the axes. This should be called after clearScreen() and before drawGUI() in each frame.
*
*/
void drawRadiation();

/**
* Processes any queued movements, including camera rotations and keyboard inputs. This should be called after captureGUIState() and before updateVarsFromGUI() in each frame.
*
*/
void executeMovements();

/**
* Processes keyboard inputs for camera movement and other controls. This should be called in executeMovements().
*
*/
void processKeyInputs();

/**
* Initializes the VBO and EBO for the radiation pattern. This includes setting up the vertex attributes, linking the VBO to CUDA, and 
* filling the EBO with the appropriate indices for drawing the triangles.
*
*/
void initRadVBOandEBO();

/**
* Initializes the VBO and EBO for the array representation. This includes setting up the vertex attributes and filling the EBO with the 
* appropriate indices for drawing the triangles that represent the array elements.
*
*/
void initArrayVBOandEBO();

/**
* Initializes the VBO for the axes. This includes setting up the vertex attributes and filling the VBO with the appropriate vertex data for drawing the axes as lines.
*
*/
void initAxesVBO();