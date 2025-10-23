# Finds ROS packages.
# If successful, sets the following variables:
#
# ROS_VERSION: string for the found ROS version (if not already set)
# ROS_INCLUDE_DIR: ROS header include path
# ROS_LIBRARIES: library files to link with in order to get ROS functionality
#
# If ROS_VERSION is not set, it will check ros versions jade, indigo, hydro, and groovy
# and configure variables for the newest version.  ROS_VERSION will also be set.
#
# If ROSDEPS is set, then it will add these dependencies to ROS_LIBRARIES
# SET(ROSDEPS rclcpp rcutils tracetools rcl rmw std_msgs__rosidl_typesupport_cpp statistics_msgs__rosidl_typesupport_cpp libstatistics_collector rmw)
#

SET(AVAILABLE_ROS_VERSIONS "humble;noetic;melodic;lunar;kinetic;jade;indigo;hydro;groovy;jazzy;kilted")
SET(ROS_DIRS "/opt/ros" "C:/pixi_ws/ros2-windows")

IF(ROSDEPS)
ELSE(ROSDEPS)
  SET(ROSDEPS rclcpp rcutils rcl rmw std_msgs__rosidl_typesupport_cpp
  statistics_msgs__rosidl_typesupport_cpp libstatistics_collector rmw)
ENDIF(ROSDEPS)



SET(ROS_VERSION CACHE STRING "ROS version")
SET(ROS_FOUND FALSE)

FOREACH(rosdirs ${ROS_DIRS})
  IF(ROS_VERSION STREQUAL "")
    MESSAGE("ROS_VERSION not defined")
      FOREACH(version ${AVAILABLE_ROS_VERSIONS})
        IF(NOT ROS_FOUND)
          IF(CMAKE_SYSTEM_NAME STREQUAL Windows)
            SET(pathtmp ${rosdirs}/include/rclcpp/rclcpp/rclcpp.hpp)
          ELSE(CMAKE_SYSTEM_NAME STREQUAL Windows)
            SET(pathtmp ${rosdirs}/${version}/include/rclcpp/rclcpp/rclcpp.hpp)
          ENDIF(CMAKE_SYSTEM_NAME STREQUAL Windows)
          IF(EXISTS ${pathtmp})
            MESSAGE(STATUS "Found ros version ${version}")
            SET(ROS_VERSION2 ${version})
            SET(ROS_FOUND 1)
          ENDIF(EXISTS ${pathtmp})
        ENDIF(NOT ROS_FOUND)
      ENDFOREACH(version)
  ELSE(ROS_VERSION STREQUAL "")
      IF(CMAKE_SYSTEM_NAME STREQUAL Windows)
        SET(pathtmp ${rosdirs}/include/rclcpp/rclcpp/rclcpp.hpp)
      ELSE(CMAKE_SYSTEM_NAME STREQUAL Windows)
        SET(pathtmp ${rosdirs}/${version}/include/rclcpp/rclcpp/rclcpp.hpp)
      ENDIF(CMAKE_SYSTEM_NAME STREQUAL Windows)
      IF(EXISTS ${pathtmp})
        SET(ROS_FOUND 1)
        SET(ROS_VERSION2 ${ROS_VERSION})
      ENDIF(EXISTS ${pathtmp})
  ENDIF(ROS_VERSION STREQUAL "")


  IF(NOT ROS_FOUND)
    MESSAGE(WARNING "ROS files not found")
  ELSE(NOT ROS_FOUND)
    IF(CMAKE_SYSTEM_NAME STREQUAL Windows)
      SET(ROS_PATH ${rosdirs})
    ELSE(CMAKE_SYSTEM_NAME STREQUAL Windows)
      SET(ROS_PATH ${rosdirs}/${ROS_VERSION2})
    ENDIF(CMAKE_SYSTEM_NAME STREQUAL Windows)
    FILE(GLOB dirs ${ROS_PATH}/include ${ROS_PATH}/include/*)
    SET(dirlist "")
    FOREACH(dir ${dirs})
      IF(IS_DIRECTORY ${dir})
        LIST(APPEND dirlist ${dir})
      ENDIF()
    ENDFOREACH(dir ${dirs})
    SET(ROS_INCLUDE_DIRS ${dirlist})
    MESSAGE(STATUS "Include dirs ${ROS_INCLUDE_DIRS}")
    MESSAGE(STATUS "Detected ROS version ${ROS_VERSION2}")
    FOREACH(NAME ${ROSDEPS})
      FIND_LIBRARY(lib${NAME} NAMES ${NAME} PATHS ${ROS_PATH}/lib)
      LIST(APPEND ROS_LIBRARIES ${lib${NAME}})
    ENDFOREACH(NAME)
    MESSAGE(STATUS "ROS dependencies ${ROSDEPS} need the following libraries:")
    FOREACH(NAME ${ROS_LIBRARIES})
      MESSAGE(STATUS "  " ${NAME})
    ENDFOREACH(NAME)
    
    MESSAGE(STATUS "ROS will also need Boost libraries...")
    find_library (BOOST_SYSTEM_LIB "boost_system")
    LIST(APPEND ROS_LIBRARIES ${BOOST_SYSTEM_LIB})
    MESSAGE(STATUS "  " ${ROS_LIBRARIES})
  ENDIF(NOT ROS_FOUND)
  MARK_AS_ADVANCED(ROS_LIBRARIES)
ENDFOREACH(rosdirs)
