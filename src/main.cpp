/**
 * Author: Matthew Cargill (903854144)
 * Class: ECE4122-A
 * Last Date Modified: 4/28/2026
 * Description: Main entry point for the application. Handles startup, main loop, and cleanup.
 * This mainly gets the logic/flow down, the details are mostly in vis.cpp
*/
#include <iostream>

#include <GL/glew.h>

#include "vis.hpp"
#include "phased_array.hpp"

bool recalcNeeded = false;

int main(int argc, char *argv[])
{
    // ==================================================================
    //                             STARTUP
    // ==================================================================
    std::cout << "Loading...\n";
    //set up the window
    if (!initVisualization())
        return -1;
	//create the buffers to hold the data for the radiation pattern
	initRadVBOandEBO();
    initArrayVBOandEBO();
    initAxesVBO();
    std::cout << "Done Loading\n";

    // ==================================================================
    //                             MAIN LOOP
    // ==================================================================
    while (!glfwWindowShouldClose(g_window)) {
        //capture user input state (did they click gui? rotate?)
        glfwPollEvents();
        captureGUIState();

        //update internal state (update camera position, update slider vars, update whether slider vars were changed)
        executeMovements();
        recalcNeeded = updateVarsFromGUI();
		
        if (recalcNeeded)
        {
            calcRadiationPattern();
        }
		    

        //draw
        clearScreen();
        drawRadiation();
		drawGUI();

        //update the screen
        glfwSwapBuffers(g_window);
        
    }

    visualizationCleanup();
    return 0;

}