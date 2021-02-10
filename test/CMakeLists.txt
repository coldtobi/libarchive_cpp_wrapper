#BSD 2-Clause license
#
#Copyright (c) 2014, Domen Vrankar
#Copyright (c) 2021, Tobias Frost
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

find_package(GTest)

if(GTEST_FOUND)
    add_executable(libarchive_cpp_wrapper_test)

    target_sources(libarchive_cpp_wrapper_test
        PRIVATE
	    test.cpp
    )

    target_link_libraries(libarchive_cpp_wrapper_test
        PRIVATE
	    LibArchive::archive_cpp_wrapper
	    GTest::gtest_main
    )

    add_test(NAME libarchive_cpp_wrapper_test COMMAND libarchive_cpp_wrapper_test)

    # this avoids that the test fails if it is not built.(cmake will say "not run", not "failed")
    set_tests_properties(libarchive_cpp_wrapper_test PROPERTIES REQUIRED_FILES "$<TARGET_FILE:libarchive_cpp_wrapper_test>")

    set_tests_properties(libarchive_cpp_wrapper_test
      PROPERTIES ENVIRONMENT "TEST_RESOURCES=${CMAKE_CURRENT_SOURCE_DIR}/resources/")
endif()
