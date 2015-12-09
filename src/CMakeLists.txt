# CMakeLists files in this project can
# refer to the root source directory of the project as ${SURVEC_SOURCE_DIR} and
# to the root binary directory of the project as ${SURVEC_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)

project(Survec)

add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE) 

set (OUTPUT_DIRECTORY "../Bin")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/Debug")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}/Release")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/Release")
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}/Debug")

# Recurse into the "Survec" and "Demo" subdirectories. This does not actually
# cause another cmake executable to run. The same process will walk through
# the project's entire directory structure.
add_subdirectory (SurvecLib)
add_subdirectory (Survec)
add_subdirectory (SurvecAlarmPluginRS232)

add_subdirectory (TextDisplay)