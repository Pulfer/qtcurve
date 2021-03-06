if(ENABLE_QT5)
  set(QTC_QT5_LINK_LIBS)
  set(QTC_QT5_DEFS)
  set(QTC_QT5_INCLUDES)
  set(QTC_QT5_MODULES Qt5Core Qt5Gui Qt5Widgets Qt5Svg)
  if(QTC_ENABLE_X11)
    set(QTC_QT5_MODULES ${QTC_QT5_MODULES} Qt5DBus Qt5X11Extras)
  endif()
  foreach(QtModule ${QTC_QT5_MODULES})
    find_package(${QtModule} REQUIRED)
    set(QTC_QT5_LINK_LIBS ${QTC_QT5_LINK_LIBS} ${${QtModule}_LIBRARIES})
    set(QTC_QT5_DEFS ${QTC_QT5_DEFS} ${${QtModule}_DEFINITIONS})
    set(QTC_QT5_INCLUDES ${QTC_QT5_INCLUDES} ${${QtModule}_INCLUDE_DIRS})
  endforeach()
  get_target_property(REAL_QMAKE_EXECUTABLE ${Qt5Core_QMAKE_EXECUTABLE}
    IMPORTED_LOCATION)
  execute_process(COMMAND "${REAL_QMAKE_EXECUTABLE}" -query QT_INSTALL_PLUGINS
    OUTPUT_VARIABLE _Qt5_PLUGIN_INSTALL_DIR
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(QTCURVE_STYLE_DIR "${_Qt5_PLUGIN_INSTALL_DIR}/styles/")
endif()

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_BINARY_DIR}")
add_definitions(-DQTC_UTILS_QT5 -DQT_DISABLE_DEPRECATED_BEFORE=0x050100
  ${QTC_QT5_DEFS})
include_directories(${QTC_QT5_INCLUDES})

if(QTC_ENABLE_X11)
  include_directories(${QTC_X11_INCLUDE_DIRS})
  add_definitions(${QTC_X11_CFLAGS})
  set(QTC_QT5_XCB_LINK_LIBS ${QTC_X11_LIBRARIES})
else()
  set(QTC_QT5_XCB_LINK_LIBS)
endif()

add_subdirectory(common)
add_subdirectory(style)
