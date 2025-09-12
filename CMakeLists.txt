# CMakeList.txt : CMake project for CMakeOpenGL, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("CMakeOpenGL")

# Add source to this project's executable.
add_executable (CMakeOpenGL
"Main.cpp"
"dependencies/glad/glad.c"
"Application.cpp" "Application.h"
"Callbackable.h"
"CallbackManager.h"
"Camera.cpp" "Camera.h"
"Cubemap.cpp" "Cubemap.h"
"EntityComponents.h"
"ErrorManager.cpp" "ErrorManager.h"
"Field.cpp" "Field.h"
"Font.cpp" "Font.h"
"FunctionManager.cpp" "FunctionManager.h"
"GLEnums.h"
"GLFWindowHolder.h"
"Initializable.h"
"InstancedModel.cpp" "InstancedModel.h"
"Interface.cpp" "Interface.h"
"Lights.h"
"Logger.h"
"Mesh.cpp" "Mesh.h"
"Model.cpp" "Model.h"
"NotCopyable.cpp" "NotCopyable.h"
"ResourceBatch.cpp" "ResourceBatch.h"
"ResourceHolders.cpp" "ResourceHolders.h"
"ResourceManager.cpp" "ResourceManager.h"
"ResourceOwner.cpp" "ResourceOwner.h"
"Scene.cpp" "Scene.h"
"SelectedFieldInfo.cpp" "SelectedFieldInfo.h"
"SetBatch.h"
"Setup.cpp"
"Shader.cpp" "Shader.h"
"SmartEnum.h"
"Stage.cpp" "Stage.h"
"TestGame.cpp" "TestGame.h"
"Text.cpp" "Text.h"
"Texture.cpp" "Texture.h"
"ThreadPool.cpp" "ThreadPool.h"
"Timer.cpp" "Timer.h"
"Utils.h"
"VertexGroup.cpp" "VertexGroup.h"
"VertexHelper.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET CMakeOpenGL PROPERTY CXX_STANDARD 20)
endif()

find_package(glm CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

target_link_libraries(CMakeOpenGL PRIVATE glm::glm-header-only)
target_link_libraries(CMakeOpenGL PRIVATE Freetype::Freetype)
target_link_libraries(CMakeOpenGL PRIVATE glfw)
target_link_libraries(CMakeOpenGL PRIVATE assimp::assimp)

target_include_directories(CMakeOpenGL PRIVATE dependencies/glad)
target_include_directories(CMakeOpenGL PRIVATE dependencies/nlohmann_json)
target_include_directories(CMakeOpenGL PRIVATE dependencies/prettyprint)
target_include_directories(CMakeOpenGL PRIVATE dependencies/stb_image)
target_include_directories(CMakeOpenGL PRIVATE dependencies/tinygltf)