# TTPN

Install CMake version 3.13.4

Download the source file of the CMake https://cmake.org/files/v3.13/cmake-3.13.4-Linux-x86_64.tar.gz
or the command lines are

wget https://cmake.org/files/v3.13/cmake-3.13.4-Linux-x86_64.tar.gz
mkdir software
cd software
tar xvfz ../cmake-3.13.4-Linux-x86_64.tar.gz

In the software/cmake-3.13.4-Linux-x86_64/bin there are executables

![alt text](link)


https://cmake.org/install/

![alt text](link)



Download QtCreator 5.14.2
https://www.qt.io/download

Install VTK 8.2.0
https://vtk.org/Wiki/VTK/Configure_and_Build

then link VTK with Qt

Open CMake GUI

Qt5Core_DIR
Qt5Gui_DIR
Qt5PrintSupport_DIR
Qt5Sql_DIR
Qt5Widgets_DIR
VTK_DIR

tv14 CMakeLists.txt
-change VTK_DIR
