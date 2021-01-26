/*
 * Copyright (C) 2008 Alex Shulgin
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
#ifndef PNGPP_CONFIG_HPP_INCLUDED
#define PNGPP_CONFIG_HPP_INCLUDED

#pragma once

#include <stdint.h>

namespace png
{

#ifdef PNG_gAMA_SUPPORTED
	static inline constexpr const bool png_gama_supported{true};
#else
	static inline constexpr const bool png_gama_supported{false};
#endif

#ifdef PNG_FLOATING_POINT_SUPPORTED
	static inline constexpr const bool png_floating_point_supported{true};
#else
	static inline constexpr const bool png_floating_point_supported{false};
#endif

#ifdef PNG_1_0_X
	static inline constexpr const bool png_1_0_x{true};
#else
	static inline constexpr const bool png_1_0_x{false};
#endif

#ifdef PNG_tRNS_SUPPORTED
	static inline constexpr const bool png_trns_supported{true};
#else
	static inline constexpr const bool png_trns_supported{false};
#endif

#ifdef PNG_READ_16_TO_8_SUPPORTED
	static inline constexpr const bool png_read_16_to_8_supported{true};
#else
	static inline constexpr const bool png_read_16_to_8_supported{false};
#endif

#ifdef PNG_READ_EXPAND_SUPPORTED
	static inline constexpr const bool png_read_expand_supported{true};
#else
	static inline constexpr const bool png_read_expand_supported{false};
#endif

#ifdef PNG_READ_STRIP_ALPHA_SUPPORTED
	static inline constexpr const bool png_read_strip_alpha_supported{true};
#else
	static inline constexpr const bool png_read_strip_alpha_supported{false};
#endif

#ifdef PNG_READ_BGR_SUPPORTED
	static inline constexpr const bool png_read_bgr_supported{true};
#else
	static inline constexpr const bool png_read_bgr_supported{false};
#endif

#ifdef PNG_WRITE_BGR_SUPPORTED
	static inline constexpr const bool png_write_bgr_supported{true};
#else
	static inline constexpr const bool png_write_bgr_supported{false};
#endif

#ifdef PNG_READ_RGB_TO_GRAY_SUPPORTED
	static inline constexpr const bool png_read_rgb_to_gray_supported{true};
#else
	static inline constexpr const bool png_read_rgb_to_gray_supported{true};
#endif

#ifdef PNG_READ_GRAY_TO_RGB_SUPPORTED
	static inline constexpr const bool png_read_gray_to_rgb_supported{true};
#else
	static inline constexpr const bool png_read_gray_to_rgb_supported{false};
#endif

#ifdef PNG_READ_SWAP_ALPHA_SUPPORTED
	static inline constexpr const bool png_read_swap_alpha_supported{true};
#else
	static inline constexpr const bool png_read_swap_alpha_supported{false};
#endif

#ifdef PNG_WRITE_SWAP_ALPHA_SUPPORTED
	static inline constexpr const bool png_write_swap_alpha_supported{true};
#else
	static inline constexpr const bool png_write_swap_alpha_supported{false};
#endif

#ifdef PNG_READ_INVERT_ALPHA_SUPPORTED
	static inline constexpr const bool png_read_invert_alpha_supported{true};
#else
	static inline constexpr const bool png_read_invert_alpha_supported{false};
#endif

#ifdef PNG_WRITE_INVERT_ALPHA_SUPPORTED
	static inline constexpr const bool png_write_invert_alpha_supported{true};
#else
	static inline constexpr const bool png_write_invert_alpha_supported{false};
#endif

#ifdef PNG_READ_FILLER_SUPPORTED
	static inline constexpr const bool png_read_filler_supported{true};
#else
	static inline constexpr const bool png_read_filler_supported{false};
#endif

#ifdef PNG_WRITE_FILLER_SUPPORTED
	static inline constexpr const bool png_write_filler_supported{true};
#else
	static inline constexpr const bool png_write_filler_supported{false};
#endif

#ifdef PNG_READ_PACK_SUPPORTED
	static inline constexpr const bool png_read_pack_supported{true};
#else
	static inline constexpr const bool png_read_pack_supported{false};
#endif

#ifdef PNG_WRITE_PACK_SUPPORTED
	static inline constexpr const bool png_write_pack_supported{true};
#else
	static inline constexpr const bool png_write_pack_supported{false};
#endif

#ifdef PNG_READ_PACKSWAP_SUPPORTED
	static inline constexpr const bool png_read_packswap_supported{true};
#else
	static inline constexpr const bool png_read_packswap_supported{false};
#endif

#ifdef PNG_WRITE_PACKSWAP_SUPPORTED
	static inline constexpr const bool png_write_packswap_supported{true};
#else
	static inline constexpr const bool png_write_packswap_supported{false};
#endif

#ifdef PNG_READ_SHIFT_SUPPORTED
	static inline constexpr const bool png_read_shift_supported{true};
#else
	static inline constexpr const bool png_read_shift_supported{false};
#endif

#ifdef PNG_WRITE_SHIFT_SUPPORTED
	static inline constexpr const bool png_write_shift_supported{true};
#else
	static inline constexpr const bool png_write_shift_supported{false};
#endif

#ifdef PNG_READ_INVERT_SUPPORTED
	static inline constexpr const bool png_read_invert_supported{true};
#else
	static inline constexpr const bool png_read_invert_supported{false};
#endif

#ifdef PNG_WRITE_INVERT_SUPPORTED
	static inline constexpr const bool png_write_invert_supported{true};
#else
	static inline constexpr const bool png_write_invert_supported{false};
#endif

#ifdef PNG_READ_USER_TRANSFORM_SUPPORTED
	static inline constexpr const bool png_read_user_transform_supported{true};
#else
	static inline constexpr const bool png_read_user_transform_supported{false};
#endif

#ifdef PNG_WRITE_USER_TRANSFORM_SUPPORTED
	static inline constexpr const bool png_write_user_transform_supported{true};
#else
	static inline constexpr const bool png_write_user_transform_supported{false};
#endif

#ifdef PNG_READ_SWAP_SUPPORTED
constexpr const bool png_read_swap_supported{true};
#else
constexpr const bool png_read_swap_supported{false};
#endif

#ifdef PNG_WRITE_SWAP_SUPPORTED
	static inline constexpr const bool png_write_swap_supported{true};
#else
	static inline constexpr const bool png_write_swap_supported{false};
#endif

#ifdef PNG_READ_INTERLACING_SUPPORTED
constexpr const bool png_read_interlacing_supported{true};
#else
constexpr const bool png_read_interlacing_supported{false};
#endif

#ifdef PNG_WRITE_INTERLACING_SUPPORTED
	static inline constexpr const bool png_write_interlacing_supported{true};
#else
	static inline constexpr const bool png_write_interlacing_supported{false};
#endif

static inline constexpr const bool __little_endian{
	[](){
		constexpr const uint16_t bytes{255u}; // fill one byte with 1's, the other byte with zero's
		constexpr const uint16_t test{(bytes & 0xFF00)}; // check which byte is all zero

		// note that if this isn't a valid way to determine endianess, then the system must behave endian neutral to our code and therefore
		// the result is irrelevant since the only time endianess matters to us is when using bitwise operations across byte boundaries
		// (or the compiler is entirely non-conformant, but we don't code for that)
		if constexpr (test == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}()
};

} // end namespace png

#endif // PNGPP_CONFIG_HPP_INCLUDED
