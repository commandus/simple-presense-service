cmake_minimum_required (VERSION 3.8)

# ------------------------ CHANGE PATH BELOW ON WINDOWS ------------------------
set(VCPKG_INC "/git/vcpkg/installed/x64-windows/include")
set(VCPKG_LIB "/git/vcpkg/installed/x64-windows/lib") 
# ------------------------ CHANGE PATH ABOVE ON WINDOWS ------------------------
include_directories(${VCPKG_INC})
link_directories(${VCPKG_LIB})

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

set(VERSION_INFO 1.0.0)
set(CMAKE_CXX_STANDARD 11)

project ("simple-presence-service" VERSION ${VERSION_INFO})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(LIBUVA $<IF:$<TARGET_EXISTS:libuv::uv_a>,libuv::uv_a,uv>)
	# avoid Error LNK2038 mismatch detected for 'RuntimeLibrary': value 'MT_StaticRelease' doesn't match value 'MD_DynamicRelease'
	# set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
	# set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
	set (OS_SPECIFIC_LIBS wsock32 ws2_32 Userenv)
else()
	set(LIBUVA LibUV::LibUV)
endif()
find_package(LibUV)


add_executable(simple-presence-service
	simple-presence-service.cpp
	uv-listener.cpp err-msg.cpp
	presence.cpp mem-presence.cpp presence-item.cpp
	third-party/argtable3/argtable3.c
	third-party/daemonize.cpp
	third-party/uv-mem.cpp
	third-party/ip-address.cpp
	third-party/ip-helper.cpp
	third-party/uv-ip-helper.cpp
)
target_include_directories(simple-presence-service PRIVATE "third-party" "." ${VCPKG_INC} ${Intl_INCLUDE_DIRS})
target_link_libraries(simple-presence-service PRIVATE ${OS_SPECIFIC_LIBS} ${LIBUVA})

add_executable(simple-presence-client
	simple-presence-client.cpp
	uv-client.cpp err-msg.cpp
	presence.cpp mem-presence.cpp presence-item.cpp
	third-party/argtable3/argtable3.c
	third-party/daemonize.cpp
	third-party/uv-mem.cpp
	third-party/ip-address.cpp
	third-party/ip-helper.cpp
	third-party/uv-ip-helper.cpp
)
target_include_directories(simple-presence-client PRIVATE "third-party" "." ${VCPKG_INC} ${Intl_INCLUDE_DIRS})
target_link_libraries(simple-presence-client PRIVATE ${OS_SPECIFIC_LIBS} ${LIBUVA})
