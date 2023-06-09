﻿cmake_minimum_required(VERSION 3.1)
cmake_policy(SET CMP0091 NEW) # for CMAKE_MSVC_RUNTIME_LIBRARY
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # cmake_policy(SET CMP0077 NEW)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
project(autogit)

################################################################################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}" "${PROJECT_SOURCE_DIR}/cmake")

include(CheckCXXCompilerFlag)
include(CheckLibraryExists)
include(CheckCXXLinkerFlag)
include(CheckIPOSupported)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

################################################################################

if (ENABLE_GIT_VERSION)
	find_package(Git)
	gitGetVersion(${CMAKE_CURRENT_SOURCE_DIR} autogit)
	set(VERSION_GIT ${autogit_WC_REVISION_HASH})
	string(TIMESTAMP PVERSION "%Y%m%d-${VERSION_GIT}-${CMAKE_SYSTEM_NAME}")
endif()

# Disable in-source builds to prevent source tree corruption.
if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()
message(STATUS "CMAKE Version: ${CMAKE_VERSION}")

message(STATUS "Source Dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Host System name: ${CMAKE_HOST_SYSTEM_NAME}")

set(default_build_type "Release")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
        STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
                 "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

message(STATUS "System name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "System version: ${CMAKE_SYSTEM_VERSION}")
if (WIN32)
	message(STATUS "Platform version: ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "SYSTEM NAME: ${CMAKE_SYSTEM_NAME}")


################################################################################

option(ENABLE_BUILD_WERROR			"All warnings being treated as errors" ON)
option(ENABLE_GIT_VERSION			"Enable git version" ON)

option(ENABLE_USE_IO_URING			"Enable auto use io_uring" OFF)
option(ENABLE_BUILD_WITH_LIBCXX		"Build with libc++" OFF)

option(ENABLE_SYSTEM_ZLIB			"Build with system zlib support" OFF)
option(ENABLE_SYSTEM_OPENSSL		"Build with system openssl support" OFF)
option(ENABLE_SYSTEM_LIBSSH2		"Build with system libssh2 support" OFF)


option(ENABLE_MOLD					"build using mold" OFF)
option(ENABLE_LLD					"build using lld" OFF)

option(ENABLE_TCMALLOC_STATIC		"Build with Tcmalloc support" OFF)
option(ENABLE_JEMALLOC_STATIC		"Build with Jemalloc support" OFF)

option(ENABLE_STATIC_LINK_TO_GCC	"Build static link to gcc" ON)

################################################################################

find_program(MOLD_LINKER mold)
find_program(LLD_LINKER lld)

set(CMAKE_C_STANDARD 11)

################################################################################

if (MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

	add_definitions(
		-D_CRT_SECURE_NO_DEPRECATE
		-D_CRT_SECURE_NO_WARNINGS
		-D_CRT_NONSTDC_NO_DEPRECATE
		-D_CRT_NONSTDC_NO_WARNINGS
		-D_SCL_SECURE_NO_DEPRECATE
		-D_SCL_SECURE_NO_WARNINGS
	)

	add_definitions(
		-DWIN32_LEAN_AND_MEAN
		-D_WIN32_WINNT=0x0601
		-DNOMINMAX
		-DUNICODE
		-D_UNICODE
		-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
	)

	add_definitions(
		-DBOOST_ALL_STATIC_LINK
		-DBOOST_THREAD_USE_LIB
		-DBOOST_FILESYSTEM_STATIC_LINK
		-DBOOST_USE_WINAPI_VERSION=0x0601
	)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /MP /Zc:__cplusplus")
	message(STATUS "Using parallel compiling (/MP)")
	set(CMAKE_CXX_STACK_SIZE "100000000") #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:100000000")
	message(STATUS "Set stack size: 100000000 bytes")

	CHECK_CXX_COMPILER_FLAG(/std:c++20 COMPILER_HAS_STD_CXX20)

	if (COMPILER_HAS_STD_CXX20)
		set(CMAKE_CXX_STANDARD 20)
		set(CMAKE_CXX_STANDARD_REQUIRED ON)
		message(STATUS "Set default cxx standard: C++20")
	elseif (MSVC_VERSION GREATER_EQUAL "1900")
		set(CMAKE_CXX_STANDARD 17)
		set(CMAKE_CXX_STANDARD_REQUIRED ON)
		message(STATUS "Set default cxx standard: C++17")
	endif()
elseif(WIN32)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

	add_definitions(
		-DWIN32_LEAN_AND_MEAN
		-D_WIN32_WINNT=0x0601
		-DNOMINMAX
		-DUNICODE
		-D_UNICODE
		-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
	)

	add_definitions(
		-DBOOST_ALL_STATIC_LINK
		-DBOOST_THREAD_USE_LIB
		-DBOOST_FILESYSTEM_STATIC_LINK
		-DBOOST_USE_WINAPI_VERSION=0x0601
	)

	if (NOT MINGW)
		add_compile_options(/utf8)
	endif()
endif()

################################################################################

CHECK_CXX_COMPILER_FLAG(-fvisibility-inlines-hidden COMPILER_HAS_VISIBILITY_INLINE_HIDDEN)
CHECK_CXX_COMPILER_FLAG(-fvisibility=hidden COMPILER_HAS_VISIBILITY_HIDDEN)
CHECK_CXX_COMPILER_FLAG(-fdiagnostics-color=always COMPILER_HAS_COLOR)
CHECK_CXX_COMPILER_FLAG(-fcoroutines COMPILER_HAS_FCOROUTINES)
CHECK_CXX_COMPILER_FLAG(-fcoroutines-ts COMPILER_HAS_FCOROUTINES_TS)
CHECK_CXX_COMPILER_FLAG(-std=c++20 COMPILER_HAS_STD_CXX20)

if (${COMPILER_HAS_VISIBILITY_INLINE_HIDDEN})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
endif()

if (${COMPILER_HAS_VISIBILITY_HIDDEN})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

if (${COMPILER_HAS_COLOR})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
endif()

if (COMPILER_HAS_FCOROUTINES)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines")
endif()

if (COMPILER_HAS_FCOROUTINES_TS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts")
endif()

if (ENABLE_SYSTEMD_LOGGING)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_SYSTEMD_LOGGING")
endif()

################################################################################

if (NOT MSVC)
	if (NOT APPLE)
		if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
			set(FMT_MODULE ON)
			if (ENABLE_BUILD_WITH_LIBCXX)
				add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>)
				add_link_options(-stdlib=libc++)
				add_definitions(-DUSE_STD_STRING_VIEW)

				CHECK_LINKER_FLAG(CXX "-rtlib=compiler-rt" IS_COMPILER_RT_SUPPORTED)

				if (IS_COMPILER_RT_SUPPORTED)
					message(STATUS "clang supports compiler-rt, use it")
					add_link_options(-rtlib=compiler-rt)
				endif()
			else()
				set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
				if (ENABLE_STATIC_LINK_TO_GCC)
					set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc")
				endif()
			endif()
		elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
			if (ENABLE_STATIC_LINK_TO_GCC)
				set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc")
			endif()
		endif()
	endif()
	if (ANDROID)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIE -pie")
	endif()
	if (COMPILER_HAS_STD_CXX20)
		set(CMAKE_CXX_STANDARD 20)
		add_definitions(-DUSE_STD_STRING_VIEW)
		message(STATUS "Set default cxx standard: C++20")
	else()
		message(FATAL_ERROR "need at least GCC 11 or clang 14")
	endif()

	if (ENABLE_MOLD)
		if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
			if (MOLD_LINKER)
				list(APPEND CMAKE_EXE_LINKER_FLAGS "-fuse-ld=mold")
			endif()
		endif()
	endif()

	if (ENABLE_LLD)
		if (LLD_LINKER)
			list(APPEND CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld")
		endif()
	endif()

endif(NOT MSVC)

################################################################################

if (ENABLE_SYSTEM_OPENSSL)
	find_package(OpenSSL)
else()
	set(OPENSSL_FOUND TRUE)
	set(OPENSSL_ROOT_DIR
		${CMAKE_CURRENT_SOURCE_DIR}/third_party/openssl
		${CMAKE_BINARY_DIR}/third_party/openssl/crypto/Debug
		${CMAKE_BINARY_DIR}/third_party/openssl/ssl/Debug
		${CMAKE_BINARY_DIR}/third_party/openssl/crypto/Release
		${CMAKE_BINARY_DIR}/third_party/openssl/ssl/Release
	)

	add_subdirectory(third_party/openssl)
	if (MSVC)
		add_definitions(-DSIXTY_FOUR_BIT)
	else()
		add_definitions(-DSIXTY_FOUR_BIT_LONG)
	endif()
	if (NOT MSVC AND NOT APPLE)
		if (NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
			set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--exclude-libs,ALL ${CMAKE_SHARED_LINKER_FLAGS}")
			set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--exclude-libs,ALL ${CMAKE_MODULE_LINKER_FLAGS}")
			set(CMAKE_EXE_LINKER_FLAGS "-Wl,--exclude-libs,ALL ${CMAKE_EXE_LINKER_FLAGS}")
		endif()
	endif()
endif()
include_directories(${OPENSSL_INCLUDE_DIR})

################################################################################

set(BUILD_ASIO_BEAST ON)
set(USE_BOOST_FILESYSTEM ON)

if (ENABLE_BUILD_TESTS)
	set(USE_BOOST_TEST ON)
endif()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)

#独立编译asio时, 必须要在使用者项目定义, 否则会将
#定义BOOST_ASIO_HEADER_ONLY, 使用者将会导致符号冲突.
add_definitions(-DBOOST_ASIO_SEPARATE_COMPILATION)
add_definitions(-DBOOST_BEAST_SEPARATE_COMPILATION)

#此BOOST_ASIO_DYN_LINK只有在编译asio为动态链接的时候
#才能启用, 它在msvc下将BOOST_ASIO_DECL分别自动作为
#dllimport、dllexport添加.
#add_definitions(-DBOOST_ASIO_DYN_LINK)

add_definitions(-DBOOST_LOCALE_HIDE_AUTO_PTR)
add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)
add_definitions(-DBOOST_DISABLE_PRAGMA_MESSAGE)
add_definitions(-DBOOST_COROUTINES_NO_DEPRECATION_WARNING)
add_definitions(-DBOOST_PROCESS_USE_STD_FS)
if (ENABLE_USE_IO_URING)
	find_package(IOUring)

	if (IOUring_FOUND)
		message(STATUS "Linux using io_uring...")
		add_definitions(-DBOOST_ASIO_HAS_IO_URING -DBOOST_ASIO_DISABLE_EPOLL)
		link_libraries(${IOUring_LIBRARIES})
	endif()
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	add_definitions(-DBOOST_ASIO_HAS_CO_AWAIT)
endif()

set(Boost_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost)
add_subdirectory(third_party/boost EXCLUDE_FROM_ALL)

################################################################################
find_package(Threads)
link_libraries(${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})

if (ENABLE_TCMALLOC_STATIC)
	find_package(Tcmalloc)
	link_libraries(${Tcmalloc_LIBRARY})
endif()

if (ENABLE_JEMALLOC_STATIC)
	find_package(JeMalloc)
	link_libraries(${JEMALLOC_LIBRARIES})
endif()

################################################################################

if (WIN32 OR NOT ENABLE_SYSTEM_ZLIB)
	add_subdirectory(third_party/zlib EXCLUDE_FROM_ALL)
	set(ZLIB_LIBRARIES zlibstatic)
	set(ZLIB_LIBRARY ${ZLIB_LIBRARIES})
	set(ZLIB_INCLUDE_DIRS
		${CMAKE_BINARY_DIR}/third_party/zlib
		${CMAKE_CURRENT_SOURCE_DIR}/third_party/zlib
		${CMAKE_CURRENT_SOURCE_DIR}/third_party/zlib/contrib/minizip
	)
	set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIRS})
	set(ZLIB_FOUND ON)
else()
	find_package(ZLIB REQUIRED)
endif()

include_directories(${ZLIB_INCLUDE_DIRS})

################################################################################

if (ENABLE_SYSTEM_LIBSSH2)
	find_package(Libssh2 REQUIRED)
else()
	#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(ENABLE_DEBUG_LOGGING ON)
	#endif()

	set(BUILD_STATIC_LIBS ON)

	set(BUILD_SHARED_LIBS OFF)
	set(BUILD_EXAMPLES OFF)
	set(BUILD_TESTING OFF)
	set(PICKY_COMPILER OFF)
	set(RUN_DOCKER_TESTS OFF)
	set(RUN_SSHD_TESTS OFF)

	add_subdirectory(third_party/libssh2)
	set(LIBSSH2_LIBRARIES libssh2_static)
	set_target_properties(libssh2_static PROPERTIES FOLDER "third_party")
	set_target_properties(libssh2_static PROPERTIES POSITION_INDEPENDENT_CODE ON)
	set(LIBSSH2_INCLUDE_DIRS
		${CMAKE_CURRENT_SOURCE_DIR}/third_party/libssh2/include
		${CMAKE_BINARY_DIR}/third_party/libssh2/src
	)
	set(LIBSSH2_FOUND ON)
endif()

################################################################################

add_subdirectory(third_party/fmt)

################################################################################

set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTS OFF)
set(USE_NSEC OFF)
set(USE_HTTPS ON)
set(USE_SSH ON)
set(SONAME OFF)
set(USE_NTLMCLIENT OFF)
set(USE_ICONV OFF)
set(USE_BUNDLED_ZLIB OFF)
set(GIT_SSH ON)
set(USE_SHA1 "OpenSSL")
set(USE_SHA256 "OpenSSL")
set(REGEX_BACKEND "builtin")

