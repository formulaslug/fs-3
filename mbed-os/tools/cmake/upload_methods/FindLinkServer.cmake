# Copyright (c) 2023 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

# ----------------------------------------------
# CMake finder for LinkServer, the NXP command-line flash and debug tool
#
# This module defines:
# LinkServer - Whether the reqested tools were found.
# LinkServer_PATH - full path to the LinkServer command line tool.
# LinkServer_VERSION - version number of LinkServer

# Check for LinkServer install folders on Windows
if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
    # On Windows, LinkServer by default is installed into a subdirectory of
    # C:/nxp
    file(GLOB LINKSERVER_HINTS LIST_DIRECTORIES TRUE "C:/nxp/LinkServer_*")
elseif("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin")
    # On Mac, it was observed to install into /Applications/LinkServer_1.2.45/dist
    file(GLOB LINKSERVER_HINTS LIST_DIRECTORIES TRUE "/Applications/LinkServer_*/dist")
else()
    set(LINKSERVER_HINTS /usr/local/LinkServer) # Linux package install location
endif()

find_program(LinkServer_PATH
    NAMES LinkServer
    DOC "Path to the LinkServer executable."
    HINTS ${LINKSERVER_HINTS}
)

if(EXISTS "${LinkServer_PATH}")
	# Detect version
	execute_process(COMMAND ${LinkServer_PATH} --version
			OUTPUT_VARIABLE LinkServer_VERSION_OUTPUT)

	# The output looks like "LinkServer v1.2.45 [Build 45] [2023-07-25 09:54:50]", so use a regex to grab the version
	string(REGEX REPLACE "LinkServer v([0-9]+\\.[0-9]+\\.[0-9]+).*" "\\1" LinkServer_VERSION ${LinkServer_VERSION_OUTPUT})
endif()


find_package_handle_standard_args(LinkServer REQUIRED_VARS LinkServer_PATH VERSION_VAR LinkServer_VERSION)


