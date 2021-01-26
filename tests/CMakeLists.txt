################################################################################
# Copyright© 2021 Mason DeRoss 
# Released under either the GNU All-permissive License or MIT license. You pick.
#
# use this to build tests for PNG++
################################################################################

set(DERMA_FILES
											#../include/stdafx.h
											#catch/catch.hpp
		tests.cpp							tests.h
		tests_color.cpp
		tests_convert_color_space.cpp)

set(LIBPNG_DIR "../lpng1637")

add_executable(catch2tests "${DERMA_FILES}")
set_target_properties(catch2tests PROPERTIES CXX_STANDARD 20 LANGUAGE CXX LINKER_LANGUAGE CXX)
target_include_directories(catch2tests PUBLIC "${COMMON_INCLUDE_DIRS}" "../" "catch" "${LIBPNG_DIR}")
target_link_libraries(catch2tests PRIVATE fmt::fmt)
# Microsoft.GSL::GSL
#target_precompile_headers(catch2tests PUBLIC ../include/stdafx.h)

if(MSVC)
	target_compile_options(catch2tests PUBLIC "${WIN64_COMPILE_OPTIONS}")
	target_compile_definitions(catch2tests PUBLIC "${COMMON_DEFINITIONS}")
else()
	# other compilers
endif()
