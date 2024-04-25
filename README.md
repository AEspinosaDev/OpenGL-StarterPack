# OpenGL-StarterPack (GLSP) 📦

OpenGL-StarterPack (GLSP) is a lightweight open source OpenGL based framework that facilitates and speeds up project creation by offering a thin and flexible abstraction to the basic objects of OpenGL.
 
GLSP is simple and aims for simplicity, adding only a light layer of abstraction to OpenGL. All its classes are designed to be easy inherited and converted into more complex objects, in case they are used as a base for more serious applications. 

Finally, GLSP comes with the main lightweight libraries used in graphics already included and integrated, to offer the user their functionality from the start.

The philosophy of GLSP is based on inheritance. It provides a simple skeleton that can be easily modified so that the user can quickly achieve their goals. Thanks to this, it is also perfect for learning purposes ! 

<div align="center">
  <img src="https://github.com/AEspinosaDev/OpenGL-StarterPack/assets/79087129/9c16d64f-7fed-4810-a17f-e48ac5182dbf" alt="image" width="50%"/>
  <img src="https://github.com/AEspinosaDev/OpenGL-StarterPack/assets/79087129/13c7bb32-82ed-4210-a72b-022185a60339" alt="image" width="49%"/>
</div>

## Table of Contents 🗃️
1. [Building](#main-data-classes)
2. [Project Integration](#main-data-classes)
3. [Usage](#main-data-classes)
4. [Main Data Classes](#main-data-classes)
5. [Integrated Libraries](#main-data-classes)




## Building 🛠️

The prequisites for using this code are:

- Windows 10, 11, Linux.
- CMake installed.

1. Clone the repo:
   ```bash
   git clone https://github.com/AEspinosaDev/OpenGL-StarterPack.git
   cd OpenGL-StarterPack
   ```
2. Build with CMake:
   ```bash
   mkdir build
   cd build
   cmake ..
   ```
The project is configured in such a way that, during the build process, CMake takes care of automatically locating and linking all dependencies on the system. Although the project has been implemented in Visual Studio Code, a practical file structure has been configured for CMake in case it is opened in Visual Studio.

The project compiles dependencies, the library itself, and the example applications directory, which statically links against the 3D library. The library is a STATIC lib, do not try to link dynamically against it.

3. Building of the examples directory is optional, and can be turned off in CMake:
```bash
cmake -DBUILD_EXAMPLES=OFF /path/to/source
```

## Project Integration ⚙️

Integration of GLSP into your own personal project is quite easy. If working with CMake, GLSP should be inserted inside the dependencies folder of your project root directory.

On your main project CMakeLists.txt, write:

```cmake

cmake_minimum_required(VERSION 3.16)

project(JohnDoe VERSION 1.0.0)

# Add GLSP subdirectory ! Set if you want to build the examples directory ...
set(GLSP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/OpenGL-StarterPack)

#Setup project own source code (User-Defined)
file(GLOB APP_SOURCES "src/*.cpp")
file(GLOB APP_HEADERS "include/*.h")
add_executable(JohnDoe ${APP_SOURCES} ${APP_HEADERS})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)

# Link project against GLSP
target_link_libraries(JohnDoe PRIVATE GLSP)

....

```

Your project structure should be somewhat like this one:

```
project_root/
│
├── resources/
│   ├── shaders
│   ├── textures
│   └── meshes
│
├── dependencies/
│   ├── OpenGL-StarterPack
│   └── another-cool-dependencies
│
├── src/
│   ├── main.cpp
│   └── compilation_file1.cpp
│
├── include/
│   ├── header1.h
│   └── header2.h
│
└── CMakeLists.txt
````



If you are not working with CMake, sorry, but it is time for you to know... 
**you should be !!** 
Nah, just kidding 🙄 you can check how to include GLSL as a dependency in your IDE of choice. With luck, your IDE should not be difficult to handle.



## Usage ✨

I encourage you to check the examples directory if you want to see how to use this framework.

- The simple example can serve as a quite good starting point for any project.
- Complex example shows how to work with a more complex OpenGL (different shader stages, compute, uniform buffers, user defined geometry)

As told in introduction, GLSP encourages you to take all that it has in order for you to modify it and turn it into yout own purposes.

## Main Data Classes 🧩

- **Renderer**: A simple class that implements all the functions associated with a renderer (creation and control of the graphics context, flow control, drawing loops, and updates). It is imperative that it be inherited and modified to create more complex and robust applications.

- **Buffer**: Wrappers of VAOs, VBOs, IBOs, and UBOs that allow the user to have complete control over their creation and use.

- **Camera**: A class that implements all the basic functionalities that a virtual camera should have.

- **Controller**: A class that serves as an interface between the spatial movement of 3D objects and user input. Useful for controlling the camera, for example.

- **Framebuffer**: Practically and flexibly abstracts FBOs and RBOs.

- **Loaders**: A namespace that offers some resource loading functions (.obj, .ply, images).

- **Mesh**: Abstracts and represents a mesh, composed, among others, of a geometry class and a material class. Inherits from Object3D, the base class that represents any object capable of being placed in a virtual world, characterized by its transformation.

- **Shader**: Abstracts the functionality of OpenGL programs and the compilation of the different stages of the graphics and compute pipeline. GLSP shaders allow implementing all the functionality of the graphics pipeline in a single .glsl file, marking each part with a hashtag followed by the word "stage" and the name of the graphics stage (vertex, control, eval, geometry, fragment). It also allows separate files.

- **Texture**: Abstracts the functionality of OpenGL samplers and images. It is flexible, and through its configuration, all types of textures can be created.

- **Widgets**: A namespace that offers some functions to create utility widgets based on ImGui.

- **Material**: A very basic material class that abstracts and combines the concepts of the graphics pipeline, its handling, and object-level state control. For simple projects, it is not necessary, and its modification and inheritance are recommended to create more secure and solid materials in more serious projects.

It is also a good starting point for anyone trying to learn computer graphics or a simple graphics API such as OpenGL, as its thin abstraction facilitates the comprehension of OpenGL workarounds.


## Integrated Libraries 📚

GLSP has already done the work of linking and integrating some useful libraries that any render application should have:

* stb_image : for loading lots of kinds of image files 🖼️.
* tinyply : for loading .ply files 🪩.
* tiny_obj_loader : for loading .obj files 🪩.
* imgui : for having some user interface functionality 📱.
* glm : for doing some maths 🤓.
* glfw : because if not, there is no window and no context 😭.
* glad : to give you access to OpenGL, as its integrated in you hardware ! 🔑



