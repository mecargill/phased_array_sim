/**
 * Author: Matthew Cargill (903854144)
 * Class: ECE4122-A
 * Last Date Modified: 4/28/2026
 * Description: Implementation file for visualization. These functions take care of openGL stuff as well as the GUI.
*/
#include <iostream>
#include <queue>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


#include "camera.hpp"
#include "shader.hpp"
#include "phased_array.hpp"
#include "vis.hpp"
#include "cuda_array.hpp"

constexpr bool lazyUpdate = false;

constexpr int SCR_W = 1600;
constexpr int SCR_H = 1080;


Camera g_camera;
Shader g_shader;
GLFWwindow *g_window;
GLuint g_vao, g_vbo, g_ebo; //these are for the radiation pattern
GLuint g_arrayVao, g_arrayVbo, g_arrayEbo; //these are for the representation of the array
GLuint g_axVao, g_axVbo; //these are for the axes
RenderMode renderMode = RenderMode::MULTI_THREAD;
bool g_firstMouse, g_mb, rotateQueued;
double g_lastX;
double g_lastY;
double dx;
double dy;
float g_deltaTime;
float g_lastFrame;
float avgCalcTime = 0.f;
std::queue<float> calcTimes;


static void framebufferSizeCallback(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);
}

static void mouseButtonCallback(GLFWwindow*, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            g_mb = true;
            g_firstMouse = true; //g_firstmouse is reset to false in cursorPosCallback once the first mouse movement is recorded
        }
        else if (action == GLFW_RELEASE)
        {
            g_mb = false;
        }
    }
}

static void cursorPosCallback(GLFWwindow*, double xpos, double ypos)
{
    if (g_mb)
    {
        if (g_firstMouse)
        {
            g_lastX = xpos;
            g_lastY = ypos;
            g_firstMouse = false;
        }
        else
        {
			rotateQueued = true;
            dx = xpos - g_lastX;
            dy = ypos - g_lastY;
            g_lastX = xpos;
            g_lastY = ypos;
        }
    }
    
}

void executeMovements()
{
    if (ImGui::GetIO().WantCaptureMouse)
    {
        rotateQueued = false;
    }

    if (rotateQueued)
    {
        
        g_camera.processMouseMovement(dx, dy);
        rotateQueued = false;
	}
    processKeyInputs();
}


