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
#ifndef PNGPP_CONVERT_COLOR_SPACE_HPP_INCLUDED
#define PNGPP_CONVERT_COLOR_SPACE_HPP_INCLUDED

#pragma once

#include <algorithm>
#include <span>

#include <spdlog/spdlog.h>

#include "error.hpp"
#include "rgb_pixel.hpp"
#include "rgba_pixel.hpp"
#include "gray_pixel.hpp"
#include "ga_pixel.hpp"
#include "index_pixel.hpp"
#include "reader.hpp"
#include "writer.hpp"

namespace png::detail
{

/**
* \brief IO transformation class template. Converts %image %color space.
*/
template<typename pixel>
struct convert_color_space_impl
{
	using traits = pixel_traits<pixel>;
	using component_type = typename traits::component_type;
	using alpha_traits = basic_alpha_pixel_traits<component_type>;

	template<typename reader>
	inline constexpr void operator()(reader& io) const noexcept
	{
		handle_16(io);
		handle_alpha(io, alpha_traits::get_alpha_filler());
		handle_palette(io);
		handle_rgb(io);
		handle_gray(io);

		io.set_color_type(traits::get_color_type());
		io.set_bit_depth(traits::get_bit_depth());
	}

//protected:

	// requirement: row_info->rowbytes = 2x the current row length = n, contents of row are n elements
	// expand row by interlacing the original row with a zero'd row of the same length into a 2*length row
	// insert gaps by moving the (n)th item to index 2*n, then zero index 2*(n) - 1, then move (n-1)th item to index 2*(n-1)...
	// Note: this seems like a zip algorithm, and there is room to optimize now that it's algorithmized
	// result: expanded row starts with `expanded index[0]` = 0, `expanded index[1]` = `original index[0]`
	static inline constexpr void expand_8_to_16(png_struct* /*unused?*/, png_row_info* row_info, byte* row) noexcept
	{
#ifdef DEBUG_EXPAND_8_16
		printf("row: width=%d, bytes=%d, channels=%d\n", row_info->width, row_info->rowbytes, row_info->channels);
		printf("<= ");
		dump_row(row, row_info->rowbytes);
#endif
		//for (uint32_t i = row_info->rowbytes; i-- > 0; )
		//{
		//	row[2 * i + 1] = row[i];
		//	row[2 * i + 0] = 0;
		//}
		auto expanded{::std::span(reinterpret_cast<uint16_t*>(row), row_info->rowbytes/2)};
		auto compact{::std::span(row, row_info->rowbytes/2)};
		// need to do this in reverse order, otherwise we could use the ranges version and avoid naming these temp objects
		::std::transform(expanded.rbegin(), expanded.rend(), compact.rbegin(), expanded.rbegin(),
			[](auto& lhs, auto& rhs)
			{
				static_assert(sizeof(lhs) == 2*sizeof(byte));
				if constexpr (__little_endian)
				{
					lhs = rhs;
				}
				else
				{
					// I think this will work for BIG ENDIAN, but I can't test it atm
					lhs = rhs;
					lhs >>= sizeof(byte);
				}
				return lhs;
			});

		if constexpr(enable_logging)
		{
			for(size_t i{0}; i < row_info->rowbytes; ++i)
			{
				spdlog::info("{:02}: rows after = 0x {:02x}", i, row[i]);
			}
		}

#ifdef DEBUG_EXPAND_8_16
		printf("=> ");
		dump_row(row, 2 * row_info->rowbytes);
#endif
	}

#ifdef DEBUG_EXPAND_8_16
	static inline constexpr void dump_row(const byte* row, uint32_t width) noexcept
	{
		printf("{");
		for (uint32_t i = 0; i < width; ++i)
		{
			printf(" %02x,", row[i]);
		}
		printf(" }\n");
	}
#endif

	template<typename reader>
	static inline constexpr void handle_16(reader& io) noexcept
	{
		if (io.get_bit_depth() == 16 && traits::get_bit_depth() == 8)
		{
			if constexpr(png_read_16_to_8_supported)
			{
				io.set_strip_16();
			}
			else
			{
				throw error("expected 8-bit data but found 16-bit; recompile with PNG_READ_16_TO_8_SUPPORTED");
			}
		}

		if (io.get_bit_depth() != 16 && traits::get_bit_depth() == 16)
		{
			if constexpr(png_read_user_transform_supported)
			{
				io.set_read_user_transform(expand_8_to_16);
				io.set_user_transform_info(NULL, 16, traits::get_channels());
			}
			else
			{
				throw error("expected 16-bit data but found 8-bit; recompile with PNG_READ_USER_TRANSFORM_SUPPORTED");
			}
		}
	}

