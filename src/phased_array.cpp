/**
 * Author: Matthew Cargill (903854144)
 * Class: ECE4122-A
 * Last Date Modified: 4/28/2026
 * Description: Implementation file for CPP based phased array. These functions calculate the radiation pattern
 * and update the VBO with new data.
*/

#include <cmath>
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <omp.h>
#include "phased_array.hpp"

//Set default extern params
int arrayWidth = defaultArrayWidth;
int arrayHeight = defaultArrayHeight;
float xElementSpacing = defaultXElementSpacing; //bc wavelength is 1, this is in units of wavelengths
float yElementSpacing = defaultYElementSpacing;
float steerThetaRad = defaultSteerThetaRad;
float steerPhiRad = defaultSteerPhiRad;

//indexing is [x index] [y index] .<first for x loc, second for y loc>] 
std::vector<std::vector<std::pair<float, float>>> arrayLocs(
	50,
	std::vector<std::pair<float, float>>(50)
);

//use a single function to make maintaining single thread and openmp easier
//this is a standard AF calculation, steered so that phase shift is 0 in the desired dir
float calcSingleAF(float theta, float phi)
{
	//cache trig values
	float sumReal = 0;
	float sumImag = 0;
	float sinTheta = sin(theta);
	float sinPhi = sin(phi);
	float cosPhi = cos(phi);
	float sinSteerTheta = sin(steerThetaRad);
	float sinSteerPhi = sin(steerPhiRad);
	float cosSteerPhi = cos(steerPhiRad);
	for (int xIndex = 0; xIndex < arrayWidth; xIndex++)
	{
		for (int yIndex = 0; yIndex < arrayHeight; yIndex++)
		{
			float xLoc = xIndex * xElementSpacing;
			float yLoc = yIndex * yElementSpacing;
			float phaseShift = k *
				(
					xLoc * sinTheta * cosPhi
					+ yLoc * sinTheta * sinPhi
					- xLoc * sinSteerTheta * cosSteerPhi
					- yLoc * sinSteerTheta * sinSteerPhi
					);
			sumReal += cos(phaseShift);
			sumImag += sin(phaseShift);
		}
	}

	return sumReal * sumReal + sumImag * sumImag;
}

void calcAFSqSingleThread(GLuint vbo)
{
	
	//the convention will be: [block with same theta but varying phi, block with next theta and varying phi, etc.]
	float* arr = new float[numThetas * numPhis];
	//the i loops over thetas, j over phis
	for (int i = 0; i < numThetas; i++)
	{
		for (int j = 0; j < numPhis; j++)
		{
			float theta = i * dTheta;
			float phi = j * dPhi;

			arr[i * numPhis + j] = calcSingleAF(theta, phi);
		
		}
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, numThetas * numPhis * sizeof(float), arr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] arr;
	
}


//CHANGE THIS TO A SINGLE LOOP FOR STUPID MSVC++ COMPILER
void calcAFSqMP(GLuint vbo)
{
	
	//the convention will be: [block with same theta but varying phi, block with next theta and varying phi, etc.]
	float* arr = new float[numThetas * numPhis];
	//the i loops over thetas, j over phis
	#pragma omp parallel for
	for (int idx = 0; idx < numThetas * numPhis; idx++)
	{
		int i = idx / numPhis;
		int j = idx % numPhis;

		float theta = i * dTheta;
		float phi = j * dPhi;

		//when I take log for db later, the square will just be a factor of 2
		arr[idx] = calcSingleAF(theta, phi);
	}
	
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, numThetas * numPhis * sizeof(float), arr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] arr;

}





