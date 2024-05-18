cmake_minimum_required(VERSION 3.10)

# set the project name
project(impulse VERSION 1.0)

# all files
file(GLOB impules_files
     "src/*.h"
     "src/*.cpp"
)

# add the executable
add_executable(impulse ${impules_files})

target_sources(impulse PRIVATE ${impules_files})
install(TARGETS impulse)