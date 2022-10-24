# GeometryViewer
Geometry viewer for PenRed compatible geometries

## Installation

In this section several methods to obtain and run the viewer are described.

### Source Compilation

This geometry viewer has been developed using the QT libraries via the QT creator ID. Therefore, the source can be loaded and built as a QT project using QT Creator. Notice that the viewer requires a shared library which includes a interface to access the geometries available in the PenRed package. This library is compiled automatically by default when the QT project is built, using the master branch of the PenRed repository. However it can be disabled inside the *CMakeLists.txt* file via the *BUILD_VIEW_SHARED_LIB* option. In this case, the library must be compiled manually, as is described in the next section.

### Shared Library Compilation

The viewer itself is isolated from the PenRed package to allow compatibility between the geometry viewer regardless the PenRed version. This is archieved loading a shared library which includes a interface to access all the available geometry modules in the PenRed package. However, this shared library, named *libgeoView_C* must be compiled and stored in the same folder as the viewer executable.

That library can be built within the PenRed package using the apropiate CMake flags. The flag that must be enabled is the *BUILD_C_BINDS*. For example, to build it, in the *src* file of the PenRed package run the following commands

```
mkdir build
cd build
cmake -DBUILD_C_BINDS=ON -DWITH_NATIVE=ON -DWITH_DICOM=OFF -DWITH_MULTI_THREADING=ON -DWITH_MPI=OFF -DWITH_LB=OFF -DDEVELOPMENT_WARNINGS=OFF ../
make install
cd ..
```

After the build is complete, the library should be created in the folder 

*/src/bindings/C/viewers/geometry/*  

inside the PenRed package, with a different extension depending on the OS. For example, in linux systems, the library is named *libgeoView_C.so*. Notice that, with the previous *cmake* command, the library is compiled with no DICOM support. Therefore, the viewer will not be able to show DICOM geometries. To enable DICOM geometries, the DICOM flag must be enabled and the corresponding dependencies installed.

### Pre-built Executable Files

*In progress* 
