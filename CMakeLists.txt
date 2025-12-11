cmake_minimum_required(VERSION 3.16)
project(SpellChecker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_PREFIX_PATH "C:/6.5.2/mingw_64/lib/cmake")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_executable(SpellChecker
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    dict/dict.cpp
    radix_tree/radix_tree.cpp
    errors/errors.cpp
    text_operations/text_formation.cpp
    text_operations/BMH_search_alg.cpp
)

target_include_directories(SpellChecker PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/dict
    ${CMAKE_CURRENT_SOURCE_DIR}/radix_tree
    ${CMAKE_CURRENT_SOURCE_DIR}/errors
    ${CMAKE_CURRENT_SOURCE_DIR}/text_operations
)

target_link_libraries(SpellChecker Qt6::Core Qt6::Widgets)

add_executable(dict_preprocessor
    dict/dict_preprocessor.cpp
    dict/dict.cpp
    radix_tree/radix_tree.cpp
    errors/errors.cpp
    tests/test.cpp
    text_operations/BMH_search_alg.cpp
)

set_target_properties(dict_preprocessor PROPERTIES AUTOMOC OFF)

target_include_directories(dict_preprocessor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/dict
    ${CMAKE_CURRENT_SOURCE_DIR}/radix_tree
    ${CMAKE_CURRENT_SOURCE_DIR}/errors
)

if(WIN32)
    add_custom_command(TARGET SpellChecker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:Qt6::Core>"
        $<TARGET_FILE_DIR:SpellChecker>)
    add_custom_command(TARGET SpellChecker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:Qt6::Widgets>"
        $<TARGET_FILE_DIR:SpellChecker>)
endif()

add_custom_target(build_dict
    COMMAND $<TARGET_FILE:dict_preprocessor> ${CMAKE_SOURCE_DIR}/data/dict.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building dictionary from text file..."
    DEPENDS dict_preprocessor
    VERBATIM
)

add_custom_target(run_preprocessor_tests
    COMMAND $<TARGET_FILE:dict_preprocessor> ${CMAKE_SOURCE_DIR}/data/dict.txt --test
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running preprocessor tests..."
    DEPENDS dict_preprocessor
    VERBATIM
)

message(STATUS "Available targets:")
message(STATUS "  SpellChecker - main application")
message(STATUS "  dict_preprocessor - dictionary preprocessing tool")
message(STATUS "  build_dict - build dictionary from text file")
message(STATUS "  run_preprocessor_tests - run tests in preprocessor")