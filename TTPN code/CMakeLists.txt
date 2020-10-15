cmake_minimum_required(VERSION 2.6)

PROJECT(tv14)
#SET (CMAKE_BUILD_TYPE Release)

set (VTK_DIR "/home/aydogan/projects/VTK-build")

# ----------------------Migration of QT5-----------------------
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies (QtGui and QtCore).
find_package(Qt5Widgets REQUIRED)

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS})

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
# -------------------------------------------------------------

IF(NOT VTK_BINARY_DIR)
FIND_PACKAGE(VTK)
IF(NOT VTK_DIR)
  MESSAGE(FATAL_ERROR "Please set VTK_DIR.")
ENDIF(NOT VTK_DIR)
INCLUDE(${VTK_USE_FILE})
ENDIF(NOT VTK_BINARY_DIR)


# use what QVTK built with
SET(QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "")
SET(QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "")
SET(QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "")
find_package(Qt5 REQUIRED COMPONENTS Core Widgets PrintSupport)
#FIND_PACKAGE(Qt5Widgets)
#FIND_PACKAGE(Qt5Declarative)
#INCLUDE(${QT_USE_FILE})


# Use the include path and library for Qt that is used by VTK.
INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set your files and resources here  
SET(tv14Srcs main.cpp arrow.cpp diagramitem.cpp diagramscene.cpp diagramtextitem.cpp itemtable.cpp barchart.cpp nettable.cpp datatable.cpp polybrowser.cpp polyreader.cpp sfdfiledialog.cpp token.cpp chartwindow.cpp polyfilteringreader.cpp myvtkChartXY.cpp multiviewwindow.cpp objectreader.cpp tdtable.cpp mainwindow.cpp cellinfo.cpp nodeinfo.cpp objectinfo.cpp parser.cpp box.cpp volrenderwindow.cpp)
SET(tv14Headers arrow.h diagramitem.h diagramscene.h diagramtextitem.h itemtable.h nettable.h datatable.h polybrowser.h polyreader.h sfdfiledialog.h token.h barchart.h chartwindow.h polyfilteringreader.h myvtkChartXY.h vtkVectorOperators.h multiviewwindow.h structs.h objectreader.h tdtable.h mainwindow.h cellinfo.h input.h interfaceutil.h nodeinfo.h objectinfo.h ObjectSegment.h objSegmentUtil.h parser.h stobject1.h stRGB.h util.h PN.h interactorstyle.h box.h volrenderwindow.h)
SET(tv14Resources tv14.qrc)

# The rest should just work (sure...)
QT5_WRAP_CPP(MOCSrcs ${tv14Headers} )
QT5_ADD_RESOURCES(ResourceSrcs ${tv14Resources})



SOURCE_GROUP("Resources" FILES
  ${tv14Resources}
  ${EXE_ICON}
)


SOURCE_GROUP("Generated" FILES
  ${UISrcs}
  ${MOCSrcs}
  ${ResourceSrcs}
  ${RCS_SOURCES}
)

ADD_DEFINITIONS(-DQT_GUI_LIBS -DQT_CORE_LIB -DQT3_SUPPORT)
SET_SOURCE_FILES_PROPERTIES(${tv14Srcs} PROPERTIES)


ADD_EXECUTABLE( tv14 MACOSX_BUNDLE ${tv14Srcs} ${tv14Resources} ${MOCSrcs} ${ResourceSrcs} tv14.qrc)

TARGET_LINK_LIBRARIES( tv14
  vtkGUISupportQt
  Qt5::Widgets
  Qt5::PrintSupport
  ${QT_LIBRARIES}
  ${PCL_LIBRARIES}
  ${VTK_LIBRARIES}
)



