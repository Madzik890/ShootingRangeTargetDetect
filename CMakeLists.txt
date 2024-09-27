# CMakeLists.txt

# CMake version that the current computer is using
cmake_minimum_required(VERSION "3.22")

# name of the project
project(shootingrange)

# Tell compiler to use C++ 14 features which is needed because
# Clang version is often behind in the XCode installation
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0)

# set working directory
set(SOURCE_DIR src)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# configure the necessary common CMake environment variables
# needed to include and link the OpenCV program into this
# demo project, namely OpenCV_INCLUDE_DIRS and OpenCV_LIBS
find_package(OpenCV REQUIRED)

# tell the build to include the headers from OpenCV
include_directories(${OpenCV_INCLUDE_DIRS} ${SOURCE_DIR})

# specify the executable target to be built
add_executable(main 
                    src/main.cpp
                    src/pipeline.cpp
                    src/modules/prepareimage.cpp
                    src/modules/targetdetect.cpp
                    src/modules/shottracking.cpp)

# tell it to link the executable target against OpenCV
target_link_libraries(main ${OpenCV_LIBS})