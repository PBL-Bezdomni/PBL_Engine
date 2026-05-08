# CPM - package manager
include(CPM)

# OpenGL
find_package(OpenGL REQUIRED)

# glad
set(GLAD_DIR ${CMAKE_CURRENT_LIST_DIR}/glad)
set(glad_SOURCE_DIR ${GLAD_DIR}/include CACHE INTERNAL "")
add_library(glad STATIC ${GLAD_DIR}/src/glad.c)
target_include_directories(glad PRIVATE ${GLAD_DIR}/include)

# stb_image
set(STB_IMAGE_DIR ${CMAKE_CURRENT_LIST_DIR}/stb_image)
set(stb_image_SOURCE_DIR ${STB_IMAGE_DIR} CACHE INTERNAL "")
add_library(stb_image STATIC ${STB_IMAGE_DIR}/stb_image.cpp)
target_include_directories(stb_image PRIVATE ${STB_IMAGE_DIR})

# miniaudio
set(MINIAUDIO_DIR ${CMAKE_CURRENT_LIST_DIR}/miniaudio)
set(miniaudio_SOURCE_DIR ${MINIAUDIO_DIR} CACHE INTERNAL "")
add_library(miniaudio STATIC ${MINIAUDIO_DIR}/miniaudio.c)
target_include_directories(miniaudio PRIVATE ${MINIAUDIO_DIR})

# freetype
CPMAddPackage(
    NAME freetype
    URL "https://download.savannah.gnu.org/releases/freetype/freetype-2.13.2.tar.xz"
)

# other
CPMAddPackage("gh:assimp/assimp@5.4.3")
CPMAddPackage("gh:glfw/glfw#3.3.8")
CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:ocornut/imgui@1.88")
CPMAddPackage("gh:gabime/spdlog@1.10.0")
CPMAddPackage(
		NAME JoltPhysics
		GITHUB_REPOSITORY jrouwe/JoltPhysics
		VERSION 5.5.0
		SOURCE_SUBDIR Build
)
CPMAddPackage(
		NAME nlohmann_json
		GITHUB_REPOSITORY nlohmann/json
		VERSION 3.12.0
)

if(MSVC AND TARGET Jolt)
	set_property(TARGET Jolt PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

set(imgui_SOURCE_DIR ${imgui_SOURCE_DIR} CACHE INTERNAL "")
add_library(imgui STATIC ${imgui_SOURCE_DIR}/imgui.cpp
					     ${imgui_SOURCE_DIR}/imgui_demo.cpp
					     ${imgui_SOURCE_DIR}/imgui_draw.cpp
						 ${imgui_SOURCE_DIR}/imgui_tables.cpp
					     ${imgui_SOURCE_DIR}/imgui_widgets.cpp)
						 
set_target_properties(glad
                      stb_image
					  miniaudio
					  freetype
                      assimp 
                      glfw 
                      glm 
                      imgui 
                      spdlog PROPERTIES FOLDER "thirdparty")

if (TARGET zlibstatic)
    set_target_properties(zlibstatic PROPERTIES FOLDER "thirdparty")
endif()
