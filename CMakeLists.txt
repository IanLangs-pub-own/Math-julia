cmake_minimum_required(VERSION 3.10)

# ------------------------
# Proyecto
# ------------------------
project(MathJulia LANGUAGES CXX)

# ------------------------
# Estándar C++
# ------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------
# Directorios
# ------------------------
include_directories(${PROJECT_SOURCE_DIR}/include)

# ------------------------
# Fuente
# ------------------------
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cc")

# ------------------------
# Ejecutables y flags
# ------------------------
# Carpeta para todos los ejecutables
set(EXEC_DIR "${PROJECT_SOURCE_DIR}/bin")
file(MAKE_DIRECTORY ${EXEC_DIR})

# Versión estable
add_executable(mjl ${SOURCES})
set_target_properties(mjl PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXEC_DIR}
)
target_compile_options(mjl PRIVATE -O2 -Wall -Wextra -Wpedantic)

# Versión nightly
add_executable(mjl-nightly ${SOURCES})
set_target_properties(mjl-nightly PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${EXEC_DIR}
)
target_compile_options(mjl-nightly PRIVATE -O0 -g -Wall -Wextra -Wpedantic -DNIGHTLY)
