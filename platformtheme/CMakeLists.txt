cmake_minimum_required(VERSION 3.16...3.22)
project(platformthemeplugin)

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Qt6XdgIconLoader REQUIRED Qt6XdgIconLoader)

# 分开查找 Qt6 模块
find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Gui REQUIRED)
find_package(Qt6DBus REQUIRED)
find_package(Qt6QuickControls2 REQUIRED)

# 分开查找 KDE Frameworks 6
find_package(KF6WindowSystem REQUIRED)

# XCB 相关库
pkg_check_modules(XCB REQUIRED xcb)
pkg_check_modules(XCB_EWMH REQUIRED xcb-ewmh)
pkg_check_modules(X11 REQUIRED x11)

set(SRCS
    main.cpp
    platformtheme.h
    platformtheme.cpp
    hintsettings.h
    hintsettings.cpp
    systemtrayicon.h
    systemtrayicon.cpp
    qdbusmenubar_p.h
    qdbusmenubar.cpp
    x11integration.h
    x11integration.cpp
    statusnotifier/dbustypes.h
    statusnotifier/dbustypes.cpp
    statusnotifier/statusnotifieritem.h
    statusnotifier/statusnotifieritem.cpp
)

# Qt6 DBus 接口生成
if(TARGET Qt6::DBus)
    qt6_add_dbus_interface(SRCS 
        org.kde.StatusNotifierWatcher.xml 
        statusnotifierwatcher_interface
    )
    
    qt6_add_dbus_adaptor(SRCS
        statusnotifier/org.kde.StatusNotifierItem.xml
        statusnotifier/statusnotifieritem.h
        StatusNotifierItem
    )
endif()

add_library(cutefishplatformtheme MODULE ${SRCS})

# 只定义必要的、简单的宏
target_compile_definitions(cutefishplatformtheme PRIVATE
    "QT_NO_FOREACH"
)

target_include_directories(cutefishplatformtheme PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${XCB_INCLUDE_DIRS}
    ${XCB_EWMH_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${Qt6XdgIconLoader_INCLUDE_DIRS}
)

target_link_libraries(cutefishplatformtheme PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::DBus
    Qt6::QuickControls2
    Qt6::CorePrivate  # 这会自动处理私有头文件路径
    Qt6::GuiPrivate   # 这会自动处理私有头文件路径
    KF6::WindowSystem
    ${XCB_LIBRARIES}
    ${XCB_EWMH_LIBRARIES}
    ${X11_LIBRARIES}
    ${Qt6XdgIconLoader_LIBRARIES}
)

set_target_properties(cutefishplatformtheme PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/platformthemes"
)

# 获取 Qt6 插件目录
get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
execute_process(
    COMMAND ${QT_QMAKE_EXECUTABLE} -query QT_INSTALL_PLUGINS
    OUTPUT_VARIABLE QT6_PLUGINS_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

install(TARGETS cutefishplatformtheme 
    LIBRARY DESTINATION "${QT6_PLUGINS_DIR}/platformthemes"
)