# ============================================================================
#  autogit —— 主可执行程序
# ============================================================================

set(CMAKE_CXX_FLAGS_DEBUG     "${CMAKE_CXX_FLAGS_DEBUG}     -DENABLE_LOGGER")
set(CMAKE_CXX_FLAGS_RELEASE   "${CMAKE_CXX_FLAGS_RELEASE}   -DENABLE_LOGGER -DDISABLE_LOGGER_TO_CONSOLE -DLOGGER_DBG_VIEW")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DENABLE_LOGGER -DDISABLE_LOGGER_TO_CONSOLE -DLOGGER_DBG_VIEW")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -DENABLE_LOGGER -DDISABLE_LOGGER_TO_CONSOLE -DLOGGER_DBG_VIEW")

# ---------------------------------------------------------------------------
# 源文件与头文件
# ---------------------------------------------------------------------------

set(autogit_SRCS
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

set(autogit_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/autogit/logging.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/autogit/scoped_exit.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/autogit/strutil.hpp
)

set(watchman_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/watchman/notify_event.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/include/watchman/watchman.hpp
)

set(watchman_windows_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/watchman/windows/windows_watchman.hpp
)

set(watchman_linux_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/watchman/linux/linux_watchman.hpp
)

# ---------------------------------------------------------------------------
# 定义可执行目标
# ---------------------------------------------------------------------------

add_executable(autogit
	${autogit_HEADERS}
	${watchman_HEADERS}
	${watchman_windows_HEADERS}
	${watchman_linux_HEADERS}
	${autogit_SRCS}
)

target_link_libraries(autogit
	Boost::asio
	Boost::bimap
	gitpp
)

target_include_directories(autogit
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ---------------------------------------------------------------------------
# C++ 标准
# ---------------------------------------------------------------------------

if (MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29.30129 AND CMAKE_VERSION VERSION_GREATER 3.20.3)
	set_property(TARGET autogit PROPERTY CXX_STANDARD 23)
	message(STATUS "autogit set cxx standard: C++23")
else()
	set_property(TARGET autogit PROPERTY CXX_STANDARD 20)
	message(STATUS "autogit set cxx standard: C++20")
endif()

# ---------------------------------------------------------------------------
# 平台相关
# ---------------------------------------------------------------------------

if (APPLE)
	find_library(CORESERVICES_LIBRARY CoreServices)
	target_link_libraries(autogit ${CORESERVICES_LIBRARY})
endif()

# ---------------------------------------------------------------------------
# IDE 分组
# ---------------------------------------------------------------------------

source_group("Header Files" FILES ${autogit_HEADERS})
source_group("Header Files/watchman" FILES ${watchman_HEADERS})
source_group("Header Files/watchman/windows" FILES ${watchman_windows_HEADERS})
source_group("Header Files/watchman/linux" FILES ${watchman_linux_HEADERS})
