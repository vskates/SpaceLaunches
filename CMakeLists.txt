cmake_minimum_required(VERSION 3.0)
project(integrator VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

find_package(OpenGL REQUIRED)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")

file(GLOB SOURCES "src/*.cpp")
file(GLOB GUI_RENDER_SOURCES "src/gui_rendering/*.cpp")
file(GLOB GUI_SOURCES "src/imgui/*.cpp")

add_executable(main
    ${SOURCES}
    ${GUI_SOURCES}
    ${GUI_RENDER_SOURCES}
    glad/glad.c
)

target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/json
    ${CMAKE_CURRENT_SOURCE_DIR}/glad
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

target_link_libraries(main
    glfw
    OpenGL::GL
)

set(SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders")
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/shaders")
set(MODEL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/rendmodels")
set(MODEL_OUTPUT_DIR "${CMAKE_BINARY_DIR}/rendmodels")

function(copy_directory source_dir dest_dir)
    file(GLOB_RECURSE files RELATIVE ${source_dir} "${source_dir}/*")
    foreach(file ${files})
        get_filename_component(dir ${file} DIRECTORY)
        add_custom_command(
            TARGET main POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${dest_dir}/${dir}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${source_dir}/${file}" "${dest_dir}/${file}"
            COMMENT "Copying ${file} to ${dest_dir}/${dir}"
        )
    endforeach()
endfunction()

copy_directory(${MODEL_SOURCE_DIR} ${MODEL_OUTPUT_DIR})
copy_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/gui_style ${CMAKE_BINARY_DIR}/gui_style) # Добавлено для копирования папки gui_style



add_custom_command(
    TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADER_OUTPUT_DIR}
    COMMENT "Creating shaders directory in the build directory"
)


file(GLOB SHADERS "${SHADER_SOURCE_DIR}/*.vert" "${SHADER_SOURCE_DIR}/*.frag")
foreach(SHADER ${SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    add_custom_command(
        TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} "${SHADER_OUTPUT_DIR}/${SHADER_NAME}"
        COMMENT "Copying ${SHADER_NAME} to build directory"
    )
endforeach()