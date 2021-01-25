################################################################################
# Copyright© 2021 Mason DeRoss
# Released under either the GNU All-permissive License or MIT license. You pick.
#
#
################################################################################

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

cmake_minimum_required(VERSION 3.7.2)
project("PNG++" VERSION 0.0.1 LANGUAGES CXX)

# Autotools variables
set(top_srcdir "${CMAKE_CURRENT_SOURCE_DIR}")
set(top_builddir "${CMAKE_CURRENT_BINARY_DIR}")

# AC_INIT variables
set(PACKAGE_NAME "${PROJECT_NAME}")
set(PACKAGE_SHORTNAME "pngpp")

set(PACKAGE_TARNAME "${PACKAGE_SHORTNAME}")
set(PACKAGE_VERSION "${PROJECT_VERSION}")
set(PACKAGE_STRING "${PROJECT_NAME} v${PROJECT_VERSION}")
set(PACKAGE_BUGREPORT "https://github.com/derossm/pngpp")

set(PACKAGE_COPYRIGHT "Copyright© 2021")
set(PACKAGE_LICENSE "MIT License")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ARCHITECTURE "x64")
set(PLATFORM "windows")

set(VCPKG_BASE_DIR "G:/dev/vcpkg")
set(VCPKG_DIR "${VCPKG_BASE_DIR}/installed")
set(VCPKG_INCLUDE_DIR "${VCPKG_DIR}/${ARCHITECTURE}-${PLATFORM}/include")
set(VCPKG_LIBRARY_DIR "${VCPKG_DIR}/${ARCHITECTURE}-${PLATFORM}/lib")

set(COMMON_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}" "${VCPKG_INCLUDE_DIR}")
set(COMMON_DEFINITIONS "WIN32_LEAN_AND_MEAN" "NOMINMAX")
set(WIN64_COMPILE_OPTIONS
	"/Zc:__cplusplus" "/Zc:inline" "/Zc:preprocessor" "/Zc:wchar_t"
	"/fp:precise" "/GS" "/Gd" "/Gm-" "/FC" "/EHsc" "/nologo" "/WX-" "/MDd" "/MP"
	"/permissive-" "/W4" "/Od" "/arch:AVX2")

#if(MSVC)
#	#add_definitions("/D_CRT_SECURE_NO_WARNINGS" "/D_CRT_SECURE_NO_DEPRECATE" "/D_CRT_NONSTDC_NO_DEPRECATE")
#else()
#	add_compile_options("-Wall" "-Wdeclaration-after-statement" "-Wredundant-decls")
#endif()

# libm or fdlibm (Freely Distributable libm) : C math library IEEE 754 floating-point arithmetic
find_package(m REQUIRED)
if(m_FOUND)
	set(HAVE_LIBM TRUE)
endif()

# zlib : compression library
find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
	set(HAVE_ZLIB TRUE)
	# zstr :
	find_package(zstr REQUIRED)
	if (ZSTR_FOUND)
		set(HAVE_ZSTR TRUE)
	endif()
endif()

# libpng : interface for reading/writing PNG format files
#find_package(libpng REQUIRED)
if(libpng_FOUND)
	set(HAVE_LIBPNG TRUE)
endif()

find_package(WIL CONFIG REQUIRED)
if(WIL_FOUND)
	set(HAVE_WIL TRUE)
endif()

find_package(rapidjson CONFIG REQUIRED)
if(rapidjson_FOUND)
	set(HAVE_RAPIDJSON TRUE)
endif()

set(ms-gsl_DIR "${VCPKG_BASE_DIR}/packages/ms-gsl_x64-windows/share/ms-gsl")
find_package(ms-gsl CONFIG REQUIRED)
if(ms-gsl_FOUND)
	set(HAVE_MS_GSL TRUE)
endif()

#find_package(abseil REQUIRED)
if(abseil_FOUND)
	set(HAVE_ABSEIL TRUE)
endif()

find_package(fmt CONFIG REQUIRED)
if(fmt_FOUND)
	set(HAVE_FMT TRUE)
endif()

configure_file(cmake/config.h.cin config.h)

foreach(SUBDIR tests src)
	add_subdirectory("${SUBDIR}")
endforeach()
