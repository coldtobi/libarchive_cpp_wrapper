#BSD 2-Clause license
#
#Copyright (c) 2014, Domen Vrankar
#All rights reserved.
#
#Redistribution and use in source and binary forms, with or without modification,
#are permitted provided that the following conditions are met:
#
#1. Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
#2. Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
#ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
#ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required( VERSION 3.5 )

project(archive_cpp_wrapper)

include(CTest)

set(CMAKE_CXX_STANDARD 11)

find_package(LibArchive)
add_library(LibArchive::LibArchive INTERFACE IMPORTED GLOBAL)
target_include_directories(LibArchive::LibArchive INTERFACE ${LibArchive_INCLUDE_DIRS})
target_link_libraries(LibArchive::LibArchive INTERFACE ${LibArchive_LIBRARIES})


file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB template_implementations "${CMAKE_CURRENT_SOURCE_DIR}/*.ipp")

add_library( ${PROJECT_NAME} SHARED

  archive_entry.cpp

  archive_reader.cpp
  archive_reader_iterator.cpp
  archive_reader_entry_buffer.cpp
  
  archive_writer.cpp

  archive_exception.cpp

  ${headers}
  ${template_implementations}
)

target_link_libraries( ${PROJECT_NAME} PRIVATE LibArchive::LibArchive )
target_include_directories(${PROJECT_NAME} PRIVATE ${libarchive_INCLUDE_DIRS} )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} )

set_target_properties( ${PROJECT_NAME} PROPERTIES VERSION "1.0.0" SOVERSION "1" )

install(
  TARGETS ${PROJECT_NAME} LIBRARY
  DESTINATION lib
)

install(
  FILES
  
  archive_reader_entry_buffer.hpp
  archive_reader.hpp
  archive_writer_format.hpp
  archive_exception.hpp
  archive_reader_iterator.hpp
  archive_reader_format.hpp
  archive_writer_filter.hpp
  archive_reader_filter.hpp
  archive_entry.hpp
  archive_writer.hpp
  archive_reader.ipp
  archive_writer.ipp
  
  DESTINATION include 
)


# alias target
add_library(LibArchive::archive_cpp_wrapper ALIAS ${PROJECT_NAME})

add_subdirectory(test)

