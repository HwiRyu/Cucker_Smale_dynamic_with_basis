cmake_minimum_required(VERSION 3.27)
project(Cucker_Smale_modeling)

set(CMAKE_CXX_STANDARD 17)

# Find the SFML package
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

# Add the executable
add_executable(Cucker_Smale_modeling main.cpp
        scale.h
        rotation.h
        draw.h
        function.h
        axes.h
        fix.h
        cucker_smale_modeling.h
)

# Link SFML libraries
target_link_libraries(Cucker_Smale_modeling sfml-system sfml-window sfml-graphics sfml-network sfml-audio)