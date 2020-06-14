# Tests
add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")

macro(package_add_test TESTNAME)
    # create an exectuable in which the tests will be stored
    add_executable(${TESTNAME} ${ARGN})
    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    target_link_libraries(${TESTNAME} gtest 
                                      gmock 
                                      gtest_main
                                      libControlDeck)
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
    gtest_discover_tests(${TESTNAME}
        # set a working directory so your project root so that you can find test data via paths relative to the project root
        WORKING_DIRECTORY ${PROJECT_DIR}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()

package_add_test(inesTest inesTest.cpp)
package_add_test(cartridgeTest cartridgeTest.cpp)
package_add_test(ppuMemory ppu/ppuMemoryMapperTest.cpp)
package_add_test(AddressingModehandlerTest cpu/AddressingModehandlerTest.cpp)
package_add_test(CPU2A03Test cpu/CPU2A03Test.cpp)
package_add_test(InstructionTest cpu/InstructionTest.cpp)
package_add_test(SystemComponentsTest cpu/SystemComponentsTest.cpp)
