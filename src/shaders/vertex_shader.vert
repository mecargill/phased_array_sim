/**
 * Author: Matthew Cargill (903854144)
 * Class: ECE4122-A
 * Last Date Modified: 4/28/2026
 * Description: vertex shader for phased array visualization
*/
// Vertex Shader
#version 330 core

layout(location = 0) in float aMagSq;    // lane 0 = magnitude of array factor squared
layout(location = 1) in vec3 aPos;

uniform mat4 view;
uniform mat4 proj;
uniform float dTheta;
uniform float dPhi;
uniform int numPhis;
uniform int numElements;
uniform bool constColor;

out float magDb;
out float maxMagDb;
out vec3 fragPos;


void main()
{

    if (constColor)
    {
        gl_Position = proj * view * vec4(aPos, 1.0);
        fragPos = aPos;
        return;
    }
    //the convention will be: [block with same theta but varying phi, block with next theta and varying phi, etc.]
    int thetaIdx = gl_VertexID / numPhis;
    int phiIdx = gl_VertexID % numPhis;

    float theta = thetaIdx * dTheta;
    float phi = phiIdx * dPhi;

    float maxMagSq = numElements * numElements; //broadside, max magnitude squared is N^2
    float normMagSq = aMagSq/maxMagSq;

    //ok, i know this is ln not log10, oh well
    magDb = 10 * log(normMagSq);
    //now this id dB relative to -40dB from max, so max would be at 40dB always
    magDb = max(magDb, -40) + 40; //now this id dB relative to -40dB from max, so max would be at 40dB always
    //for my worlds scale Ill just keep the max at 10
    maxMagDb = 10;
    magDb /= 4;

    //convert to cartesian coordinates
    float x = magDb * sin(theta) * cos(phi);
    float y = magDb * sin(theta) * sin(phi);
    float z = magDb * cos(theta);
    fragPos = vec3(x, y, z);
    gl_Position = proj * view * vec4(x, y, z, 1.0);

}