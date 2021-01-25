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
#ifndef PNGPP_PIXEL_BUFFER_HPP_INCLUDED
#define PNGPP_PIXEL_BUFFER_HPP_INCLUDED

#pragma once

#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "packed_pixel.hpp"
#include "gray_pixel.hpp"
#include "index_pixel.hpp"

namespace png
{

/**
 * \brief The pixel row traits class template. Provides a common way to get starting address of the row for packed and unpacked row types.
 *
 * Not implemented--see specializations.
 */
template<typename row>
class row_traits;

/**
 * \brief The basic class template to represent image pixel data.
 */
template<typename pixel, typename row, typename traits = row_traits<row>>
class basic_pixel_buffer
{
public:
	/**
	 * \brief A row of pixel data.
	 */
	using row_type = row;
	using row_access = row_type&;
	using row_const_access = const row_type&;
	using row_traits = traits;

	/**
	 * \brief Constructs an empty 0x0 pixel buffer object.
	 */
	inline constexpr basic_pixel_buffer() noexcept : m_width(0), m_height(0) {}

	/**
	 * \brief Constructs an empty pixel buffer object.
	 */
	inline constexpr basic_pixel_buffer(uint32_t width, uint32_t height) noexcept : m_width(0), m_height(0)
	{
		resize(width, height);
	}

	inline constexpr uint32_t get_width() const noexcept
	{
		return m_width;
	}

	inline constexpr uint32_t get_height() const noexcept
	{
		return m_height;
	}

	/**
	 * \brief Resizes the pixel buffer.
	 *
	 * If new width or height is greater than the original, expanded pixels are filled with value of \a pixel().
	 */
	inline constexpr void resize(uint32_t width, uint32_t height) noexcept
	{
		m_width = width;
		m_height = height;
		m_rows.resize(height);
		for (typename row_vec::iterator r = m_rows.begin(); r != m_rows.end(); ++r)
		{
			r->resize(width);
		}
	}

	/**
	 * \brief Returns a reference to the row of image data at specified index.
	 *
	 * Checks the index before returning a row: an instance of std::out_of_range is thrown if \c index is greater than \c height.
	 */
	inline constexpr row_access get_row(size_t index) noexcept
	{
		return m_rows.at(index);
	}

	/**
	 * \brief Returns a const reference to the row of image data at specified index.
	 *
	 * The checking version.
	 */
	inline constexpr row_const_access get_row(size_t index) const noexcept
	{
		return m_rows.at(index);
	}

	/**
	 * \brief The non-checking version of get_row() method.
	 */
	inline constexpr row_access operator[](size_t index) noexcept
	{
		return m_rows[index];
	}

	/**
	 * \brief The non-checking version of get_row() method.
	 */
	inline constexpr row_const_access operator[](size_t index) const noexcept
	{
		return m_rows[index];
	}

	/**
	 * \brief Replaces the row at specified index.
	 */
	inline constexpr void put_row(size_t index, const row_type& r) noexcept
	{
		assert(r.size() == m_width);
		m_rows.at(index) = r;
	}

	/**
	 * \brief Returns a pixel at (x,y) position.
	 */
	inline constexpr pixel get_pixel(uint64_t x, uint64_t y) const noexcept
	{
		return get_row(y).at(x);
	}

	/**
	 * \brief Replaces a pixel at (x,y) position.
	 */
	inline constexpr void set_pixel(uint64_t x, uint64_t y, pixel p) noexcept
	{
		get_row(y).at(x) = p;
	}

protected:
	uint32_t m_width;
	uint32_t m_height;
	using row_vec = std::vector<row_type>;
	row_vec m_rows;
};

/**
 * \brief The row_traits specialization for unpacked pixel rows.
 */
template<typename pixel>
class row_traits<std::vector<pixel>>
{
public:
	/**
	 * \brief Returns the starting address of the row.
	 */
	static inline constexpr pixel* get_data(std::vector<pixel>& vec) noexcept
	{
		assert(vec.size());
		return &vec[0];
	}
};

/**
 * The pixel_buffer specialization for unpacked pixels.
 */
template<typename pixel>
class pixel_buffer : public basic_pixel_buffer<pixel, std::vector<pixel>>
{
public:
	inline constexpr pixel_buffer() noexcept {}

