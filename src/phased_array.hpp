/**
 * Author: Matthew Cargill (903854144)
 * Class: ECE4122-A
 * Last Date Modified: 4/28/2026
 * Description: Header file for CPP based phased array. These functions calculate the radiation pattern
 * and update the VBO with new data.
*/
#pragma once
#include <GL/glew.h>

//These variables set default behavior
constexpr int defaultArrayWidth = 8;
constexpr int defaultArrayHeight = 8;
constexpr float defaultXElementSpacing = 1.0 / 2; //bc wavelength is 1, this is in units of wavelengths
constexpr float defaultYElementSpacing = 1.0 / 2;
constexpr float defaultSteerThetaRad = 0;
constexpr float defaultSteerPhiRad = 0;

//These control the sampling scheme used - not evenly around a sphere, but makes tiling with triangles far easier
constexpr float PI = 3.14159265358979323846;
constexpr int numThetas = 180 * 5;
constexpr int numPhis = 360 * 5;
constexpr float dTheta = PI / numThetas;
constexpr float dPhi = 2 * PI / numPhis;
#define k 2 * 3.14159265358979323846 //wavelength is implicitly 1

//These variables control the simulation, and can be changed by the user. They are defined in phased_array.cpp.
// phased_array.cpp uses them, but the sliders modify them in vis.cpp - that's why they're extern
//convention will be that width is in the x direction and height is in y
extern int arrayWidth;
extern int arrayHeight;
extern float xElementSpacing; //bc wavelength is 1, this is in units of wavelengths
extern float yElementSpacing;
extern float steerThetaRad;
extern float steerPhiRad;

/**
* Calculates the radiation pattern of the phased array and updates the VBO with new data. 
* This is the single threaded version, which is used for a speedup comparison. 
* It performs very poorly
*/
void calcAFSqSingleThread(GLuint vbo);

/**
* Calculates the radiation pattern of the phased array and updates the VBO with new data.
* A little faster, but still worse than CUDA.
*/
void calcAFSqMP(GLuint vbo);