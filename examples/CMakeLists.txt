
#Simple Example
add_executable(simple_example  simple-example/main.cpp simple-example/example.cpp simple-example/example.h)
# Link project against GLSP
target_link_libraries(simple_example PRIVATE GLSP)
target_compile_definitions(simple_example PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")

#Complex Example
file(GLOB APP_SOURCES
"complex-example/*.cpp"
"complex-example/*.h"
)
add_executable(complex_example  ${APP_SOURCES})
# Link project against GLSP
target_link_libraries(complex_example PRIVATE GLSP)
target_compile_definitions(complex_example PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/")

set_property(TARGET simple_example complex_example PROPERTY FOLDER "examples")