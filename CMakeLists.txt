cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GLSP VERSION 1.0.0)

option(GLSP_BUILD_EXAMPLES "Build Examples Directory" ON)

#OpenGL should always be available ...
find_package(OpenGL REQUIRED)

#Set up dependencies
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/glfw-3.4)
add_subdirectory(deps/glad)
add_subdirectory(deps/glm-1.0.1)
add_subdirectory(deps/imgui) 
add_subdirectory(deps/stb_image)
add_subdirectory(deps/tiny_obj_loader)	
add_subdirectory(deps/tinyply)	

#Setup project own source code
file(GLOB GLSP_SOURCES "src/*.cpp")
file(GLOB GLSP_HEADERS "include/GLSP/*.h")
add_library(GLSP STATIC ${GLSP_SOURCES} ${GLSP_HEADERS})


if(MSVC) # If using the VS compiler...

	# set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	# set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

endif()

#Include directories
target_include_directories(GLSP PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
#Link libraries
target_link_libraries(GLSP PUBLIC glm glfw glad stb_image imgui tiny_obj_loader tinyply)
#Set dependencies inside folder
set_property(TARGET glfw glad glm imgui stb_image tiny_obj_loader tinyply PROPERTY FOLDER "deps")

# Choose if building demos directory. User-Defined.
if(GLSP_BUILD_EXAMPLES)
    add_subdirectory(${CMAKE_SOURCE_DIR}/examples)
    
endif()

#Where to
set_target_properties(GLSP PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug
)
