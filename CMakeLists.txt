#
# Copyright (C) 2020 Orion Innovation, formerly MERA
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

project(libcloudproxy C CXX)

cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME "LIBCLOUDPROXY")
set(PROJECT_VERSION "1.0")
set(PROJECT_PRETTY_NAME "LIBCLOUDPROXY")
set(PROJECT_DESCRIPTION "Library for accessing agl cloud proxy service with C/C++")
set(PROJECT_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
set(PROJECT_LIBDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
set(LIBCLOUDPROXY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# get pkgconfig
INCLUDE(FindPkgConfig)
link_libraries(-Wl,--as-needed -Wl,--gc-sections -Wl,--no-undefined)

set(CMAKE_BUILD_TYPE Debug)
set(USE_HMI_DEBUG FALSE)

add_subdirectory(src)
add_subdirectory(include)

#generate configure file
configure_file(libcloudproxy.pc.in libcloudproxy.pc @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcloudproxy.pc
  DESTINATION
  ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
