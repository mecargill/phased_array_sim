/**
 * Author: Matthew Cargill (903854144)
 * Class: ECE4122-A
 * Last Date Modified: 4/28/2026
 * Description: Implementation file for Cuda based array. Calc the pattern on GPU and
 * directly write to VBO with interop.
*/
#include <GL/glew.h>
#include <cmath>
#include <iostream>

#include <cuda_runtime.h>
#include <cuda_gl_interop.h>

#include "phased_array.hpp"
#include "cuda_array.hpp"

cudaGraphicsResource_t cuda_vbo_resource;
bool initialized = false;
cudaError_t err;

void initCuda(GLuint h_vbo)
{
	//create the resource
	err = cudaGraphicsGLRegisterBuffer(&cuda_vbo_resource, h_vbo, cudaGraphicsRegisterFlagsNone);
	initialized = true;
}

void cleanupCuda()
{
	err = cudaGraphicsUnregisterResource(cuda_vbo_resource);
}

__global__ void calcAFSqKernel(	float* d_vbo, 
								int numThetas, 
								int numPhis, 
								float dTheta, 
								float dPhi, 
								float sinSteerTheta, 
								float sinSteerPhi, 
								float cosSteerPhi,
								int arrayWidth,
								int arrayHeight,
								float xElementSpacing,
								float yElementSpacing)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	
	if (idx < numPhis * numThetas)
	{
		int thetaIdx = idx / numPhis;
		int phiIdx = idx % numPhis;
		float theta = thetaIdx * dTheta;
		float phi = phiIdx * dPhi;

		//cache the trig functions
		float sumReal = 0;
		float sumImag = 0;
		float sinTheta = sinf(theta);
		float sinPhi = sinf(phi);
		float cosPhi = cosf(phi);
		
		for (int xIndex = 0; xIndex < arrayWidth; xIndex++)
		{
			for (int yIndex = 0; yIndex < arrayHeight; yIndex++)
			{
				float xLoc = xIndex * xElementSpacing;
				float yLoc = yIndex * yElementSpacing;
				//calc the phase shift by k times dot product of location and normal to a plane with the desired angle
				//add the weight phase which would cause the phase shift to be zero in the desired direction
				float phaseShift = k *
					(
						xLoc * sinTheta * cosPhi
						+ yLoc * sinTheta * sinPhi
						- xLoc * sinSteerTheta * cosSteerPhi
						- yLoc * sinSteerTheta * sinSteerPhi
						);
				sumReal += cosf(phaseShift);
				sumImag += sinf(phaseShift);
			}
		}
		d_vbo[idx] = sumReal*sumReal + sumImag*sumImag;
	}
		
}

void calcAFSqCUDA(GLuint vbo)
{
	if (!initialized)
	{
		std::cerr << "Error: Forgot to initialize CUDA\n";
		exit(-1);
	}
	int numSamples = numPhis * numThetas;
	float sinSteerTheta = sin(steerThetaRad);
	float sinSteerPhi = sin(steerPhiRad);
	float cosSteerPhi = cos(steerPhiRad);
	//lock vbo resource from openGL
	err = cudaGraphicsMapResources(1, &cuda_vbo_resource, 0);

	//create the device pointer to the vbo resource
	float* d_vbo = nullptr;
	size_t num_bytes = 0;
	err = cudaGraphicsResourceGetMappedPointer((void**)&d_vbo, &num_bytes, cuda_vbo_resource);

	//run the kernel to update vbo (round up not down)
	int threads = 256;
	int blocks = (numSamples + threads - 1) / threads;
	calcAFSqKernel <<<blocks, threads>>> (	d_vbo, 
											numThetas, 
											numPhis, 
											dTheta, 
											dPhi, 
											sinSteerTheta, 
											sinSteerPhi, 
											cosSteerPhi,
											arrayWidth,
											arrayHeight,
											xElementSpacing,
											yElementSpacing);
	err = cudaGetLastError();
	// wait for kernel to finish before unlocking from GL
	err = cudaDeviceSynchronize();
	err = cudaGraphicsUnmapResources(1, &cuda_vbo_resource, 0);
}