//polling this means that I don't have to store state for keys
void processKeyInputs()
{
    float now = (float)glfwGetTime();
    g_deltaTime = now - g_lastFrame;
    g_lastFrame = now;

    if (glfwGetKey(g_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(g_window, true);
    }
    if (glfwGetKey(g_window, GLFW_KEY_R) == GLFW_PRESS)
    {
        g_camera.reset();
    }
    if (glfwGetKey(g_window, GLFW_KEY_W) == GLFW_PRESS)
    {
        g_camera.processKeyboard(CameraDir::FORWARD, g_deltaTime);
    }
    if (glfwGetKey(g_window, GLFW_KEY_A) == GLFW_PRESS)
    {
        g_camera.processKeyboard(CameraDir::LEFT, g_deltaTime);
    }
    if (glfwGetKey(g_window, GLFW_KEY_S) == GLFW_PRESS)
    {
        g_camera.processKeyboard(CameraDir::BACKWARD, g_deltaTime);
    }
    if (glfwGetKey(g_window, GLFW_KEY_D) == GLFW_PRESS)
    {
        g_camera.processKeyboard(CameraDir::RIGHT, g_deltaTime);
    }
    if (glfwGetKey(g_window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        g_camera.processKeyboard(CameraDir::DOWN, g_deltaTime);
    }
    if (glfwGetKey(g_window, GLFW_KEY_E) == GLFW_PRESS)
    {
        g_camera.processKeyboard(CameraDir::UP, g_deltaTime);
    }
    if (glfwGetKey(g_window, GLFW_KEY_1) == GLFW_PRESS)
    {
		if (renderMode != RenderMode::SINGLE_THREAD)
        {
            renderMode = RenderMode::SINGLE_THREAD;
            while (calcTimes.size() > 0)
                calcTimes.pop();
        }
    }
    if (glfwGetKey(g_window, GLFW_KEY_2) == GLFW_PRESS)
    {
        if (renderMode != RenderMode::MULTI_THREAD)
        {
            renderMode = RenderMode::MULTI_THREAD;
            while (calcTimes.size() > 0)
                calcTimes.pop();
        }
    }
    if (glfwGetKey(g_window, GLFW_KEY_3) == GLFW_PRESS)
    {
        if (renderMode != RenderMode::CUDA)
        {
            renderMode = RenderMode::CUDA;
            while (calcTimes.size() > 0)
                calcTimes.pop();
        }
    }
}

bool initVisualization()
{
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //Create GLFW Window
    g_window = glfwCreateWindow(SCR_W, SCR_H,"Phased Array Visualization", nullptr, nullptr);

    if (!g_window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(g_window);

    //turn on syncing with the screen refresh rate (to prevent unnecessary work) 
    glfwSwapInterval(1);

    //GLEW init
    //   glewExperimental enables core-profile extension pointers (VAOs, etc.).
    //   Must be called after glfwMakeContextCurrent().
    //   glGetError() clears the benign GL_INVALID_ENUM that glewInit may emit.
    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: "
                  << glewGetErrorString(glewErr) << "\n";
        glfwTerminate();
        return false;
    }
    glGetError(); // clear benign error
    
    glEnable(GL_DEPTH_TEST);

    glfwSetCursorPosCallback(g_window, cursorPosCallback);
    glfwSetFramebufferSizeCallback(g_window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(g_window, mouseButtonCallback);
    //shaders never change, so call this only once
    g_shader = Shader("./shaders/vertex_shader.vert", "./shaders/fragment_shader.frag");
    g_shader.use();

    //Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(g_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();

    return true;
}

void initRadVBOandEBO()
{
    glGenVertexArrays(1, &g_vao);
    glGenBuffers(1, &g_vbo);
    //activate the vao
    glBindVertexArray(g_vao);
    //activate the vbo so vao is attached to it
    glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
    glVertexAttrib3f(1, 0.0f, 0.0f, 0.0f);
	//set up the structure of vbo (defined in vao) - af is layout=0 - theta and phi locations will be calculated based on uniforms
    glVertexAttribPointer(
        0,                  //layout = 0
        1,                  //components per attribute
        GL_FLOAT,           //datatype
        GL_FALSE,           //normalized
        1 * sizeof(float),    //stride
        (void*)0            //offset to start at (ie if you wanted to store normal in same buffer)
    );
	glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, numThetas*numPhis*sizeof(float), nullptr, GL_STATIC_DRAW);

    
    //unbind for safety
    
    //link vbo to cuda
    initCuda(g_vbo);
    //give initial values to the vbo
    calcAFSqMP(g_vbo);

    //now for the ebo (we connect this theta to next, so last theta not needed)
	//but, for phi, it connects in a circle , so all phis needed
    //when I say top, right, etc. I mean looking in at origin with z up
    std::vector<unsigned int> indices;
	indices.reserve((numThetas - 1) * numPhis * 6);
    for (int i = 0; i < numThetas - 1; i++)
    {
        for (int j = 0; j < numPhis; j++)
        {
            unsigned int topLeft = i * numPhis + j; //points[i][j]
            unsigned int botLeft = (i + 1) * numPhis + j; //points[i+1][j]
            unsigned int topRight;
            unsigned int botRight;
            if (j < numPhis - 1)
            {
                topRight = i * numPhis + j + 1; //points[i][j+1]
                botRight = (i + 1) * numPhis + j + 1; //points[i+1][j+1]
            }
            else
            {
                topRight = i * numPhis; //points[i][0] - wrapped back to beginning
                botRight = (i + 1) * numPhis; //points[i+1][0]
            }
            //tl, bl, br and tl, br, tr
            indices.push_back(topLeft);
            indices.push_back(botLeft);
            indices.push_back(botRight);
            indices.push_back(topLeft);
            indices.push_back(botRight);
            indices.push_back(topRight);

            

        }
    }
    glGenBuffers(1, &g_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ebo);
    glBufferData(   GL_ELEMENT_ARRAY_BUFFER,
                    indices.size() * sizeof(unsigned int),
                    indices.data(),
                    GL_DYNAMIC_DRAW);
    glBindVertexArray(0);
}

void initArrayVBOandEBO()
{
    float halfW = 1;
	float halfH = 0.01;
    float arrayVerts[8*3] = {
        -halfW, -halfW,  halfH,       //0
         halfW, -halfW,  halfH,       //1
         halfW,  halfW,  halfH,       //2
        -halfW,  halfW,  halfH,       //3
        -halfW, -halfW, -halfH,       //4
         halfW, -halfW, -halfH,       //5
         halfW,  halfW, -halfH,       //6
        -halfW,  halfW, -halfH,       //7
	};
    glGenVertexArrays(1, &g_arrayVao);
    glGenBuffers(1, &g_arrayVbo);
    //activate the vao
    glBindVertexArray(g_arrayVao);
    //activate the vbo so vao is attached to it
    glBindBuffer(GL_ARRAY_BUFFER, g_arrayVbo);

    glVertexAttrib1f(0, 0.0f);

    //set up the structure of vbo (defined in vao) - af is layout=0 - theta and phi locations will be calculated based on uniforms
    glVertexAttribPointer(
        1,                  //layout = 1
        3,                  //vector components per attribute
        GL_FLOAT,           //datatype
        GL_FALSE,           //normalized
        3 * sizeof(float),    //stride
        (void*)0            //offset to start at (ie if you wanted to store normal in same buffer)
    );
	glEnableVertexAttribArray(1);

    glBufferData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(float), arrayVerts, GL_STATIC_DRAW);
	
    //unbind for safety
    

    //now for ebo
    unsigned int indices[6 * 3 * 2] = {
        0, 1, 2, 0, 2, 3, //front face
        4, 5, 6, 4, 6, 7, //back face
        0, 1, 5, 0, 5, 4, //bottom face
        2, 3, 7, 2, 7, 6, //top face
        1, 2, 6, 1, 6, 5, //right face
        0, 3, 7, 0, 7, 4 //left face
	};
    glGenBuffers(1, &g_arrayEbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_arrayEbo);
    glBufferData(   GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(indices),
                    indices,
                    GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void initAxesVBO()
{
   
    float lines[] = {
        -500,0,0,  500,0,0, // X
        0,-500,0,  0,500,0, // Y
        0,0,-500,  0,0,500  // Z
    };
    glGenVertexArrays(1, &g_axVao);
    glGenBuffers(1, &g_axVbo);
    glBindVertexArray(g_axVao);
    glBindBuffer(GL_ARRAY_BUFFER, g_axVbo);

    glVertexAttrib1f(0, 0.0f);
    glVertexAttribPointer(
        1,                  //layout = 1
        3,                  //vector components per attribute
        GL_FLOAT,           //datatype
        GL_FALSE,           //normalized
        3 * sizeof(float),    //stride
        (void*)0            //offset to start at (ie if you wanted to store normal in same buffer)
    );
    glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(1);
   

    glBindVertexArray(0);
    
}

void clearScreen()
{
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void calcRadiationPattern()
{
	static int avgLength = 5;
    float start = (float)glfwGetTime();
    
    switch (renderMode)
    {
    case RenderMode::SINGLE_THREAD:
        calcAFSqSingleThread(g_vbo);
        break;
    case RenderMode::MULTI_THREAD:
        calcAFSqMP(g_vbo);
        break;
    case RenderMode::CUDA:
        calcAFSqCUDA(g_vbo);
        break;
	}

    float end = (float)glfwGetTime();
    float calcTime = end - start;
    if (calcTimes.size() < avgLength)
    {
        calcTimes.push(calcTime);
		avgCalcTime = (avgCalcTime * (calcTimes.size()-1) + calcTime) / (calcTimes.size());
       
    }
    else
    {
        avgCalcTime -= calcTimes.front() / avgLength;
		avgCalcTime += calcTime / avgLength;
		avgCalcTime < 0 ? avgCalcTime = 0 : avgCalcTime; 
        calcTimes.pop();
        calcTimes.push(calcTime);
	}
}

//also draws array
void drawRadiation()
{

    glm::mat4 proj = glm::perspective(g_camera.getFOVRad(), static_cast<float>(SCR_W)/SCR_H, 0.1f, 500.0f);
    glm::mat4 view = g_camera.getViewMatrix();

	//set the uniforms 
    g_shader.setMat4("proj", proj);
    g_shader.setMat4("view", view);
    g_shader.setFloat("dTheta", dTheta);
	g_shader.setFloat("dPhi", dPhi);
	g_shader.setInt("numPhis", numPhis);
	g_shader.setInt("numElements", arrayWidth * arrayHeight);

    
    g_shader.setBool("constColor", false);
    glBindVertexArray(g_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ebo);
    glDrawElements(GL_TRIANGLES, numPhis * (numThetas - 1) * 6, GL_UNSIGNED_INT, 0);

    
    g_shader.setBool("constColor", true);
    glBindVertexArray(g_arrayVao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_arrayEbo);
    glDrawElements(GL_TRIANGLES, 6 * 3 * 2, GL_UNSIGNED_INT, 0);

    g_shader.setBool("constColor", true); 
    glBindVertexArray(g_axVao);
    glDrawArrays(GL_LINES, 0, 6);

       
}



void captureGUIState()
{
    // A. Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

bool updateVarsFromGUI()
{
    bool (*checkEdits)(void);
    if (lazyUpdate)
    {
		checkEdits = ImGui::IsItemDeactivatedAfterEdit;
    }
    else
    {
        checkEdits = ImGui::IsItemEdited;
    }

    bool result = false;

    ImGui::Begin("Controls");
    ImGui::Text("Phased Array Configuration");
    switch (renderMode)
    {
    case RenderMode::SINGLE_THREAD:
        ImGui::Text("Calculation Mode: Single Thread");
        break;
    case RenderMode::MULTI_THREAD:
        ImGui::Text("Calculation Mode: Multi Thread");
        break;
    case RenderMode::CUDA:
        ImGui::Text("Calculation Mode: CUDA");
        break;
    }
    
    if (calcTimes.size() > 0)
        ImGui::Text("Average Render Time %.2f s", avgCalcTime);
    else
		ImGui::Text("Average Render Time: calculating...");

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    ImGui::SliderFloat("Theta Steer", &steerThetaRad, 0.0f, PI/2);
    result |= (*checkEdits)();

    ImGui::SliderFloat("Phi Steer", &steerPhiRad, 0.0f, 2*PI);
    result |= (*checkEdits)();

    ImGui::SliderFloat("X Spacing (in wls)", &xElementSpacing, 0.0f, 2);
    result |= (*checkEdits)();

    ImGui::SliderFloat("Y Spacing (in wls)", &yElementSpacing, 0.0f, 2);
    result |= (*checkEdits)();

    ImGui::SliderInt("Array Width", &arrayWidth, 0, 20);
    result |= (*checkEdits)();

    ImGui::SliderInt("Array Height", &arrayHeight, 0, 20);
    result |= (*checkEdits)();

    if (ImGui::Button("Reset Beam"))
    {
        arrayWidth = defaultArrayWidth;
        arrayHeight = defaultArrayHeight;
        xElementSpacing = defaultXElementSpacing; //bc wavelength is 1, this is in units of wavelengths
        yElementSpacing = defaultYElementSpacing;
        steerThetaRad = defaultSteerThetaRad;
        steerPhiRad = defaultSteerPhiRad;

        result = true;
    }
    ImGui::End();
   
    return result;
}

void drawGUI()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void visualizationCleanup()
{
    // Cleanup Dear ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    //Cleanup any cuda memory
    cleanupCuda();
    // Cleanup GLFW
    glfwDestroyWindow(g_window);
    glfwTerminate();
}
