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

namespace png
{

//inline static constexpr const bool pngpp_has_std_move{true};
//inline static constexpr const bool pngpp_has_static_assert{true};

inline static constexpr const bool __little_endian{
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

#ifdef PNG_READ_SWAP_SUPPORTED
constexpr const bool png_read_swap_supported{true};
#else
constexpr const bool png_read_swap_supported{false};
#endif

#ifdef PNG_READ_INTERLACING_SUPPORTED
constexpr const bool png_read_interlacing_supported{true};
#else
constexpr const bool png_read_interlacing_supported{false};
#endif

} // end namespace png

#endif // PNGPP_CONFIG_HPP_INCLUDED
