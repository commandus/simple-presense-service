cmake_minimum_required (VERSION 3.8)

# ------------------------ CHANGE PATH BELOW ON WINDOWS ------------------------
set(VCPKG_INC "/git/vcpkg/installed/x64-windows/include")
# ------------------------ CHANGE PATH ABOVE ON WINDOWS ------------------------
	
# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include_directories(${VCPKG_INC})

set(VERSION_INFO 1.0.0)

project ("simple-presense-service" VERSION ${VERSION_INFO})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(LIBUV LibUV)
	set(LIBUVA $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,uv>)
	# avoid Error LNK2038 mismatch detected for 'RuntimeLibrary': value 'MT_StaticRelease' doesn't match value 'MD_DynamicRelease'
	# set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
	# set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
	set (OS_SPECIFIC_LIBS wsock32 ws2_32 Userenv)
else()
	set(LIBUV LibUV)
	set(LIBUVA LibUV::LibUV)
endif()
find_package(${LIBUV})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable (simple-presense-service 
	simple-presense-service.cpp
	uv-listener.cpp
	third-party/argtable3/argtable3.c
	third-party/daemonize.cpp
	third-party/uv-mem.cpp
	third-party/ip-address.cpp
	third-party/ip-helper.cpp
)
target_include_directories(simple-presense-service PRIVATE "third-party" "." ${VCPKG_INC} ${Intl_INCLUDE_DIRS})
target_link_libraries(simple-presense-service PRIVATE ${OS_SPECIFIC_LIBS} ${LIBUVA})

set(CMAKE_CXX_STANDARD 11)

# TODO: Add tests and install targets if needed.
