project(VampSolution)

file(GLOB_RECURSE Vamp_Sources "Src/*.cpp")
file(GLOB_RECURSE Vamp_Headers "Src/*.h")

set (CMAKE_CXX_STANDARD 17)

add_executable(VampGame ${Vamp_Headers} ${Vamp_Sources})
target_include_directories(VampGame PUBLIC "src/imgui/" "Libs/stb" "H:/CPPProjects/Chipmunk2D/include")
target_link_libraries(VampGame "d3d11.lib" "d3dcompiler.lib" "H:/CPPProjects/Chipmunk2D/msvc/VS2015/chipmunk/x64/Debug/chipmunk.lib")