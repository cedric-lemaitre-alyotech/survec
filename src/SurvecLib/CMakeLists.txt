# Create a library called "SurvecLib" which includes the source file "Survec.cpp".
# The extension is already found. Any number of sources could be listed here.

# Tesseract package
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

SET(Tesseract_INCLUDE_DIR $ENV{Tesseract_INCLUDE_DIR})
SET(Leptonica_INCLUDE_DIR $ENV{Leptonica_INCLUDE_DIR})
SET(Tesseract_LIB_DIR $ENV{Tesseract_LIB_DIR})
SET(Leptonica_LIB_DIR $ENV{Leptonica_LIB_DIR})

find_package(Tesseract)
if(Tesseract_FOUND)
    message(STATUS "Tesseract:   YES")
    set(HAVE_TESSERACT 1)
else()
    message(STATUS "Tesseract:   NO")
endif()

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

# OpenCV
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

#################
# TESSERACT
if(HAVE_TESSERACT)
	include_directories( ${Tesseract_INCLUDE_DIR} )
	include_directories( ${Leptonica_INCLUDE_DIR} )	
	set(Tesseract_LIB_DEBUG ${Tesseract_LIB_DIR}/${TESSERACT_ARCH}/DLL_Debug/libtesseract${TESSERACT_VERSION}d.lib)
	set(Tesseract_LIB_RELEASE ${Tesseract_LIB_DIR}/${TESSERACT_ARCH}/DLL_Release/libtesseract${TESSERACT_VERSION}.lib)
	set(Leptonica_LIB_DEBUG ${Leptonica_LIB_DIR}/${TESSERACT_ARCH}/liblept${LEPTONICA_VERSION}d.lib)
	set(Leptonica_LIB_RELEASE ${Leptonica_LIB_DIR}/${TESSERACT_ARCH}/liblept${LEPTONICA_VERSION}.lib)	
endif()

#################

# SurvecLib as shared lib
SET (LIB_TYPE SHARED)

file (GLOB Sources *.cpp)
file (GLOB Headers *.h)

SET(CMAKE_DEBUG_POSTFIX _d)
add_library (SurvecLib ${LIB_TYPE} ${Sources} ${Headers})

# Make sure the compiler can find include files for our Survec library
# when other libraries or executables link to SurvecLib
target_include_directories (SurvecLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(	SurvecLib 
						${Qt5Widgets_LIBRARIES} 
						${Qt5Xml_LIBRARIES} 
						${Qt5SerialPort_LIBRARIES} 
						${OpenCV_LIBS} 
						debug ${Tesseract_LIB_DEBUG} optimized ${Tesseract_LIB_RELEASE}
						debug ${Leptonica_LIB_DEBUG} optimized ${Leptonica_LIB_RELEASE}
					)