	inline constexpr pixel_buffer(uint32_t width, uint32_t height) noexcept : basic_pixel_buffer<pixel, std::vector< pixel>>(width, height) {}
};

namespace detail
{

template<typename pixel, typename reference>
class basic_packed_pixel_proxy
{
public:
	explicit inline constexpr basic_packed_pixel_proxy(reference ref) noexcept : m_ref(ref), m_shift(0) {}

	inline constexpr basic_packed_pixel_proxy(reference ref, size_t index) noexcept : m_ref(ref), m_shift(get_shift(index)) {}

	inline constexpr operator pixel() const noexcept
	{
		return pixel((m_ref >> m_shift) & pixel::get_bit_mask());
	}

protected:
	/*
	 * bits: . .   .
	 *	  1: 7 6 5 4 3 2 1 0
	 *	  2:   6   4   2   0
	 *	  4:	   4	   0
	 */
	static inline constexpr size_t get_shift(size_t index) noexcept
	{
		int const bits{pixel::get_bit_depth()};
		return (8 - bits) - (index % get_pixels_per_byte()) * bits;
	}

	static inline constexpr size_t get_pixels_per_byte() noexcept
	{
		return 8 / pixel::get_bit_depth();
	}

	reference m_ref;
	size_t m_shift;
};

template<typename pixel>
class const_packed_pixel_proxy : public basic_packed_pixel_proxy<pixel, const byte&>
{
public:
	inline constexpr const_packed_pixel_proxy(const byte& ref, size_t index) noexcept
		: basic_packed_pixel_proxy<pixel, const byte&>(ref, index) {}
};

template<typename pixel>
class packed_pixel_proxy : public basic_packed_pixel_proxy<pixel, byte&>
{
public:
	using basic_proxy = basic_packed_pixel_proxy<pixel, byte&>;

	inline constexpr packed_pixel_proxy(byte& ref, size_t index) noexcept : basic_proxy(ref, index) {}

	inline constexpr packed_pixel_proxy(const packed_pixel_proxy& other) noexcept : basic_proxy(other.m_ref)
	{
		this->m_shift = other.m_shift;
	}

	inline constexpr packed_pixel_proxy& operator=(const packed_pixel_proxy& other) noexcept
	{
		return *this = static_cast<pixel>(other);
	}

	template<typename reference>
	inline constexpr packed_pixel_proxy& operator=(const basic_packed_pixel_proxy<pixel, reference>& other) noexcept
	{
		return *this = static_cast<pixel>(other);
	}

	inline constexpr packed_pixel_proxy& operator=(const pixel& p) noexcept
	{
		this->m_ref = (this->m_ref & ~(pixel::get_bit_mask() << this->m_shift)) | (p << this->m_shift);
		return *this;
	}
};

} // namespace detail

/**
 * \brief The packed pixel row class template.
 *
 * Stores the pixel row as a std::vector of byte-s, providing access to individual packed pixels via proxy objects.
 */
template<typename pixel>
class packed_pixel_row
{
private:
	size_t m_size;
	std::vector<byte> m_vec;

	static inline constexpr size_t get_pixels_per_byte() noexcept
	{
		return CHAR_BIT / pixel::get_bit_depth();
	}

public:
	/**
	 * \brief Constructs a pixel row object for \c size packed pixels.
	 */
	explicit inline constexpr packed_pixel_row(size_t size = 0) noexcept
		: m_size{size}, m_vec(size / get_pixels_per_byte() + (size % get_pixels_per_byte() ? 1 : 0)) {}

	inline constexpr ~packed_pixel_row() noexcept = default;

	// default or delete ctor/mtor? FIXME
	inline constexpr packed_pixel_row(const packed_pixel_row& other) noexcept = default;
	inline constexpr packed_pixel_row(packed_pixel_row&& other) noexcept = default;

