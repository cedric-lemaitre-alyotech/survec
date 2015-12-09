# Create a library called "SurvecLib" which includes the source file "Survec.cpp".
# The extension is already found. Any number of sources could be listed here.

##########################

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies (QtGui, QtXml and QtCore).
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5SerialPort REQUIRED)

# include directories
include_directories(${Qt5Widgets_INCLUDES})
include_directories(${Qt5Xml_INCLUDES})
include_directories(${Qt5SerialPort_INCLUDES})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Xml_DEFINITIONS})
add_definitions(${Qt5SerialPort_DEFINITIONS})
add_definitions(-DQT_PLUGIN)

# OpenCV
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/Debug/plugins")
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIRECTORY}/Release/plugins")

# SurvecAlarmPlugin as shared lib
SET (LIB_TYPE SHARED)

file (GLOB Sources *.cpp)
file (GLOB Headers *.h)

SET(CMAKE_DEBUG_POSTFIX _d)
add_library (SurvecAlarmPluginRS232 ${LIB_TYPE} ${Sources} ${Headers})

# Make sure the compiler can find include files for our Survec library
# when other libraries or executables link to SurvecLib
target_include_directories (SurvecLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(	SurvecAlarmPluginRS232
						LINK_PUBLIC SurvecLib 
						${Qt5Widgets_LIBRARIES} 
						${Qt5Xml_LIBRARIES} 
						${Qt5SerialPort_LIBRARIES} 
						${OpenCV_LIBS} 
					)

