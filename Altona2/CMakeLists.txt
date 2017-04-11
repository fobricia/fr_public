cmake_minimum_required(VERSION 2.6)

project(Altona2)

include(CheckCXXCompilerFlag)
include(CheckTypeSize)

# Run cmake -DCMAKE_BUILD_TYPE=Debug .
# or  cmake -DCMAKE_BUILD_TYPE=Release .
# to enable debugging or optimizations from the command line.
# Debug builds have range checks enabled by default.
if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE Debug)
	#set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	check_cxx_compiler_flag("-O0 -g" ALTONA2_HAS_CXX_OPTION_DEBUG_GCC)
	if(ALTONA2_HAS_CXX_OPTION_DEBUG_GCC)
		set(ALTONA2_DEBUG_FLAGS "-O0 -g")
	else()
		check_cxx_compiler_flag("/Od /Zi /DEBUG" ALTONA2_HAS_CXX_OPTION_DEBUG_VC)
		if(ALTONA2_HAS_CXX_OPTION_DEBUG_VC)
			set(ALTONA2_DEBUG_FLAGS "/Od /Zi /DEBUG")
		else()
			message(WARNING "Unknown compiler. Don't know how to set debug flags.")
		endif()
	endif()
	add_definitions(-DsDEFINE_MEMDEBUG)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	check_cxx_compiler_flag("-g" ALTONA2_HAS_CXX_OPTION_DEBUG_GCC)
	if(ALTONA2_HAS_CXX_OPTION_DEBUG_GCC)
		set(ALTONA2_DEBUG_FLAGS "-g")
	else()
		check_cxx_compiler_flag("/Zi /DEBUG" ALTONA2_HAS_CXX_OPTION_DEBUG_VC)
		if(ALTONA2_HAS_CXX_OPTION_DEBUG_VC)
			set(ALTONA2_DEBUG_FLAGS "/Zi /DEBUG")
		else()
			message(WARNING "Unknown compiler. Don't know how to set debug flags.")
			set(ALTONA2_DEBUG_FLAGS "")
		endif()
	endif()
else()
	message(FATAL_ERROR "Unknown build type ${CMAKE_BUILD_TYPE}.")
endif()

check_cxx_compiler_flag("-std=c++11" ALTONA2_HAS_CXX_OPTION_CXX11_GCC47)
if(ALTONA2_HAS_CXX_OPTION_CXX11_GCC47)
	set(ALTONA2_CXX11_FLAGS "-std=c++11")
else()
	check_cxx_compiler_flag("-std=c++0x" ALTONA2_HAS_CXX_OPTION_CXX11_GCC43)
	if(ALTONA2_HAS_CXX_OPTION_CXX11_GCC43)
		set(ALTONA2_CXX11_FLAGS "-std=c++0x")
	else()
		message(WARNING "No suitable C++ dialect option found. Assuming compiler supports C++11.")
		set(ALTONA2_CXX11_FLAGS "")
	endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ALTONA2_DEBUG_FLAGS} ${ALTONA2_CXX11_FLAGS}")

# not very nice - but that whole incbin stuff isn't very nice.
# just generate the damn source code and let us take care of assembling it!
check_type_size("void*" SIZEOF_VOID_P BUILTIN_TYPES_ONLY)
if(SIZEOF_VOID_P EQUAL 8)
	set(ALTONA2_INCBIN_ARCH "-x64")
elseif(SIZEOF_VOID_P EQUAL 4)
	set(ALTONA2_INCBIN_ARCH "")
else()
	set(ALTONA2_INCBIN_ARCH "")
	message(WARNING "Unknown target register width ${SIZEOF_VOID_P}. Fix incbin so it doesn't depend on the CPU architecture.")
endif()

find_program(ALTONA2_TOOL_yasm yasm)
if(NOT ALTONA2_TOOL_yasm)
	message(WARNING "yasm not found on path. Expect that incbin will not work, and that makeproject will not compile.")
endif()

add_definitions(-DsDEFINE_LINUX -DsDEFINE_SHELL)

# check for librt here, if necessary

include_directories(
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/.."
)

add_library(base OBJECT
	Altona2/Libs/Base/Containers.cpp
	Altona2/Libs/Base/Graphics.cpp
	Altona2/Libs/Base/GraphicsNull.cpp
	Altona2/Libs/Base/Machine.cpp
	Altona2/Libs/Base/String.cpp
	Altona2/Libs/Base/System.cpp
	Altona2/Libs/Base/SystemLinux.cpp
	Altona2/Libs/Base/SystemPosix.cpp
	Altona2/Libs/Base/Types.cpp
	Altona2/Libs/Base/Math.cpp
	Altona2/Libs/Base/Serialize.cpp
)

add_library(util OBJECT
	Altona2/Libs/Util/Scanner.cpp
)

add_executable(incbin
	Altona2/Tools/Incbin/Main.cpp
	$<TARGET_OBJECTS:base>
	$<TARGET_OBJECTS:util>
)

add_executable(asc
	Altona2/Tools/Asc/Asc.cpp
	Altona2/Tools/Asc/Doc.cpp
	Altona2/Tools/Asc/Hlsl.cpp
	Altona2/Tools/Asc/Glsl.cpp
	Altona2/Tools/Asc/Main.cpp
	$<TARGET_OBJECTS:base>
	$<TARGET_OBJECTS:util>
)

add_executable(makeproject
	Altona2/Tools/MakeProject/Doc.cpp
	Altona2/Tools/MakeProject/DocMake.cpp
	Altona2/Tools/MakeProject/DocVs2008.cpp
	Altona2/Tools/MakeProject/DocVs2010.cpp
	Altona2/Tools/MakeProject/DocVs2012.cpp
	Altona2/Tools/MakeProject/DocXcode4.cpp
	Altona2/Tools/MakeProject/DocNdk.cpp
	Altona2/Tools/MakeProject/Main.cpp
	${PROJECT_BINARY_DIR}/Incbin.o
	$<TARGET_OBJECTS:base>
	$<TARGET_OBJECTS:util>
)

add_custom_command(
	OUTPUT ${PROJECT_BINARY_DIR}/Incbin.o
	COMMAND incbin -i=${PROJECT_SOURCE_DIR}/Altona2/Tools/MakeProject/Incbin.incbin -o=${PROJECT_BINARY_DIR}/Incbin.o ${ALTONA2_INCBIN_ARCH}
	MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/Altona2/Tools/MakeProject/Incbin.incbin
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Altona2/Tools/MakeProject
)