	template<typename reader>
	static inline constexpr void handle_alpha(reader& io, uint32_t filler) noexcept
	{
		bool src_alpha{(io.get_color_type() & color_mask_alpha)};
		bool src_tRNS{io.has_chunk(chunk_tRNS)};
		bool dst_alpha{traits::get_color_type() & color_mask_alpha};
		if ((src_alpha || src_tRNS) && !dst_alpha)
		{
			if constexpr(png_read_strip_alpha_supported)
			{
				io.set_strip_alpha();
			}
			else
			{
				throw error("alpha channel unexpected; recompile with PNG_READ_STRIP_ALPHA_SUPPORTED");
			}
		}
		if (!src_alpha && dst_alpha)
		{
			if constexpr (png_trns_supported && png_read_expand_supported)
			{
				if (src_tRNS)
				{
					io.set_tRNS_to_alpha();
					return;
				}
			}

			if constexpr(png_read_filler_supported && !png_1_0_x)
			{
				io.set_add_alpha(filler, filler_after);
			}
			else
			{
				throw error("expected alpha channel but none found; recompile with PNG_READ_FILLER_SUPPORTED and be sure to use libpng > 1.0.x");}
			}
	}

	template<typename reader>
	static inline constexpr void handle_palette(reader& io) noexcept
	{
		bool src_palette{io.get_color_type() == color_type_palette};
		bool dst_palette{traits::get_color_type() == color_type_palette};
		if (src_palette && !dst_palette)
		{
			if constexpr (png_read_expand_supported)
			{
				io.set_palette_to_rgb();
				io.get_info().drop_palette();
			}
			else
			{
				throw error("indexed colors unexpected; recompile with PNG_READ_EXPAND_SUPPORTED");
			}
		}
		else if (!src_palette && dst_palette)
		{
			throw error("conversion to indexed colors is unsupported");
		}
		else if (src_palette && dst_palette && io.get_bit_depth() != traits::get_bit_depth())
		{
			if (traits::get_bit_depth() == 8)
			{
				if constexpr (png_read_pack_supported)
				{
					io.set_packing();
				}
			}
			else
			{
				throw error("cannot convert to indexed colors with bit-depth < 8");
			}
		}
	}

	template<typename reader>
	static inline constexpr void handle_rgb(reader& io) noexcept
	{
		bool src_rgb{io.get_color_type() & (color_mask_rgb | color_mask_palette)};
		bool dst_rgb{traits::get_color_type() & color_mask_rgb};
		if (src_rgb && !dst_rgb)
		{
			if constexpr (png_read_rgb_to_gray_supported)
			{
				io.set_rgb_to_gray(/*rgb_to_gray_error*/);
			}
			else
			{
				throw error("grayscale data expected; recompile with PNG_READ_RGB_TO_GRAY_SUPPORTED");
			}
		}
		if (!src_rgb && dst_rgb)
		{
			if constexpr (png_read_gray_to_rgb_supported)
			{
				io.set_gray_to_rgb();
			}
			else
			{
				throw error("expected RGB data; recompile with PNG_READ_GRAY_TO_RGB_SUPPORTED");
			}
		}
	}

	template<typename reader>
	static inline constexpr void handle_gray(reader& io) noexcept
	{
		if ((io.get_color_type() & ~color_mask_alpha) == color_type_gray)
		{
			if (io.get_bit_depth() < 8 && traits::get_bit_depth() >= 8)
			{
				if constexpr (png_read_expand_supported)
				{
					io.set_gray_1_2_4_to_8();
				}
				else
				{
					throw error("convert_color_space: expected 8-bit data; recompile with PNG_READ_EXPAND_SUPPORTED");
				}
			}
		}
	}
};

} // namespace png::detail

/**
 * \brief IO transformation class template. Converts %image %color
 * space.
 *
 * This IO transformation class template is used to convert %color
 * space of the source %image to the %color space of the target
 * %image. An error with human-readable description is thrown
 * when the %color space could not be converted. Often, this
 * means that you have to recompile libpng with some more
 * conversion options turned on.
 *
 * Not implemented--see specializations.
 *
 * \see image, image::read
 */
template<typename pixel> struct convert_color_space : png::detail::convert_color_space_impl<pixel> {};

#endif // PNGPP_CONVERT_COLOR_SPACE_HPP_INCLUDED
