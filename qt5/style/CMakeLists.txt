set(qtcurve_SRCS
  qtcurve.cpp
  qtcurve_api.cpp
  qtcurve_utils.cpp
  windowmanager.cpp
  blurhelper.cpp
  utils.cpp
  shortcuthandler.cpp)
set(qtcurve_MOC_HDRS
  qtcurve.h
  qtcurve_p.h
  qtcurve_plugin.h
  windowmanager.h
  blurhelper.h
  shortcuthandler.h)
set(qtcurve_X11_SRCS
  shadowhelper.cpp)
set(qtcurve_X11_MOC_HDRS
  shadowhelper.h)

translate_add_sources(${qtcurve_SRCS} ${qtcurve_MOC_HDRS}
  ${qtcurve_X11_SRCS} ${qtcurve_X11_MOC_HDRS})

if(NOT ENABLE_QT5)
  return()
endif()

set(qtcurve_style_common_SRCS
  ../common/common.cpp
  ../common/config_file.cpp)

foreach(pngname check_on check_x_on dialog_error dialog_information
    dialog_warning)
  add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${pngname}-png.h"
    COMMAND "${PROJECT_BINARY_DIR}/tools/qt-dump-png"
    "${PROJECT_SOURCE_DIR}/data/${pngname}.png" "qtc_${pngname}"
    "${CMAKE_CURRENT_BINARY_DIR}/${pngname}-png.h"
    DEPENDS qt-dump-png "${PROJECT_SOURCE_DIR}/data/${pngname}.png")
  add_custom_target(qtc_qt5_${pngname}_hdr
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${pngname}-png.h")
endforeach()

set(qtcurve_SRCS ${qtcurve_SRCS} ${qtcurve_style_common_SRCS})

if(QTC_ENABLE_X11)
  set(qtcurve_SRCS ${qtcurve_X11_SRCS} ${qtcurve_SRCS})
  set(qtcurve_MOC_HDRS ${qtcurve_X11_MOC_HDRS} ${qtcurve_MOC_HDRS})
endif()

add_definitions(-DQT_PLUGIN)
include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}")
qt5_wrap_cpp(qtcurve_MOC_SRCS ${qtcurve_MOC_HDRS})
add_library(qtcurve-qt5 MODULE ${qtcurve_SRCS} ${qtcurve_MOC_SRCS})
set_target_properties(qtcurve-qt5 PROPERTIES
  LINK_FLAGS "-Wl,--no-undefined"
  OUTPUT_NAME "qtcurve"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  PREFIX "")
add_dependencies(qtcurve-qt5 qtc_qt5_check_on_hdr qtc_qt5_check_x_on_hdr
  qtc_qt5_dialog_error_hdr qtc_qt5_dialog_information_hdr
  qtc_qt5_dialog_warning_hdr)
target_link_libraries(qtcurve-qt5 ${QTC_QT5_LINK_LIBS}
  ${QTC_QT5_XCB_LINK_LIBS} qtcurve-utils)
install(TARGETS qtcurve-qt5 LIBRARY DESTINATION ${QTCURVE_STYLE_DIR})