	inline constexpr packed_pixel_row& operator=(const packed_pixel_row& other) noexcept = default;
	inline constexpr packed_pixel_row& operator=(packed_pixel_row&& other) noexcept = default;

	// default or delete comparison? FIXME
	inline constexpr auto operator<=>(const packed_pixel_row& other) const noexcept = default;

	inline constexpr size_t size() const noexcept
	{
		return m_size;
	}

	/**
	 * \brief Resizes the pixel row to hold up to \c size packed pixels.
	 */
	inline constexpr void resize(size_t size) noexcept
	{
		m_size = size;
		m_vec.resize(size / get_pixels_per_byte() + (size % get_pixels_per_byte() ? 1 : 0));
	}

	/**
	 * \brief The immutable packed pixel proxy type.
	 */
	using const_pixel_proxy = detail::const_packed_pixel_proxy<pixel>;

	/**
	 * \brief The mutable packed pixel proxy type.
	 */
	using pixel_proxy = detail::packed_pixel_proxy<pixel>;

	/**
	 * \brief Returns an immutable proxy the to the pixel at \c
	 * index.
	 */
	inline constexpr const_pixel_proxy at(size_t index) const noexcept
	{
		return const_pixel_proxy(m_vec.at(index / get_pixels_per_byte()), index);
	}

	/**
	 * \brief Returns a mutable proxy the to the pixel at \c index.
	 */
	inline constexpr pixel_proxy at(size_t index) noexcept
	{
		return pixel_proxy(m_vec.at(index / get_pixels_per_byte()), index);
	}

	/**
	 * \brief Returns an immutable proxy the to the pixel at \c index. The non-checking version.
	 */
	inline constexpr const_pixel_proxy operator[](size_t index) const noexcept
	{
		return const_pixel_proxy(m_vec[index / get_pixels_per_byte()], index);
	}

	/**
	 * \brief Returns n mutable proxy the to the pixel at \c index. The non-checking version.
	 */
	inline constexpr pixel_proxy operator[](size_t index) noexcept
	{
		return pixel_proxy(m_vec[index / get_pixels_per_byte()], index);
	}

	/**
	 * \brief Returns the starting address of the row.
	 */
	inline constexpr byte* get_data() noexcept
	{
		//assert(m_vec.size());
		return &m_vec[0];
	}
};

/**
 * \brief The row_traits class template specialization for packed pixel row type.
 */
template<typename pixel>
class row_traits<packed_pixel_row<pixel>>
{
public:
	/**
	 * \brief Returns the starting address of the row.
	 */
	static inline constexpr byte* get_data(packed_pixel_row<pixel>& row) noexcept
	{
		return row.get_data();
	}
};

/**
 * \brief The pixel buffer class template specialization for the packed_gray_pixel type.
 */
template<int bits>
class pixel_buffer<packed_gray_pixel<bits>> : public basic_pixel_buffer<packed_gray_pixel<bits>, packed_pixel_row<packed_gray_pixel<bits>>>
{
public:
	using pixel_type = packed_gray_pixel<bits>;
	using pixel_row_type = packed_pixel_row<pixel_type>;

	inline constexpr pixel_buffer() noexcept = default;
	inline constexpr pixel_buffer(uint32_t width, uint32_t height) noexcept : basic_pixel_buffer<pixel_type, pixel_row_type>(width, height) {}
};

/**
 * \brief The pixel buffer class template specialization for the packed_index_pixel type.
 */
template<int bits>
class pixel_buffer<packed_index_pixel<bits>> : public basic_pixel_buffer<packed_index_pixel<bits>, packed_pixel_row<packed_index_pixel<bits>>>
{
public:
	using pixel_type = packed_index_pixel<bits>;
	using pixel_row_type = packed_pixel_row<pixel_type>;

	inline constexpr pixel_buffer() noexcept = default;
	inline constexpr pixel_buffer(uint32_t width, uint32_t height) noexcept : basic_pixel_buffer<pixel_type, pixel_row_type>(width, height) {}
};

} // namespace png

#endif // PNGPP_PIXEL_BUFFER_HPP_INCLUDED
