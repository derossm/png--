/**********************************************************************************************************************************************\
	Copyright© 2021 Mason DeRoss

	Released under either the GNU All-permissive License or MIT license. You pick.

	Copying and distribution of this file, with or without modification, are permitted in any medium without royalty,
	provided the copyright notice and this notice are preserved. This file is offered as-is, without any warranty.

	DESCRIPTION:

\**********************************************************************************************************************************************/
#include "../include/stdafx.h"
#include <fmt/format.h>
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

TEST_CASE("Some Tests", "[PNGPP]")
{
	color testObjNone;
	color testObjRGB{1, 2, 3};
	color testObjCtor{testObjRGB};
	color testObjMtor{std::move(testObjCtor)};
	std::cout << ::fmt::format("red: {:b}", testObjNone.red) << ::fmt::format(" green: {:b}", testObjNone.green) << ::fmt::format(" blue: {:b}", testObjNone.blue) << "\n";
	std::cout << ::fmt::format("red: {:b}", testObjRGB.red) << ::fmt::format(" green: {:b}", testObjRGB.green) << ::fmt::format(" blue: {:b}", testObjRGB.blue) << "\n";
	std::cout << ::fmt::format("red: {:b}", testObjCtor.red) << ::fmt::format(" green: {:b}", testObjCtor.green) << ::fmt::format(" blue: {:b}", testObjCtor.blue) << "\n";
	std::cout << ::fmt::format("red: {:b}", testObjMtor.red) << ::fmt::format(" green: {:b}", testObjMtor.green) << ::fmt::format(" blue: {:b}", testObjMtor.blue) << "\n";
	// never use std::endl, it flushes stream every call

	png_struct s1;
	png_row_info ri1;
	byte* bptr = new byte[16];

	for (auto i = 0; i < 9; ++i)
	{
		bptr[i] = i + 128;
	}
	for (auto i = 9; i < 16; ++i)
	{
		bptr[i] = 0;
	}

	ri1.rowbytes = 16;
	::png::detail::convert_color_space_impl<rgb_pixel>::expand_8_to_16(&s1, &ri1, bptr);
	delete [] bptr;
}

}