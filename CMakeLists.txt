cmake_minimum_required(VERSION 3.16...3.22)
project(cutefish-qt-plugins)

# 设置 Qt6 并启用标准项目配置
find_package(Qt6 REQUIRED COMPONENTS Core)
qt_standard_project_setup()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(platformtheme)
add_subdirectory(widgetstyle)