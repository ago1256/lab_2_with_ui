cmake_minimum_required(VERSION 3.16)
project(SpellChecker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Путь к Qt (ЗАМЕНИ НА СВОЙ ПУТЬ!)
# Примеры путей:
# set(CMAKE_PREFIX_PATH "C:/Qt/6.5.2/mingw_64/lib/cmake")
# set(CMAKE_PREFIX_PATH "C:/Qt/6.0.0/mingw81_64/lib/cmake")
set(CMAKE_PREFIX_PATH "C:/6.5.2/mingw_64/lib/cmake")

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 1. ОСНОВНОЕ ПРИЛОЖЕНИЕ (SpellChecker)
# Добавляем исполняемый файл
add_executable(SpellChecker
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    dict/dict.cpp
    radix_tree/radix_tree.cpp
    errors/errors.cpp
    text_operations/text_formation.cpp
    text_operations/text_search.cpp
)

# Добавляем папки в include для основного приложения
target_include_directories(SpellChecker PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/dict
    ${CMAKE_CURRENT_SOURCE_DIR}/radix_tree
    ${CMAKE_CURRENT_SOURCE_DIR}/errors
    ${CMAKE_CURRENT_SOURCE_DIR}/text_operations
)

# Линкуем Qt библиотеки только для основного приложения
target_link_libraries(SpellChecker Qt6::Core Qt6::Widgets)

# 2. ПРЕПРОЦЕССОР СЛОВАРЯ (dict_preprocessor) - БЕЗ Qt!
# Добавляем отдельный исполняемый файл для препроцессора
add_executable(dict_preprocessor
    dict/dict_preprocessor.cpp          # Нужно создать этот файл
    dict/dict.cpp                  # Используем общие файлы
    radix_tree/radix_tree.cpp      # Используем общие файлы
    errors/errors.cpp              # Используем общие файлы
)

# ВАЖНО: отключаем AUTOMOC для препроцессора, т.к. он не использует Qt
set_target_properties(dict_preprocessor PROPERTIES AUTOMOC OFF)

# Добавляем папки в include для препроцессора
target_include_directories(dict_preprocessor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/dict
    ${CMAKE_CURRENT_SOURCE_DIR}/radix_tree
    ${CMAKE_CURRENT_SOURCE_DIR}/errors
)

# НЕ линкуем Qt библиотеки для препроцессора!

# Для Windows - опционально, можно удалить если не нужно
if(WIN32)
    # Копируем Qt DLLs только для основного приложения
    add_custom_command(TARGET SpellChecker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:Qt6::Core>"
        $<TARGET_FILE_DIR:SpellChecker>)
    add_custom_command(TARGET SpellChecker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:Qt6::Widgets>"
        $<TARGET_FILE_DIR:SpellChecker>)
endif()

# Опционально: добавляем пользовательскую цель для сборки словаря
add_custom_target(build_dict
    COMMAND dict_preprocessor ../data/dict.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building dictionary from text file..."
    DEPENDS dict_preprocessor
)