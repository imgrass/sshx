# Description:
#   ./README
# Help:
#   [https://cmake.org/cmake/help/latest/]
#   [https://zhuanlan.zhihu.com/p/93895403]
# Author: imgrass

cmake_minimum_required(VERSION 3.0)

# set the project name
project(sshx C)
set(EXECUTABLE_FILE_NAME sshx)

# print environment argvs
message(STATUS "[CMAKE_SYSTEM] system:${CMAKE_SYSTEM}")
message(STATUS "[CMAKE_C_COMPILER_ID] ${CMAKE_C_COMPILER_ID}")
if (NOT "${CMAKE_C_COMPILER_ID}" STREQUAL "GNU")
    message(FATAL_ERROR "We want GNU-C compiler, but now is
                         ${CMAKE_C_COMPILER_ID}")
endif()

# set complilation flags
add_compile_options(-O2 -g -D_FORTIFY_SOURCE=2 -fPIE -fstack-protector-strong
                    --param=ssp-buffer-size=4 -Wmissing-prototypes -Wall)

#==============================================================================
# COMPILE OPTIONS
#==============================================================================
message(STATUS "==+ ON or OFF these compile options")
macro(handle_compile_option name description initial_value)
    option(${name} ${description} ${initial_value})
    if (${${name}} STREQUAL "ON")
        message(STATUS "+ set -D${name} ON: ${description}")
        add_definitions(-D${name})
    else()
        message(STATUS "- unset -D${name}")
    endif()
endmacro()

handle_compile_option(IMGRASS_DEBUG "Option for debug used by imgrass" OFF)


#==============================================================================
# SOURCE FILES
#==============================================================================
macro(add_dir_for_src_lst src_list dir src_list_base)
    message(STATUS "==+ Include src files under <dir:${${dir}}>")
    foreach(src IN LISTS ${src_list_base})
        message(STATUS "==> Add src file <${src}>")
        set(${src_list} ${${src_list}} ${${dir}}/${src})
    endforeach()
    message(STATUS "... src_list is ${${src_list}}")
endmacro()

set(SRC_FILES)
# perform the same operation on the source code in different directories
set(DIR_SRC src)
set(SRC_FILES_BASE
    main.c parse_roadmap.c ssh_sftp.c ssh_tunnel.c utils.c
)
add_dir_for_src_lst(SRC_FILES DIR_SRC SRC_FILES_BASE)
# ...


#==============================================================================
# THE DIRECTORIES OF HEADER FILES
#==============================================================================
set(DIR_HEADER
    include
    lib/libssh2/include
    lib/libxml.c/include
)
include_directories(${DIR_HEADER})

#==============================================================================
# THE DIRECTORIES OF STATIC LINK LIBRARY
#==============================================================================
macro(printf_linked_lib lnked_lib_lst)
    foreach(lib IN LISTS ${lnked_lib_lst})
        message(STATUS "==> Link lib <${lib}>")
    endforeach()
endmacro()
#------------------------------------------------------------------------------
# Do not specify the file location exactly, the linker automatically looks for
# it.
set(DIR_LIB_FOUND_AUTOMATICALLY
    lib/libssh2/lib
    lib/libxml.c/lib
)
link_directories(${DIR_LIB_FOUND_AUTOMATICALLY})
find_library(LIB_SSH2 libssh2.a ${DIR_LIB_FOUND_AUTOMATICALLY})
find_library(LIB_XMLC libxml.a ${DIR_LIB_FOUND_AUTOMATICALLY})

#------------------------------------------------------------------------------
# specify the file location exactly to avoid the confusion with other files of
# the same name
## set(LIB_XML2 libxml2.a)
## link_libraries(${CMAKE_SOURCE_DIR}/lib/libxml2/lib/${LIB_XML2})

# print the libraries here.
set(LIB_NEED_TO_LINKED
    ${LIB_XMLC}
    ${LIB_SSH2}
)
printf_linked_lib(LIB_NEED_TO_LINKED)


#==============================================================================
# TARGET BINARY FILE
#==============================================================================
add_executable(${EXECUTABLE_FILE_NAME} ${SRC_FILES})

#------------------------------------------------------------------------------
# implement link ...
target_link_libraries(${EXECUTABLE_FILE_NAME} ${LIB_NEED_TO_LINKED})
