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
#ifndef PNGPP_CONSUMER_HPP_INCLUDED
#define PNGPP_CONSUMER_HPP_INCLUDED

#pragma once

#include <cassert>
#include <stdexcept>
#include <iostream>
#include <istream>

#include "config.hpp"
#include "error.hpp"
#include "streaming_base.hpp"
#include "reader.hpp"
#include "pixel_buffer.hpp"

namespace png
{

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

/**
 * \brief Pixel consumer class template.
 *
 * Used as a base class for custom pixel consumer classes as well as inside image class implementation to read pixels into the pixel buffer.
 *
 * Encapsulates PNG %image reading procedure. In order to create a custom pixel %consumer use CRTP trick:
 *
 * \code
 * class pixel_consumer : public png::consumer<pixel, pixel_consumer>
 * {
 *		...
 * };
 * \endcode
 *
 * Your pixel %consumer class should implement \c get_next_row() method and \c reset() method (optional). Their signatures are as follows:
 *
 * \code
 * png::byte* get_next_row(png::uint_32 pos);
 * void reset(size_t pass);
 * \endcode
 *
 * The \c get_next_row() method is called every time a new row of %image data is available to the reader. The position of the row being read is
 * passed as \c pos parameter. The \c pos takes values from \c 0 to \c <image_height>-1 inclusively. The method should return the starting
 * address of a row buffer capable of storing appropriate amount of pixels (i.e. the width of the %image being read). The address should be
 * casted to png::byte* pointer type using \c reinterpret_cast<> or a C-style cast.
 *
 * The optional \c reset() method is called every time the new pass of interlaced %image processing starts. The number of interlace pass is
 * avaiable as the only parameter of the method. For non-interlaced images the method is called once prior to any calls to \c get_next_row().
 * The value of \c 0 is passed for the \c pass number.
 *
 * An optional template parameter \c info_holder encapsulates image_info storage policy. Using def_image_info_holder results in image_info object
 * stored as a sub-object of the consumer class. You may specify image_info_ref_holder in order to use a reference to the externally stored
 * image_info object. This way you will have to construct the consumer object passing the reference to image_info object.
 *
 * Also, you might want implement an %info holder object yourself to fine-tune your code. In any case, you can access the image_info object
 * from your %consumer class methods using the following code:
 *
 * \code
 * png::image_info& info = m_info_holder.get_info();
 * \endcode
 *
 * An optional \c bool template parameter \c interlacing_supported specifies whether reading interlacing images is supported by your %consumer class.
 * It defaults to \c false. An attempt to read an interlaced %image will result in discarding pixels obtained at all the interlacing passes except the last one.
 *
 * In order to fully support interlacing specify \c true for \c interlacing_supported parameter and implement \c reset() method.
 *
 * \see image, generator
 */

template<typename pixel, typename pixcon, typename info_holder = def_image_info_holder, bool interlacing_supported = false>
class consumer : public streaming_base<pixel, info_holder>
{
public:
	using traits = pixel_traits<pixel>;

	/**
	 * \brief The default io transformation: does nothing.
	 */
	struct transform_identity
	{
		inline constexpr void operator()(io_base&) const noexcept {}
	};

	/**
	 * \brief Reads an image from the stream using custom io transformation.
	 *
	 * Essentially, this method constructs a reader object and instructs it to read the image from the stream.
	 * It handles IO transformation, as well as interlaced image reading.
	 */
	template<typename istream, typename transformation>
	inline constexpr void read(istream& stream, const transformation& transform = transform_identity())
	{
		reader<istream> rd(stream);
		rd.read_info();
		transform(rd);

		if constexpr (__little_endian)
		{
			if constexpr (png_read_swap_supported)
			{
				if (pixel_traits<pixel>::get_bit_depth() == 16)
				{
					rd.set_swap();
				}
			}
			else
			{
				throw error("Cannot read 16-bit image: recompile with PNG_READ_SWAP_SUPPORTED.");
			}
		}

		// interlace handling _must_ be set up prior to info update
		size_t pass_count;
		if (rd.get_interlace_type() != interlace_none)
		{
			if constexpr (png_read_interlacing_supported)
			{
				pass_count = rd.set_interlace_handling();
			}
			else
			{
				throw error("Cannot read interlaced image: interlace handling disabled.");
			}
		}
		else
		{
			pass_count = 1;
		}

		rd.update_info();
		if (rd.get_color_type() != traits::get_color_type() || rd.get_bit_depth() != traits::get_bit_depth())
		{
			throw std::logic_error("color type and/or bit depth mismatch in png::consumer::read()");
		}

		this->get_info() = rd.get_image_info();

		auto pixel_con{static_cast<pixcon*>(this)};
		if (pass_count > 1 && !interlacing_supported)
		{
			skip_interlaced_rows(rd, pass_count);
			pass_count = 1;
		}
		read_rows(rd, pass_count, pixel_con);

		rd.read_end_info();
	}

	inline constexpr auto operator<=>(const consumer&) const noexcept = default;

protected:
	using base = streaming_base<pixel, info_holder>;

	/**
	 * \brief Constructs a consumer object using passed image_info object to store image information.
	 */
	explicit inline constexpr consumer(image_info& info) noexcept : base(info) {}

private:
	consumer() = delete;
	~consumer() = default;

	inline constexpr consumer(const consumer&) noexcept = delete;
	inline constexpr consumer(consumer&&) noexcept = delete;

	inline constexpr consumer& operator=(const consumer&) noexcept = delete;
	inline constexpr consumer& operator=(consumer&&) noexcept = delete;

	template<typename istream>
	inline constexpr void skip_interlaced_rows(reader<istream>& rd, size_t pass_count) noexcept
	{
		using row = std::vector<pixel>;
		using row_traits_type = row_traits<row>;

		row dummy_row(this->get_info().get_width());
		for (size_t pass{1}; pass < pass_count; ++pass)
		{
			rd.read_row(reinterpret_cast<byte*>(row_traits_type::get_data(dummy_row)));
		}
	}

	template<typename istream>
	inline constexpr void read_rows(reader<istream>& rd, size_t pass_count, pixcon* pixel_con) noexcept
	{
		for (size_t pass{0}; pass < pass_count; ++pass)
		{
			pixel_con->reset(pass);

			for (uint_32 pos{0}; pos < this->get_info().get_height(); ++pos)
			{
				rd.read_row(pixel_con->get_next_row(pos));
			}
		}
	}
};

} // namespace png

#endif // PNGPP_CONSUMER_HPP_INCLUDED
