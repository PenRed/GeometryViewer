cmake_minimum_required(VERSION 3.5)

project(GeometryViewer VERSION 0.1 LANGUAGES CXX)

option(BUILD_VIEW_SHARED_LIB "Build PenRed shared geometry view lib" ON)

if(BUILD_VIEW_SHARED_LIB)
    include(ExternalProject)
    find_package(Git REQUIRED)

    ExternalProject_Add(
        PenRed
        PREFIX "${CMAKE_SOURCE_DIR}/PenRed"
        GIT_REPOSITORY "https://github.com/PenRed/PenRed"
        GIT_TAG "master"
        SOURCE_DIR "${CMAKE_SOURCE_DIR}/PenRed/repo"
        BINARY_DIR "${CMAKE_SOURCE_DIR}/PenRed"
        SOURCE_SUBDIR "src"
        CMAKE_ARGS -DBUILD_C_BINDS=ON -DWITH_NATIVE=ON -DWITH_DICOM=OFF -DWITH_MULTI_THREADING=ON -DWITH_MPI=OFF -DWITH_LB=OFF -DDEVELOPMENT_WARNINGS=OFF
    )

    install(
    FILES "${CMAKE_SOURCE_DIR}/PenRed/repo/src/bindings/C/viewers/geometry/libgeoView_C*"
    DESTINATION .
    )
endif(BUILD_VIEW_SHARED_LIB)

if (MSVC) 
  # We need a stack size of 8MB (as it is default on linux)
  # See https://github.com/open62541/open62541/issues/1326
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:8388608")
endif(MSVC)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        textconfig.h
        textconfig.cpp
        viewer.cpp
        viewer.h
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        pen_geoViewInterface.hh
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GeometryViewer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GeometryViewer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(GeometryViewer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GeometryViewer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(GeometryViewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(GeometryViewer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(BUILD_VIEW_SHARED_LIB)
    add_custom_command(
            TARGET ${PROJECT_NAME}  POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_SOURCE_DIR}/PenRed/repo/src/bindings/C/viewers/geometry/libgeoView_C*
                    ${CMAKE_CURRENT_BINARY_DIR})
endif(BUILD_VIEW_SHARED_LIB)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GeometryViewer)
endif()
