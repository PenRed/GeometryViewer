# GeometryViewer
Geometry viewer for PenRed compatible geometries

## Installation

This section explains how to install and run the PenRed geometry viewer. This can be done compiling the source code or using a prebuit binary.

### Viewer Source Compilation

The geometry viewer has been developed using the QT libraries via the QT creator ID. Therefore, the source code can be built as a QT project using QT Creator. Notice that the viewer requires a shared library which includes a interface to access the geometries available in the PenRed package. By default, this library is compiled automatically when the QT project is built, using the master branch of the PenRed repository. However it can be disabled via a CMake option named *BUILD_VIEW_SHARED_LIB*. If it is disabled, the library must be compiled from the PenRed source code, as is described in the next section.

### Shared Library Compilation

The viewer itself is isolated from the PenRed package to achieve compatibility with the geometry viewer regardless the PenRed version. This is done loading a shared library which includes a interface to access all the available geometry modules in the PenRed package. However, this shared library, named *libgeoView_C*, must be compiled and stored in the same folder as the viewer executable.

The library can be built using the PenRed package with the apropiate CMake flags. The flag that must be enabled is the *BUILD_C_BINDS*. For example, to build it in the *src* folder of the PenRed package, we can run the following commands

```
mkdir build
cd build
cmake -DBUILD_C_BINDS=ON -DWITH_NATIVE=ON -DWITH_DICOM=OFF -DWITH_MULTI_THREADING=ON -DWITH_MPI=OFF -DWITH_LB=OFF -DDEVELOPMENT_WARNINGS=OFF ../
make install
cd ..
```

Once the build is completed, the library will be stored in the folder 

*/src/bindings/C/viewers/geometry/*  

inside the PenRed package, with a different extension depending on the OS. For example, in linux systems, the library is named *libgeoView_C.so*. Notice that, with the previous *cmake* command, the library is compiled with no DICOM support. Therefore, the viewer will not be able to show DICOM geometries. To enable DICOM geometries, the DICOM flag must be enabled and the corresponding dependencies installed.

### Pre-built Executable Files

Another option to use the viewer is downloading the already built packages, which include the executable file, the compiled shared library, a script to run the viewer, depending on the OS, and the required QT libraries and other dependencies to be able to run the viewer without a QT instalation. These bundles can be found in the releases provided in this repository.

### Viewer Usage

The usage of the viewer will be described in a user manual, which is under development.
