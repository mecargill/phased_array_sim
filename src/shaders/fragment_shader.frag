/**
 * Author: Matthew Cargill (903854144)
 * Class: ECE4122-A
 * Last Date Modified: 4/28/2026
 * Description: Fragment shader for phased array visualization
*/
// Fragment Shader
#version 330 core

uniform bool constColor;

in float magDb;
in float maxMagDb;
in vec3 fragPos;

out vec4 FragColor;

void main()
{ 
    vec3 dx = dFdx(fragPos);
    vec3 dy = dFdy(fragPos);
    vec3 normal = normalize(cross(dx, dy));  //dx is right, dy must be up, so cross(dx, dy) give normal pointing out

    //normal is out, we start at -y looking at +y with x on right
    vec3 lightDir = normalize(vec3(1.0, -0.2, 1.0));
    float diffuse = max(dot(normal, lightDir), 0.0);
    float ambient = 0.3;
    float light = ambient + (1.0 - ambient) * diffuse;


    
    vec3 color;
    if (constColor)
    {
        color = vec3(1.0, 1.0, 1.0);
    }
    else
    {
        float mixFactor = magDb / maxMagDb;

        vec3 orange = vec3(0.996, 0.35, 0.0); 
        vec3 blue = vec3(0.436, 0.614, 0.988);
        color = mix(blue, orange, mixFactor);
    }

    FragColor = vec4(light*color, 1.0); 

}