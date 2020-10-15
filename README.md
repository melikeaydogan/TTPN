# TTPN User Manuel

## TTPN
Token Tracking Petri Net (TTPN) is a GUI to visualize activity detection.

## Installation Manuel

### Install CMake version 3.13.4 

> wget http://www.cmake.org/files/v3.13/cmake-3.13.4.tar.gz

> tar xzf cmake-3.13.4.tar.gz

> cd cmake-3.13.4

> ./configure --prefix=/home/aydogan/cmake

> make

> make install

### Install zlib

> wget http://www.zlib.net/zlib-1.2.11.tar.gz

> tar -xvzf zlib-1.2.11.tar.gz

> cd zlib-1.2.11

> ./configure --prefix=/usr/local/zlib

> sudo make install

### Install curl

> sudo apt install curl

### Install hdf5

Download hdf5-1.12.0.tar.gz from https://www.hdfgroup.org/downloads/hdf5/source-code/

> tar -xvf hdf5-1.12.0.tar.gz

> cd hdf5-1.12.0

> ./configure --prefix=$fld_install --with-zlib=$fld_install

> make install


### Install netcdf-c library

> git clone http://github.com/Unidata/netcdf-c netcdf-c

> mkdir netcdf-build

> cd netcdf-build

> cmake ../netcdf-c

### Download QtCreator 5.14.2
Download the release from https://download.qt.io/official_releases/qt/5.14/5.14.2/

> chmod +x qt-opensource-linux-x64-5.14.2.run

> ./qt-opensource-linux-x64-5.14.2.run

### Install VTK 8.2.0

After downloading the source file of the VTK from the link https://www.vtk.org/files/release/8.2/VTK-8.2.0.tar.gz

> mkdir projects

> cd projects

> wget https://www.vtk.org/files/release/8.2/VTK-8.2.0.tar.gz

> tar xvfz VTK-8.2.0.tar.gz

> mkdir VTK-build

> cd VTK-build

> ccmake ../VTK-8.2.0

With last command line cmake window will open.

![alt text](https://user-images.githubusercontent.com/70952816/95137242-55136c80-0770-11eb-9af0-651f87d83352.png)

#### Link VTK with Qt


Press c to configure then make VTKGroup_Qt ON and configure it again.

![alt text](https://user-images.githubusercontent.com/70952816/95137239-53e23f80-0770-11eb-9937-07bffacbb806.png)

You will get error and it wil be solved once you change the direction of the Qt, Qt5_DIR, and after this change configure it.

![alt text](https://user-images.githubusercontent.com/70952816/95137237-53e23f80-0770-11eb-8f26-20bb8f935fb3.png)

![alt text](https://user-images.githubusercontent.com/70952816/95137233-52b11280-0770-11eb-94e4-56dd824dd1ff.png)

Directions of other Qt are be filled automatically and another configuration is needed.

![alt text](https://user-images.githubusercontent.com/70952816/95137232-52187c00-0770-11eb-8a48-7e58c042dfd6.png)

![alt text](https://user-images.githubusercontent.com/70952816/95137229-517fe580-0770-11eb-9228-e37b5888247a.png)

After generating the build with g, then VTK is ready.

## Download tv14 and make changes on the code

1. In the mainwindow.cpp, to get images on the TTPN, change the places with "waytottpncode" with your direction to the file of TTPN code.

2. In the CMakeLists.txt, change the VTK_DIR in line 6.

## How to open TTPN code with QtCreator

1. Choose Open & choose the CMakeLists.txt. Then The code will be shown like that

![alt text](https://user-images.githubusercontent.com/70952816/96138259-3f3b3f80-0f06-11eb-8b14-b193c6e0928d.png)

2. From the left bottom buttons, run the code then the TTPN window will appear.

![alt text](https://user-images.githubusercontent.com/70952816/96138245-3d717c00-0f06-11eb-89fd-a750438b1183.png)


