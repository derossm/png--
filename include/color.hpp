/*
 * Copyright (C) 2007,2008 Alex Shulgin
 *
 * This file is part of png++ the C++ wrapper for libpng. PNG++ is free
 * software; the exact copying conditions are as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef PNGPP_COLOR_HPP_INCLUDED
#define PNGPP_COLOR_HPP_INCLUDED

#pragma once

#include <spdlog/spdlog.h>

#include "types.hpp"

namespace png
{

/**
 * \brief PNG color struct extension. Adds constructors.
 */
struct color : public png_color
{
	inline constexpr explicit color(byte r = 0, byte g = 0, byte b = 0) noexcept
	{
		this->red = r;
		this->green = g;
		this->blue = b;

		if constexpr(enable_logging)
		{
			spdlog::info("constructor");
		}
	}

	/**
	 * \brief Initializes color with a copy of png_color object.
	 */
	inline constexpr color(const png_color& other) noexcept
	{
		this->red = other.red;
		this->green = other.green;
		this->blue = other.blue;

		if constexpr(enable_logging)
		{
			spdlog::info("png_color:ctor");
		}
	}

	// Rule of Five
	inline constexpr color(png_color&& other) noexcept
	{
		this->red = other.red;
		this->green = other.green;
		this->blue = other.blue;

		if constexpr(enable_logging)
		{
			spdlog::info("png_color:mtor");
		}
	}

	inline constexpr color& operator=(const png_color& other) noexcept
	{
		this->red = other.red;
		this->green = other.green;
		this->blue = other.blue;

		if constexpr(enable_logging)
		{
			spdlog::info("png_color:cass");
		}

		return *this;
	}

	inline constexpr color& operator=(png_color&& other) noexcept
	{
		this->red = other.red;
		this->green = other.green;
		this->blue = other.blue;

		if constexpr(enable_logging)
		{
			spdlog::info("png_color:mass");
		}

		return *this;
	}

	inline constexpr color(const color& other) noexcept
	{
		this->red = other.red;
		this->green = other.green;
		this->blue = other.blue;

		if constexpr(enable_logging)
		{
			spdlog::info("color:ctor");
		}
	}

	inline constexpr color(color&& other) noexcept
	{
		this->red = other.red;
		this->green = other.green;
		this->blue = other.blue;

		if constexpr(enable_logging)
		{
			spdlog::info("color:mtor");
		}
	}

	inline constexpr color& operator=(const color& other) noexcept
	{
		this->red = other.red;
		this->green = other.green;
		this->blue = other.blue;

		if constexpr(enable_logging)
		{
			spdlog::info("color:cass");
		}

		return *this;
	}

	inline constexpr color& operator=(color&& other) noexcept
	{
		this->red = other.red;
		this->green = other.green;
		this->blue = other.blue;

		if constexpr(enable_logging)
		{
			spdlog::info("color:mass");
		}

		return *this;
	}

	inline constexpr ~color() noexcept = default;

	inline constexpr auto operator<=>(const color& other) const noexcept = default;
};

} // namespace png

#endif // PNGPP_COLOR_HPP_INCLUDED
