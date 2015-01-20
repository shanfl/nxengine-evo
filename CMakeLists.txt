cmake_minimum_required(VERSION 2.6)
macro(source_group_by_dir source_files)
    if(MSVC)
        set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
        foreach(sgbd_file ${${source_files}})
            string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif(MSVC)
endmacro(source_group_by_dir)

include(CheckCXXCompilerFlag)

project(nx)

set (nx_VERSION_MAJOR 2)
set (nx_VERSION_MINOR 1)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(SDL2)
find_package(SDL2_ttf)
find_package(PNG)

include_directories(${SDL2_INCLUDE_DIR})
include_directories(${SDL2TTF_INCLUDE_DIR})

file(GLOB APP_SOURCES "[a-zA-Z]*.cpp" "[a-zA-Z]*.h")
file(GLOB TB_SOURCES "TextBox/[a-zA-Z]*.cpp" "TextBox/[a-zA-Z]*.h")
file(GLOB_RECURSE AI_SOURCES "ai/[a-zA-Z]*.cpp" "ai/[a-zA-Z]*.h")
file(GLOB AG_SOURCES "autogen/[a-zA-Z]*.cpp" "autogen/[a-zA-Z]*.h")
file(GLOB CM_SOURCES "common/[a-zA-Z]*.cpp" "common/[a-zA-Z]*.h")
file(GLOB EG_SOURCES "endgame/[a-zA-Z]*.cpp" "endgame/[a-zA-Z]*.h")
file(GLOB GR_SOURCES "graphics/[a-zA-Z]*.cpp" "graphics/[a-zA-Z]*.h")
file(GLOB IN_SOURCES "intro/[a-zA-Z]*.cpp" "intro/[a-zA-Z]*.h")
file(GLOB PA_SOURCES "pause/[a-zA-Z]*.cpp" "pause/[a-zA-Z]*.h")
file(GLOB SL_SOURCES "siflib/[a-zA-Z]*.cpp" "siflib/[a-zA-Z]*.h")
file(GLOB SN_SOURCES "sound/[a-zA-Z]*.cpp" "sound/[a-zA-Z]*.h")
file(GLOB EXTR_SOURCES "extract/[a-zA-Z]*.cpp" "extract/[a-zA-Z]*.h")
set(EXTR_SOURCES
    ${EXTR_SOURCES}
    "common/misc.cpp"
    "common/stat.cpp"
    "stagedata.cpp"
)

include_directories(${nx_SOURCE_DIR})

set(SOURCES
    ${APP_SOURCES}
    ${TB_SOURCES}
    ${AI_SOURCES}
    ${AG_SOURCES}
    ${CM_SOURCES}
    ${EG_SOURCES}
    ${GR_SOURCES}
    ${IN_SOURCES}
    ${PA_SOURCES}
    ${SL_SOURCES}
    ${SN_SOURCES}
)
source_group_by_dir(SOURCES)
option(DEBUG "build application with debug traces" OFF) # OFF is the default
option(MUTABLE_SCALE "build application with changable scale" ON) # OFF is the default
option(USE_TTF "build application with ttf font support" ON) # OFF is the default

if(DEBUG)
  add_definitions(-DTRACE_SCRIPT)
endif(DEBUG)

if(MUTABLE_SCALE)
    add_definitions(-DCONFIG_MUTABLE_SCALE)
endif(MUTABLE_SCALE)

if(USE_TTF)
    add_definitions(-DCONFIG_ENABLE_TTF)
endif(USE_TTF)

add_executable(nx ${SOURCES})
target_link_libraries(nx ${SDL2_LIBRARY} ${SDL2TTF_LIBRARY} ${PNG_LIBRARY})

add_executable(extract ${EXTR_SOURCES})


