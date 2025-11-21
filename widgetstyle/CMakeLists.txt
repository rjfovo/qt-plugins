cmake_minimum_required(VERSION 3.16...3.22)
project(cutefishstyle)

set(TARGET cutefishstyle)
set(CMAKE_CXX_STANDARD 17)

# 启用 PkgConfig 支持
find_package(PkgConfig REQUIRED)

# 分开查找 Qt6 模块
find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Gui REQUIRED)
find_package(Qt6DBus REQUIRED)

# 分开查找 KDE Frameworks 6
find_package(KF6WindowSystem REQUIRED)

# 或者使用 PkgConfig 作为备选方案
pkg_check_modules(KF6WindowSystem IMPORTED_TARGET kf6-windowsystem)
if(KF6WindowSystem_FOUND)
    message(STATUS "Found KF6WindowSystem via PkgConfig")
endif()

set (SRCS
    blurhelper.cpp
    blurhelper.h
    pstyleplugin.cpp
    pstyleplugin.h
    basestyle.h
    basestyle.cpp
    phantomcolor.h
    phantomcolor.cpp
    shadowhelper.h
    shadowhelper.cpp
    tileset.h
    tileset.cpp
    boxshadowrenderer.h
    boxshadowrenderer.cpp
)

add_library(${TARGET} MODULE ${SRCS})

target_link_libraries(${TARGET}
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::DBus
    Qt6::CorePrivate  # 这会自动处理私有头文件路径
    Qt6::GuiPrivate   # 这会自动处理私有头文件路径
    KF6::WindowSystem
)

# 如果 PkgConfig 找到了库，也链接它
if(KF6WindowSystem_FOUND)
    target_link_libraries(${TARGET} PkgConfig::KF6WindowSystem)
endif()

# 设置包含目录
target_include_directories(${TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 设置插件属性
set_target_properties(${TARGET} PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/styles"
)

# 使用 Qt6 的标准插件安装路径
install(TARGETS ${TARGET} 
    DESTINATION "${QT6_PLUGINS_DIR}/styles"
)