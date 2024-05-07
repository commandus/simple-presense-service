SUBDIRS = .
ACLOCAL_AMFLAGS = -I m4
VERSION = 1.0.0
VERSION_INFO = 1:0:0

# ../../third-party for make distcheck happy
common_flags = -I. -Ithird-party -I../../third-party

AM_CPPFLAGS=$(common_flags)
AM_CXXFLAGS=$(common_flags) -std=c++11
# COMMON_CPP_FLAGS = -D_GLIBCXX_USE_CXX11_ABI=0

ARGTABLE_SRC = third-party/argtable3/argtable3.c

#
# Binaries
#
bin_PROGRAMS = simple-presense-service

nobase_dist_include_HEADERS = \
	config.h \
	err-msg.h presence.h mem-presence.h uv-client.h uv-listener.h \
	third-party/argtable3/argtable3.h third-party/strptime.h \
    third-party/daemonize.h \
	third-party/ip-helper.h third-party/ip-address.h \
	third-party/uv-mem.h third-party/uv-ip-helper.h

simple_presence_service_SOURCES =
	simple-presence-service.cpp \
	uv-listener.cpp err-msg.cpp \
	presence.cpp presence-item.cpp mem-presence.cpp \
	third-party/argtable3/argtable3.c \
	third-party/daemonize.cpp \
	third-party/uv-mem.cpp \
	third-party/ip-address.cpp \
	third-party/ip-helper.cpp

simple_presence_service_LDADD = $(EXTRA_LIB) -L. -luv
simple_presence_service_CPPFLAGS =

#
# Configs, readme, CMake etc.
#
configdir = $(datadir)
dist_config_DATA = \
	autogen.sh CMakeLists.txt CODE_OF_CONDUCT.md CONTRIBUTING.md COPYING HISTORY LICENSE README.md TODO \
	third-party/argtable3/README
