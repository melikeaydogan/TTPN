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

### Install hdf5

sudo apt-get install libhdf5-serial-dev

### Install netcdf library

https://www.unidata.ucar.edu/software/netcdf/docs/getting_and_building_netcdf.html

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

Open CMake GUI and search for qt and choose all of the options and configure.

![alt text](https://user-images.githubusercontent.com/70952816/95137240-547ad600-0770-11eb-9bc9-0e4c5e403ea8.png)

![alt text](https://user-images.githubusercontent.com/70952816/95137239-53e23f80-0770-11eb-9937-07bffacbb806.png)

![alt text](https://user-images.githubusercontent.com/70952816/95137237-53e23f80-0770-11eb-8f26-20bb8f935fb3.png)

![alt text](https://user-images.githubusercontent.com/70952816/95137236-5349a900-0770-11eb-8ad9-d0a4904d77b2.png)

![alt text](https://user-images.githubusercontent.com/70952816/95137234-5349a900-0770-11eb-9a20-06dfa71e3ae7.png)

![alt text](https://user-images.githubusercontent.com/70952816/95137233-52b11280-0770-11eb-94e4-56dd824dd1ff.png)

![alt text](https://user-images.githubusercontent.com/70952816/95137232-52187c00-0770-11eb-8a48-7e58c042dfd6.png)

![alt text](https://user-images.githubusercontent.com/70952816/95137229-517fe580-0770-11eb-9228-e37b5888247a.png)

Qt5Core_DIR
Qt5Gui_DIR
Qt5PrintSupport_DIR
Qt5Sql_DIR
Qt5Widgets_DIR
VTK_DIR

tv14 CMakeLists.txt
-change VTK_DIR
