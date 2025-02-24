﻿# https://github.com/google/glog
set(glog_package_name "glog-0.7.1")

# --------------------------------------------------------------------------------------------------
# Extract glog zip...
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/3rd/${glog_package_name})
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${glog_package_name}.zip
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/3rd)
endif()

# --------------------------------------------------------------------------------------------------
# Add glog module...
set(glog_dst_dir ${CMAKE_CURRENT_SOURCE_DIR}/libs/${CMAKE_BUILD_TYPE}/${glog_package_name})
if(EXISTS ${glog_dst_dir})
  set(CMAKE_PREFIX_PATH ${glog_dst_dir} ${CMAKE_PREFIX_PATH})
  find_package(glog REQUIRED)
else()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rd/${glog_package_name})
endif()

# --------------------------------------------------------------------------------------------------
# Install glog library to libs
function(x_tools_install_glog target)
  add_custom_target(
    ${target}_install_glog
    COMMAND ${CMAKE_COMMAND} --install . --prefix ${glog_dst_dir}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/3rd/${glog_package_name}
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/glog.cmake)
  set_property(TARGET ${target}_install_glog PROPERTY FOLDER "install")
endfunction()
