cmake_minimum_required(VERSION 3.5)

project(Imaging VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES Imaging_en_US.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        ${TS_FILES}
)

add_executable(Imaging
    ${PROJECT_SOURCES}
)

target_link_libraries(Imaging PRIVATE Qt6::Widgets)

# OpenCV Configuration
find_package(OpenCV REQUIRED)

target_include_directories(Imaging PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(Imaging PRIVATE ${OpenCV_LIBS})

# Tesseract Configuration
set(Tesseract_INCLUDE_DIRS "/opt/homebrew/Cellar/tesseract/5.3.1/include")
set(Tesseract_LIBRARIES "/opt/homebrew/Cellar/tesseract/5.3.1/lib/libtesseract.dylib")

target_include_directories(Imaging PRIVATE ${Tesseract_INCLUDE_DIRS})
target_link_libraries(Imaging PRIVATE ${Tesseract_LIBRARIES})


qt6_add_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})

set_target_properties(Imaging PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS Imaging
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