add_definitions(-DGIT_OPENSSL)
add_definitions(-DGIT_SSH)
add_definitions(-DGIT_HTTPS)

include_directories(${ZLIB_INCLUDE_DIR})
include_directories(${LIBSSH2_INCLUDE_DIRS})

add_subdirectory(third_party/libgit2 EXCLUDE_FROM_ALL)
set(LIBGIT2_LIBRARIES pcre http-parser util libgit2)
set(GIT2_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libgit2/include)
set_target_properties(libgit2 PROPERTIES FOLDER "third_party/")
if (TARGET pcre)
	set_target_properties(pcre PROPERTIES FOLDER "third_party/")
endif()
set_target_properties(http-parser PROPERTIES FOLDER "third_party/")
set_target_properties(util PROPERTIES FOLDER "third_party/")

################################################################################

include_directories(
	${GIT2_INCLUDE_DIRS}
)

################################################################################

link_libraries(
		${ZLIB_LIBRARIES}

		Boost::thread
		Boost::date_time
		Boost::program_options
		Boost::filesystem

		fmt

		${LIBGIT2_LIBRARIES}
		${LIBSSH2_LIBRARIES}
		${OPENSSL_LIBRARIES}
)

################################################################################

if (WIN32)

	if (MINGW)
		link_libraries(
			ws2_32
			mswsock
			bcrypt
		)
	else()
		link_libraries(
			Secur32.lib
			Bcrypt.lib
			Winmm.lib
			Mswsock.lib
		)
		link_libraries(
			Mfuuid.lib
			Strmiids.lib
			Mfplat.lib
		)
	endif()
endif()

################################################################################

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)
endif()

################################################################################

add_subdirectory(incubator)
add_subdirectory(autogit)

################################################################################

configure_file(autogit.service.in autogit.service)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/autogit.service DESTINATION /usr/lib/systemd/system/)
