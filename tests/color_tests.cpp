/**********************************************************************************************************************************************\
	Copyright© 2021 Mason DeRoss

	Released under either the GNU All-permissive License or MIT license. You pick.

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#include "../include/stdafx.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <gsl/gsl>
#include <windows.h>

#include "../include/png.hpp"
#include "../include/convert_color_space.hpp"

#define CATCH_CONFIG_MAIN
#pragma warning(push, 0)
#include "catch.hpp"
#pragma warning(pop)

//int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
//{
//	return Catch::Session().run();
//}

namespace png::testing
{

TEST_CASE("color struct tests", "[PNGPP]")
{
	color testObjNone;
	color testObjRGB{1, 2, 3};
	color testObjCtor{testObjRGB};
	color testObjMtor{std::move(testObjCtor)};
	::std::cout	<< "\n";
	::spdlog::info("color object output:");
	::spdlog::info("========================================");
	::std::cout	<< "\t\t\t\t| "
				<< ::fmt::format("red: 0b {:02b} | ", testObjNone.red)
				<< ::fmt::format("green: 0b {:02b} | ", testObjNone.green)
				<< ::fmt::format("blue: 0b {:02b}", testObjNone.blue)
				<< "\n";
	::std::cout	<< "\t\t\t\t| "
				<< ::fmt::format("red: 0b {:02b} | ", testObjRGB.red)
				<< ::fmt::format("green: 0b {:02b} | ", testObjRGB.green)
				<< ::fmt::format("blue: 0b {:02b}", testObjRGB.blue)
				<< "\n";
	::std::cout	<< "\t\t\t\t| "
				<< ::fmt::format("red: 0b {:02b} | ", testObjCtor.red)
				<< ::fmt::format("green: 0b {:02b} | ", testObjCtor.green)
				<< ::fmt::format("blue: 0b {:02b}", testObjCtor.blue)
				<< "\n";
	::std::cout	<< "\t\t\t\t| "
				<< ::fmt::format("red: 0b {:02b} | ", testObjMtor.red)
				<< ::fmt::format("green: 0b {:02b} | ", testObjMtor.green)
				<< ::fmt::format("blue: 0b {:02b}", testObjMtor.blue)
				<< "\n";
	::spdlog::info("========================================\n");
	// never use std::endl, it flushes stream every call

	//::spdlog::info("red: 0b {:02b} ", testObjNone.red);
	//::spdlog::info("green: 0b {:02b} ", testObjNone.green);
	//::spdlog::info("blue: 0b {:02b}", testObjNone.blue);

	//::spdlog::info("red: 0b {:02b} ", testObjRGB.red);
	//::spdlog::info("green: 0b {:02b} ", testObjRGB.green);
	//::spdlog::info("blue: 0b {:02b}", testObjRGB.blue);

	//::spdlog::info("red: 0b {:02b} ", testObjCtor.red);
	//::spdlog::info("green: 0b {:02b} ", testObjCtor.green);
	//::spdlog::info("blue: 0b {:02b}", testObjCtor.blue);

	//::spdlog::info("red: 0b {:02b} ", testObjMtor.red);
	//::spdlog::info("green: 0b {:02b} ", testObjMtor.green);
	//::spdlog::info("blue: 0b {:02b}", testObjMtor.blue);
}

TEST_CASE("convert_color_space_impl tests", "[PNGPP]")
{
	png_struct s1;
	png_row_info ri1;
	byte* bptr = new byte[16];

	for (auto i = 0; i < 8; ++i)
	{
		bptr[i] = i + 147;
	}
	for (auto i = 8; i < 16; ++i)
	{
		bptr[i] = 0;
	}

	ri1.rowbytes = 16;
	::png::detail::convert_color_space_impl<rgb_pixel>::expand_8_to_16(&s1, &ri1, bptr);
	delete [] bptr;
}

}