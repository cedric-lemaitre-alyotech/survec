# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions( "/D_CRT_SECURE_NO_WARNINGS" )

# Widgets finds its own dependencies (QtGui, QtCore and QtXml).
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5SerialPort REQUIRED)
find_package(Qt5LinguistTools)

# include directories
include_directories(${Qt5Widgets_INCLUDES})
include_directories(${Qt5Xml_INCLUDES})
include_directories(${Qt5SerialPort_INCLUDES})
#include_directories(${PROJECT_BINARY_DIR}/SurvecLib)


# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS})

qt5_wrap_ui(UI_HEADERS 
	MainWindow.ui
	ColorEditWidget.ui
	ROIConfigurationWidget.ui
	cwROIResult.ui
	cwVideoPlayer.ui
	cwDisplayAlarmImg.ui
)
qt5_add_resources(UI_RESOURCES resources.qrc)

# OpenCV
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set(CPP_FILES 	ColorEditWidget.cpp 
				CustomRectItem.cpp
				cwROIResult.cpp
				cwVideoPlayer.cpp
				cwView.cpp
				main.cpp
				MainWindow.cpp
				ROIConfigurationWidget.cpp
				Scene.cpp
				cwDisplayAlarmImg.cpp
				Survec.rc)
				
set(HEADER_FILES 	ColorEditWidget.h
					CustomRectItem.h
					cwROIResult.h
					cwVideoPlayer.h
					cwView.h
					MainWindow.h
					ROIConfigurationWidget.h
					Scene.h
					cwDisplayAlarmImg.h)

set(UI_FILES 	ColorEditWidget.ui
				cwROIResult.ui
				cwVideoPlayer.ui
				MainWindow.ui
				ROIConfigurationWidget.ui
				cwDisplayAlarmImg.ui)
					

# languages (qt linguist)

set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)
set(TRANSLATION_FILES ${CMAKE_CURRENT_LIST_DIR}/translations/survec_fr.ts)
set(FILES_TO_TRANSLATE ${CPP_FILES} ${UI_FILES} ${HEADER_FILES})
qt5_create_translation(QM_FILES ${FILES_TO_TRANSLATE} ${TRANSLATION_FILES})
 			
##add_custom_target(translations_update DEPENDS ${QM_FILES})
add_custom_target(translations DEPENDS ${QM_FILES})
 
install(FILES ${QM_FILES} DESTINATION ${CMAKE_CURRENT_LIST_DIR}/translations/)

add_executable (Survec WIN32 ${UI_HEADERS} ${UI_RESOURCES} ${CPP_FILES} ${HEADER_FILES})

# Link the executable to the Survec library. Since the Survec library has
# public include directories we will use those link directories when building
# Survec
target_link_libraries (Survec LINK_PUBLIC SurvecLib ${Qt5Widgets_LIBRARIES} ${Qt5Xml_LIBRARIES} ${Qt5SerialPort_LIBRARIES} ${OpenCV_LIBS})