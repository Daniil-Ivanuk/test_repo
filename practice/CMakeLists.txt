cmake_minimum_required(VERSION 3.20)
project(TimeTracking VERSION 1.0 LANGUAGES CXX)

if(NOT MSVC)
    message(FATAL_ERROR "C++/CLI projects require MSVC compiler")
endif()

# Пути к исходникам и заголовкам
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include")

include_directories(${INCLUDE_DIR})

# Директории для выходных файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release")

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG})
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE})

# Настройки компилятора
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /clr")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Исполняемый файл
add_executable(TimeTracking
    ${SOURCE_DIR}/LoginForm.cpp
    ${SOURCE_DIR}/Main.cpp
    ${SOURCE_DIR}/MainFormAdmin.cpp
    ${SOURCE_DIR}/RegisterForm.cpp
    ${SOURCE_DIR}/Role.cpp
    ${INCLUDE_DIR}/all_includes.h
    ${INCLUDE_DIR}/LoginForm.h
    ${INCLUDE_DIR}/Main.h
    ${INCLUDE_DIR}/MainFormAdmin.h
    ${INCLUDE_DIR}/RegisterForm.h
    ${INCLUDE_DIR}/Role.h
)

# Настройки для .NET Framework
set_target_properties(TimeTracking PROPERTIES
    VS_DOTNET_TARGET_FRAMEWORK_VERSION "v4.7.2"
    COMMON_LANGUAGE_RUNTIME ""
    WIN32_EXECUTABLE ON
)

# Рабочая директория для отладки
set_target_properties(TimeTracking PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}"
    VS_DEBUGGER_ENVIRONMENT "PATH=${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}"
    VS_CONFIGURATION_TYPE "Application"
)

# Подключаемые .NET сборки (из скриншота)
set_property(TARGET TimeTracking PROPERTY
    VS_DOTNET_REFERENCES
    "System"
    "System.Windows.Forms"
    "System.Drawing"
    "System.Data"
    "System.ComponentModel.Composition"
    "System.ComponentModel.Composition.Registration"
    "System.ComponentModel.DataAnnotations"
    "System.Drawing.Design"
    "System.Management"
)

# Дополнительные библиотеки
target_link_libraries(TimeTracking
    odbc32
    sqlsrv
)

# Дополнительные настройки
set_property(TARGET TimeTracking PROPERTY VS_WINRT_COMPONENT OFF)
set_property(TARGET TimeTracking PROPERTY VS_NO_SOLUTION_DEPLOY ON)

# Установка проекта как стартового
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT TimeTracking)

# Включить папки в обозревателе решений
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Настройки для Debug/Release
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(TimeTracking PROPERTIES
        COMPILE_DEFINITIONS "DEBUG;_DEBUG"
        MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL"
    )
else()
    set_target_properties(TimeTracking PROPERTIES
        COMPILE_DEFINITIONS "NDEBUG"
        MSVC_RUNTIME_LIBRARY "MultiThreadedDLL"
    )
endif()