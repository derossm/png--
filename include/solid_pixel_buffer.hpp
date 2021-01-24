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
#ifndef PNGPP_SOLID_PIXEL_BUFFER_HPP_INCLUDED
#define PNGPP_SOLID_PIXEL_BUFFER_HPP_INCLUDED

#include <ranges>
#include <algorithm>
#include <span>

#include <cassert>
#include <cstddef>
#include <climits>
#include <stdexcept>
#include <vector>

#include "config.hpp"
#include "packed_pixel.hpp"
#include "gray_pixel.hpp"
#include "index_pixel.hpp"

namespace png
{

#ifdef PNGPP_HAS_STD_MOVE
constexpr bool pngpp_has_std_move{true};
#else
constexpr bool pngpp_has_std_move{false};
#endif

#ifdef PNGPP_HAS_STATIC_ASSERT
constexpr bool pngpp_has_static_assert{true};
#else
constexpr bool pngpp_has_static_assert{false};
#endif

/**
 * \brief Pixel buffer, that stores pixels as continuous memory chunk.
 * solid_pixel_buffer is useful when user whats to open png, do some changes and fetch to buffer to draw (as texture for example).
 */
template<typename pixel>
class solid_pixel_buffer
{
public:
	using pixel_traits_t = pixel_traits<pixel>;
	struct row_traits
	{
		using row_access = pixel*;
		using row_const_access = const pixel*;

		static inline constexpr byte* get_data(row_access row) noexcept
		{
			return reinterpret_cast<byte*>(row);
		}
	};

	/**
	 * \brief A row of pixel data.
	 */
	using row_access = typename row_traits::row_access;
	using row_const_access = typename row_traits::row_const_access;
	using row_type = row_access;

	/**
	 * \brief Constructs an empty 0x0 pixel buffer object.
	 */
	inline constexpr solid_pixel_buffer() noexcept = default;

	/**
	 * \brief Constructs an empty pixel buffer object.
	 */
	inline constexpr solid_pixel_buffer(uint_32 width, uint_32 height) noexcept
		: m_width{width}, m_height{height}, m_stride{m_width * bytes_per_pixel}
	{
		m_bytes.reserve(height * m_stride);
	}

	inline constexpr uint_32 get_width() const noexcept
	{
		return m_width;
	}

	inline constexpr uint_32 get_height() const noexcept
	{
		return m_height;
	}

	/**
	 * \brief Resizes the pixel buffer.
	 *
	 * If new width or height is greater than the original, expanded pixels are filled with value of \a pixel().
	 */
	inline constexpr void resize(uint_32 width, uint_32 height) noexcept
	{
		m_width = width;
		m_height = height;
		m_stride = m_width * bytes_per_pixel;
		m_bytes.resize(height * m_stride);
	}

	/**
	 * \brief Returns a reference to the row of image data at specified index.
	 *
	 * Checks the index before returning a row: an instance of std::out_of_range is thrown if \c index is greater than \c height.
	 */
	inline constexpr row_access get_row(size_t index) noexcept
	{
		return reinterpret_cast<row_access>(&m_bytes.at(index * m_stride));
	}

	/**
	 * \brief Returns a const reference to the row of image data at specified index.
	 *
	 * The checking version.
	 */
	inline constexpr row_const_access get_row(size_t index) const noexcept
	{
		return (row_const_access)(&m_bytes.at(index * m_stride));
	}

	/**
	 * \brief The non-checking version of get_row() method.
	 */
	inline constexpr row_access operator[](size_t index) noexcept
	{
		return (row_access)(&m_bytes[index * m_stride]);
	}

	/**
	 * \brief The non-checking version of get_row() method.
	 */
	inline constexpr row_const_access operator[](size_t index) const noexcept
	{
		return (row_const_access)(&m_bytes[index * m_stride]);
	}

	/**
	 * \brief Replaces the row at specified index.
	 */
	inline constexpr void put_row(size_t index, row_const_access r) noexcept
	{
		auto row{get_row(index)};
		/*for (uint_32 i = 0; i < m_width; ++i)
		{
			*row++ = *r++;
		}*/
		// copy the values in r to row
		std::ranges::copy(std::span(r, m_width), std::span(row, m_width).begin());
	}

	/**
	 * \brief Returns a pixel at (x,y) position.
	 */
	inline constexpr pixel get_pixel(size_t x, size_t y) const noexcept
	{
		size_t index{(y * m_width + x) * bytes_per_pixel};
		return *reinterpret_cast<const pixel*>(&m_bytes.at(index));
	}

	/**
	 * \brief Replaces a pixel at (x,y) position.
	 */
	inline constexpr void set_pixel(size_t x, size_t y, pixel p) noexcept
	{
		size_t index{(y * m_width + x) * bytes_per_pixel};
		*reinterpret_cast<pixel*>(&m_bytes.at(index)) = p;
	}

	/**
	 * \brief Provides easy constant read access to underlying byte-buffer.
	 */
	inline constexpr const std::vector<byte>& get_bytes() const noexcept
	{
		return m_bytes;
	}

	/**
	 * \brief Moves the buffer to client code (c++11 only) .
	 */
	inline constexpr std::vector<byte> fetch_bytes() noexcept
	{
		if constexpr (pngpp_has_std_move)
		{
			m_width = 0;
			m_height = 0;
			m_stride = 0;

			// the buffer is moved outside without copying and leave m_bytes empty.
			// NOTE: inhibiting RVO with explicit move of a member variable instead of using an rvalue
			// also leaving m_bytes in a state of undefined behavior if accessed FIXME
			return std::move(m_bytes);
		}
		else
		{
			return std::vector<byte>{};
		}
	}

protected:
	static constexpr const size_t bytes_per_pixel{pixel_traits_t::channels * pixel_traits_t::bit_depth / CHAR_BIT};

protected:
	uint_32 m_width{0};
	uint_32 m_height{0};
	size_t m_stride{0};
	std::vector<byte> m_bytes;

	//if constexpr (pngpp_has_static_assert)
	//{
		static_assert(pixel_traits_t::bit_depth% CHAR_BIT == 0, "Bit_depth should consist of integer number of bytes");

		static_assert(sizeof(pixel)* CHAR_BIT == pixel_traits_t::channels * pixel_traits_t::bit_depth, "pixel type should contain channels data only");
	//}
};

/**
 * \brief solid_pixel_buffer for packed_pixel is not implemented now.
 * Should there be a gap between rows? How to deal with last useless bits in last byte in buffer?
 */
template<int bits>
class solid_pixel_buffer<packed_pixel<bits>>;

} // namespace png

#endif // PNGPP_solid_pixel_buffer_HPP_INCLUDED
