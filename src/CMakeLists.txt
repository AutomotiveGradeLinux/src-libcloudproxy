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


cmake_minimum_required(VERSION 3.0)

set(TARGET_LIBCLOUDPROXY cloudproxy)
add_definitions("-std=c++11")


pkg_check_modules(libcloudproxy_depends  json-c libafbwsc libsystemd)
#pkg_check_modules(json-c libafbwsc libsystemd)
set(libcloudproxy_sources libcloudproxy.cpp)

include_directories(${LIBCLOUDPROXY_INCLUDE_DIR})
link_libraries(-Wl,--as-needed -Wl,--gc-sections -Wl,--no-undefined)

add_library(${TARGET_LIBCLOUDPROXY} SHARED ${libcloudproxy_sources})
target_compile_options(${TARGET_LIBCLOUDPROXY} PUBLIC ${libcloudproxy_depends_CFLAGS})

if(DEFINED DEBUGMODE)
 target_compile_options(${TARGET_LIBCLOUDPROXY} PRIVATE -g -O0)
else(DEFINED DEBUGMODE)
 target_compile_options(${TARGET_LIBCLOUDPROXY} PRIVATE -g -O2)
endif(DEFINED DEBUGMODE)

include_directories(${TARGET_LIBCLOUDPROXY} ${libcloudproxy_depends_INCLUDE_DIRS})
target_link_libraries(${TARGET_LIBCLOUDPROXY} afbwsc -lpthread ${link_libraries} ${libcloudproxy_depends_LIBRARIES})
set_target_properties(${TARGET_LIBCLOUDPROXY}
    PROPERTIES
    VERSION 0.1
)

if(DEFINED CMAKE_INSTALL_LIBDIR)
    INSTALL(TARGETS ${TARGET_LIBCLOUDPROXY}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif(DEFINED CMAKE_INSTALL_LIBDIR)
