/**
 * Author: Matthew Cargill (903854144)
 * Class: ECE4122-A
 * Last Date Modified: 4/28/2026
 * Description: Header file for Cuda based array. Calc the pattern on GPU and
 * directly write to VBO with interop.
*/

#pragma once

/**
* Initializes CUDA and registers the VBO for CUDA access. Must be called before any other CUDA functions.
*
*/
void initCuda(GLuint vbo);

/**
* Unregisters the VBO and cleans up CUDA resources. Should be called on application exit.
*
*/
void cleanupCuda();

/**
* Calculates the array factor squared on the GPU and writes the results directly to the given VBO. 
* The VBO should have been registered with CUDA using initCuda() beforehand.
*
*/
void calcAFSqCUDA(GLuint vbo);