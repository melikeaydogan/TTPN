# TTPN

## Install CMake version 3.13.4 

Download the source file of the CMake https://cmake.org/files/v3.13/cmake-3.13.4-Linux-x86_64.tar.gz
or the command lines are

wget https://cmake.org/files/v3.13/cmake-3.13.4-Linux-x86_64.tar.gz

mkdir software

cd software

tar xvfz ../cmake-3.13.4-Linux-x86_64.tar.gz


In the directory software/cmake-3.13.4-Linux-x86_64/bin there are executables

![alt text](https://user-images.githubusercontent.com/70952816/93706122-e1485180-fb2b-11ea-84e0-77725ab70ad8.png)

The installation of cmake is done

## Install VTK 8.2.0

After downloading the soruce file of the VTK from the link https://www.vtk.org/files/release/8.2/VTK-8.2.0.tar.gz

mkdir projects

cd projects

tar xvfz ../VTK-8.2.0.tar.gz

mkdir VTK-build


Then with the cmake-gui VTK will be build. To make it, choose the source directory and build directory and choose the necessary libraries and configure them then generate it.

![alt text](https://user-images.githubusercontent.com/70952816/93706385-72b8c300-fb2e-11ea-8b84-39fab4dcccc1.png)

## Download QtCreator 5.14.2
Download the release from https://download.qt.io/official_releases/qt/5.14/5.14.2/

chmod +x qt-opensource-linux-x64-5.14.2.run

./qt-opensource-linux-x64-5.14.2.run

## Link VTK with Qt

Open CMake GUI and search for qt and choose all of the options and configure.

![alt text](https://user-images.githubusercontent.com/70952816/93706800-a5fd5100-fb32-11ea-9298-f43a8b81aff0.png)

![alt text](https://user-images.githubusercontent.com/70952816/93706787-8d8d3680-fb32-11ea-8f6f-e36369c919f5.png)

Qt5Core_DIR
Qt5Gui_DIR
Qt5PrintSupport_DIR
Qt5Sql_DIR
Qt5Widgets_DIR
VTK_DIR

tv14 CMakeLists.txt
-change VTK_DIR
