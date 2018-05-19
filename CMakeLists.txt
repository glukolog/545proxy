CMAKE_MINIMUM_REQUIRED(VERSION 3.1)

set(proname "afree-cpuminer-proxy") # afree-cpuminer-proxy

PROJECT(${proname})

# 指定目标文件支持xp系统
IF(WIN32)
  IF (MSVC)
    set(CMAKE_MSVC_IDE "vs2017")
    IF (MSVC_VERSION EQUAL 1700)
	  set(CMAKE_MSVC_IDE "vs2012")
      #set(CMAKE_GENERATOR_TOOLSET "v110_xp" CACHE STRING "Platform Toolset" FORCE)
	ELSEIF (MSVC_VERSION EQUAL 1800)
	  set(CMAKE_MSVC_IDE "vs2013")
      #set(CMAKE_GENERATOR_TOOLSET "v120_xp" CACHE STRING "Platform Toolset" FORCE)
    ELSEIF (MSVC_VERSION EQUAL 1900)
	  set(CMAKE_MSVC_IDE "vs2015")
	  #set(CMAKE_GENERATOR_TOOLSET "v140_xp" CACHE STRING "Platform Toolset" FORCE)
    ELSEIF (MSVC_VERSION EQUAL 2000)
	  set(CMAKE_MSVC_IDE "vs2017")
	  #set(CMAKE_GENERATOR_TOOLSET "v141_xp" CACHE STRING "Platform Toolset" FORCE)
    ENDIF()
  ENDIF(MSVC)
ENDIF(WIN32)

set(afree-cpuminer-proxy_SOURCES
	conf.c
	main.c
	miner.c
	pool.c
	proxy.c
	sha2.c
	stratum.c
	#test.c
	util.c
	afree-cpuminer-proxy.rc
	afree-cpuminer-proxy.conf)

set(afree-cpuminer-proxy_HEADERS
	defs.h
	sha2.h
	stratum.h
	)
	
source_group("Header Files" FILES ${afree-cpuminer-proxy_HEADERS})
source_group("Source Files" FILES ${afree-cpuminer-proxy_SOURCES})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/libuv/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/jansson/include)

IF(WIN32)
  IF (MSVC)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(CMAKE_PLATFORM "x64")
	else()
		set(CMAKE_PLATFORM "win32")
	endif()

	LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../lib/jansson/lib/${CMAKE_MSVC_IDE}/${CMAKE_PLATFORM}/)
	LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../lib/libuv/lib/${CMAKE_MSVC_IDE}/${CMAKE_PLATFORM}/)
	LINK_LIBRARIES(jansson.lib libuv.lib Ws2_32.lib Psapi.lib Iphlpapi.lib Userenv.lib)

  ENDIF()
ENDIF()
add_executable(${proname}
	${afree-cpuminer-proxy_HEADERS}
	${afree-cpuminer-proxy_SOURCES})

if (WIN32)
  if(MSVC)
    # 增加一些编译属性的方法
    target_compile_options(${proname} PRIVATE
        #/MP
        #/MX
        /wd4996
        #/wd4503
        )
		
	if(NOT EXISTS "${CMAKE_BINARY_DIR}/ALL_BUILD.vcxproj.user")
		file(GENERATE
			OUTPUT "${CMAKE_BINARY_DIR}/ALL_BUILD.vcxproj.user"
			INPUT "${CMAKE_SOURCE_DIR}/cmake/ALL_BUILD.vcxproj.user.in")
	endif()
	if(NOT EXISTS "${CMAKE_BINARY_DIR}/afree-cpuminer-proxy.vcxproj.user")
		file(GENERATE
			OUTPUT "${CMAKE_BINARY_DIR}/afree-cpuminer-proxy.vcxproj.user"
			INPUT "${CMAKE_SOURCE_DIR}/cmake/afree-cpuminer-proxy.vcxproj.user.in")
	endif()
	add_custom_command(TARGET ${proname}
	POST_BUILD
	COMMAND copy /y "\"${CMAKE_SOURCE_DIR}\\${proname}.conf\"" "\"${CMAKE_BINARY_DIR}\\\$(Configuration)\\${proname}.conf\"")
  endif()
endif()
