#
#boiler plate
#

cmake_minimum_required(VERSION 3.18)
project(PAVis LANGUAGES C CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLEW)
add_definitions(-DGLFW_EXPOSE_NATIVE_WGL)
add_definitions(-D_WIN32)
#
#check for cuda
#

if(NOT CMAKE_CUDA_COMPILER)
    message(FATAL_ERROR "CUDA compiler not found. Is the toolkit installed?")
endif()

set(CMAKE_CUDA_ARCHITECTURES native)

#
#create the target
#

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS 
        "src/*.cpp" 
        "src/*.cu" 
        "external/ImGUI/*.cpp"
        )
add_executable(PAVis ${SOURCES})

#
#set the output folder
#

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/bin")

set_target_properties(PAVis PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

#
#get all the libraries set up - glfw, glew, opengl
#

find_package(OpenGL REQUIRED)
find_package(OpenMP REQUIRED)
find_package(CUDAToolkit REQUIRED)
add_subdirectory (external/glfw-3.1.2)

set(GLEW_SOURCE external/glew-1.13.0/src/glew.c)
set(GLEW_HEADERS external/glew-1.13.0/include/GL/glew.h)
add_library( GLEW_1130 STATIC
	${GLEW_SOURCE}
	${GLEW_HEADERS}
)



target_include_directories(PAVis PRIVATE ${CMAKE_SOURCE_DIR}/src)

include_directories(
    ./external/glfw-3.1.2/include/
    ./external/glm-0.9.7.1/
    ./external/glew-1.13.0/include/
    ./external/ImGUI/
    ./external/ImGUI/backends/
    .
)


add_compile_definitions(
    TW_STATIC
    TW_NO_LIB_PRAGMA
    TW_NO_DIRECT3D
    GLEW_STATIC
    _CRT_SECURE_NO_WARNINGS
)

#
#link the libraries
#

set(ALL_LIBS
    OpenGL::GL
    glfw
    GLEW_1130
    OpenMP::OpenMP_CXX
    CUDA::cudart
)

target_link_libraries(PAVis PRIVATE
    ${ALL_LIBS}
)

#
#Copy any shaders over
#

set(SHADER_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src/shaders")
set(SHADER_DEST_DIR   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders")



file(GLOB_RECURSE SHADER_FILES
    CONFIGURE_DEPENDS
    "${SHADER_SOURCE_DIR}/*"
)


# Create a custom target that copies files on every build
add_custom_target(copy_assets ALL
    COMMENT "Copying assets and shaders to build directory..."
)


foreach(SRC_FILE ${SHADER_FILES})
    # Compute the file's path relative to SOURCE_DIR
    file(RELATIVE_PATH REL_PATH "${SHADER_SOURCE_DIR}" "${SRC_FILE}")

    set(DST_FILE "${SHADER_DEST_DIR}/${REL_PATH}")

    # Get the destination subfolder so we can create it if needed
    get_filename_component(DST_DIR "${DST_FILE}" DIRECTORY)

    add_custom_command(
        TARGET copy_assets PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${DST_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SRC_FILE}" "${DST_FILE}"
        COMMENT "Copying ${REL_PATH}"
    )
endforeach()


# Make your executable depend on the copy step
add_dependencies(PAVis copy_assets)