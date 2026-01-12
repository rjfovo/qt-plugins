cmake_minimum_required(VERSION 3.16...3.22)
project(platformthemeplugin)

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(PkgConfig REQUIRED)

# 分开查找 Qt6 模块
find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Gui REQUIRED)
## DBus support: detect if available and link when present
find_package(Qt6DBus QUIET)

## KDE Frameworks optional for now
# KF6 WindowSystem 和 StatusNotifierItem 可选，若存在则链接并启用宏
find_package(KF6WindowSystem QUIET)
find_package(KF6StatusNotifierItem QUIET)

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
    x11integration.h
    x11integration.cpp
    # statusnotifier/dbustypes.h
    # statusnotifier/dbustypes.cpp
    # statusnotifier/statusnotifieritem.h
    # statusnotifier/statusnotifieritem.cpp
    # statusnotifier/statusnotifieritem.h
    # statusnotifier/statusnotifieritem.cpp
)

# Choose real systemtray implementation when KF6 StatusNotifierItem/WindowSystem are available,
# otherwise fall back to a minimal stub to avoid adding a hard dependency during development.
if(TARGET KF6::StatusNotifierItem AND TARGET KF6::WindowSystem)
    list(APPEND SRCS
        systemtrayicon.h
        systemtrayicon.cpp
    )
else()
    ## Choose real or stub systemtray implementation depending on available libraries
    list(APPEND SRCS
        systemtrayicon.h
    )

    if(KF6WindowSystem_FOUND OR KF6StatusNotifierItem_FOUND OR Qt6DBus_FOUND)
        list(APPEND SRCS systemtrayicon.cpp)
    else()
        list(APPEND SRCS systemtrayicon_stub.cpp)
    endif()
endif()

# Qt6 DBus 接口生成
## DBus interface generation skipped
# if(TARGET Qt6::DBus)
#     qt6_add_dbus_interface(SRCS 
#         org.kde.StatusNotifierWatcher.xml 
#         statusnotifierwatcher_interface
#     )
#     
#     qt6_add_dbus_adaptor(SRCS
#         statusnotifier/org.kde.StatusNotifierItem.xml
#         statusnotifier/statusnotifieritem.h
#         StatusNotifierItem
#     )
# endif()

add_library(cutefishplatformtheme MODULE ${SRCS})

# 只定义必要的、简单的宏
target_compile_definitions(cutefishplatformtheme PRIVATE
    "QT_NO_FOREACH"
)

if(KF6WindowSystem_FOUND)
    target_compile_definitions(cutefishplatformtheme PRIVATE HAVE_KWINDOWSYSTEM)
endif()

target_include_directories(cutefishplatformtheme PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${XCB_INCLUDE_DIRS}
    ${XCB_EWMH_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    /usr/include/x86_64-linux-gnu/qt6/QtGui/6.8.2
    /usr/include/x86_64-linux-gnu/qt6/QtGui/6.8.2/QtGui
)

target_link_libraries(cutefishplatformtheme PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    ${XCB_LIBRARIES}
    ${XCB_EWMH_LIBRARIES}
    ${X11_LIBRARIES}
)

if(Qt6DBus_FOUND)
    target_link_libraries(cutefishplatformtheme PRIVATE Qt6::DBus)
endif()

if(KF6WindowSystem_FOUND)
    target_link_libraries(cutefishplatformtheme PRIVATE KF6::WindowSystem)
endif()

if(KF6StatusNotifierItem_FOUND)
    target_link_libraries(cutefishplatformtheme PRIVATE KF6::StatusNotifierItem)
endif()

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
