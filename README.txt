I know you said if I did the walkthrough building wasn't necessary, but just in case, you need openGL, GLFW, GLEW, GLM, CUDA, OpenMP, and ImGUI.

I used the same openGL libraries from the homeworks - from the GitHub repo "OpenGL-Tutorials".

this is the directory structure I used:

src/
	shaders/
		shader files

	cpp and cu files
external/
	glew-1.13.0/
	glfw-3.1.2/
	glm-0.9.7.1/
	ImGUI/
		ImGUI files
		backends/
			ImGUI backend files
build/

CmakeLists.txt

	

I have included my CMakeLists.txt file, but have not included the external folder or build folder in my submission so that the submission is not super large
ImGUI can be found here: https://github.com/ocornut/imgui
You need all files from the top level directory, and from the backends directory you only need:
imgui_impl_opengl3_loader.h 
imgui_impl_glfw.cpp 
imgui_impl_glfw.h 
imgui_impl_opengl3.cpp 
imgui_impl_opengl3.h

I used Visual Studio and MSVC++ and CUDA linked to that, so that would be the best bet for reproducing. 
I've been building using VS built in Cmake features but I imagine you can just build with cmake as well.