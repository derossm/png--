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
#ifndef PNGPP_IMAGE_INFO_HPP_INCLUDED
#define PNGPP_IMAGE_INFO_HPP_INCLUDED

#include <ranges>
#include <algorithm>

#include "types.hpp"
#include "palette.hpp"
#include "tRNS.hpp"
#include "pixel_traits.hpp"

namespace png
{

/**
 * \brief Holds information about PNG image.
 *
 * \see image, generator, consumer
 */
class image_info
{
public:
	/**
	 * \brief Constructs the image_info object with default values for color_type, interlace_type, compression_method and filter_type.
	 */
	inline image_info() noexcept {}
	inline constexpr ~image_info() noexcept = default;

	inline constexpr image_info& operator=(const image_info& other) noexcept = default;
	/*{
		m_width = other.m_width;
		m_height = other.m_height;
		m_bit_depth = other.m_bit_depth;
		m_color_type = other.m_color_type;
		m_interlace_type = other.m_interlace_type;
		m_compression_type = other.m_compression_type;
		m_filter_type = other.m_filter_type;
		m_gamma = other.m_gamma;

		m_palette = other.m_palette;
		m_tRNS = other.m_tRNS;

		return *this;
	}*/
	inline constexpr image_info(const image_info& other) noexcept = default;
	/*{
		*this = other;
	}*/

	inline constexpr image_info& operator=(image_info&& other) noexcept = default;
	/*{
		m_width = other.m_width;
		m_height = other.m_height;
		m_bit_depth = other.m_bit_depth;
		m_color_type = other.m_color_type;
		m_interlace_type = other.m_interlace_type;
		m_compression_type = other.m_compression_type;
		m_filter_type = other.m_filter_type;
		m_gamma = other.m_gamma;

		m_palette = ::std::move(other.m_palette);
		//other.m_palette.clear();

		m_tRNS = ::std::move(other.m_tRNS);
		//other.m_tRNS.clear();

		return *this;
	}*/

	inline constexpr image_info(image_info&& other) noexcept = default;
	/*{
		*this = ::std::move(other);
	}*/

	inline constexpr uint_32 get_width() const noexcept
	{
		return m_width;
	}

	inline constexpr void set_width(uint_32 width) noexcept
	{
		m_width = width;
	}

	inline constexpr uint_32 get_height() const noexcept
	{
		return m_height;
	}

	inline constexpr void set_height(uint_32 height) noexcept
	{
		m_height = height;
	}

	inline constexpr color_type get_color_type() const noexcept
	{
		return m_color_type;
	}

	inline constexpr void set_color_type(color_type color_space) noexcept
	{
		m_color_type = color_space;
	}

	inline constexpr int get_bit_depth() const noexcept
	{
		return m_bit_depth;
	}

	inline constexpr void set_bit_depth(int bit_depth) noexcept
	{
		m_bit_depth = bit_depth;
	}

	inline constexpr interlace_type get_interlace_type() const noexcept
	{
		return m_interlace_type;
	}

	inline constexpr void set_interlace_type(interlace_type interlace) noexcept
	{
		m_interlace_type = interlace;
	}

	inline constexpr compression_type get_compression_type() const noexcept
	{
		return m_compression_type;
	}

	inline constexpr void set_compression_type(compression_type compression) noexcept
	{
		m_compression_type = compression;
	}

	inline constexpr filter_type get_filter_type() const noexcept
	{
		return m_filter_type;
	}

	inline constexpr void set_filter_type(filter_type filter)
	{
		m_filter_type = filter;
	}

	inline constexpr const palette& get_palette() const noexcept
	{
		return m_palette;
	}

	inline constexpr palette& get_palette() noexcept
	{
		return m_palette;
	}

	inline void set_palette(const palette& plte) noexcept
	{
		m_palette = plte;
	}

	/**
	 * \brief Removes all entries from the palette.
	 */
	inline void drop_palette() noexcept
	{
		m_palette.clear();
	}

	inline constexpr const tRNS& get_tRNS() const noexcept
	{
		return m_tRNS;
	}

	inline constexpr tRNS& get_tRNS() noexcept
	{
		return m_tRNS;
	}

	inline void set_tRNS(const tRNS& trns) noexcept
	{
		m_tRNS.clear();
		::std::ranges::copy(trns, m_tRNS.begin());
	}

	inline constexpr double get_gamma() const noexcept
	{
		return m_gamma;
	}

	inline constexpr void set_gamma(double gamma) noexcept
	{
		m_gamma = gamma;
	}

protected:
	uint_32 m_width{0};
	uint_32 m_height{0};
	int m_bit_depth{0};
	color_type m_color_type{color_type_none};
	interlace_type m_interlace_type{interlace_none};
	compression_type m_compression_type{compression_type_default};
	filter_type m_filter_type{filter_type_default};
	double m_gamma{0.0};
	palette m_palette;
	tRNS m_tRNS;
};

/**
 * \brief Returns an image_info object with color_type and bit_depth fields setup appropriate for the \c pixel type.
 */
template<typename pixel>
inline constexpr image_info make_image_info() noexcept
{
	using traits = pixel_traits<pixel>;
	image_info info;
	info.set_color_type(traits::get_color_type());
	info.set_bit_depth(traits::get_bit_depth());
	return info;
}

} // namespace png

#endif // PNGPP_IMAGE_INFO_HPP_INCLUDED
