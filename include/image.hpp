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
#ifndef PNGPP_IMAGE_HPP_INCLUDED
#define PNGPP_IMAGE_HPP_INCLUDED

#include <fstream>
#include "pixel_buffer.hpp"
#include "generator.hpp"
#include "consumer.hpp"
#include "convert_color_space.hpp"

namespace png
{

/**
 * \brief Class template to represent PNG image.
 *
 * The image consists of pixel data as well as additional %image
 * %info like interlace type, compression method, palette (for
 * colormap-based images) etc. Provides methods to read and write
 * images from/to a generic stream and to manipulate %image pixels.
 *
 * The default pixel_buffer stores pixels in a vector of vectors, which
 * is good for openning, editing or converting an image to any
 * pixel type.
 * But for simple and fast image unpacking to one memory chunk this approch
 * is unacceptable, because it leads to multiple memory allocations, the
 * unpacked image is spread across the memory and client code needs to
 * gather it manualy. solid_pixel_buffer solves this problem, but with
 * restriction: pixels with fractional number of bytes per channel are
 * not allowed (see solid_pixel_buffer.hpp for details).
 */
template<typename pixel, typename pixel_buffer_type = pixel_buffer<pixel>>
class image
{
public:
	/**
	 * \brief The pixel traits type for \c pixel.
	 */
	using traits = pixel_traits<pixel>;

	/**
	 * \brief The pixel buffer type for \c pixel.
	 */
	using pixbuf = pixel_buffer_type;

	/**
	 * \brief Represents a row of image pixel data.
	 */
	using row_type = typename pixbuf::row_type;
	using row_access = typename pixbuf::row_access;
	using row_const_access = typename pixbuf::row_const_access;

	/**
	 * \brief A transformation functor to convert any image to
	 * appropriate color space.
	 */
	using transform_convert = convert_color_space<pixel>;

	/**
	 * \brief The default io transformation: does nothing.
	 */
	struct transform_identity
	{
		inline constexpr void operator()(io_base&) const noexcept {}
	};

	/**
	 * \brief Constructs an empty image.
	 */
	inline constexpr image() noexcept : m_info(make_image_info<pixel>()) {}

	/**
	 * \brief Constructs an empty image of specified width and height.
	 */
	inline constexpr image(uint_32 width, uint_32 height) noexcept : m_info(make_image_info<pixel>())
	{
		resize(width, height);
	}

	/**
	 * \brief Constructs an image reading data from specified file using default converting transform.
	 */
	explicit inline constexpr image(const std::string& filename) noexcept
	{
		read(filename, transform_convert());
	}

	/**
	 * \brief Constructs an image reading data from specified file using custom transformaton.
	 */
	template<typename transformation>
	inline constexpr image(const std::string& filename, const transformation& transform) noexcept
	{
		read(filename.c_str(), transform);
	}

	/**
	 * \brief Constructs an image reading data from specified file using default converting transform.
	 */
	explicit inline constexpr image(const char* filename) noexcept
	{
		read(filename, transform_convert());
	}

	/**
	 * \brief Constructs an image reading data from specified file using custom transformaton.
	 */
	template<typename transformation>
	inline constexpr image(const char* filename, const transformation& transform) noexcept
	{
		read(filename, transform);
	}

	/**
	 * \brief Constructs an image reading data from a stream using default converting transform.
	 */
	explicit inline constexpr image(std::istream& stream) noexcept
	{
		read_stream(stream, transform_convert());
	}

	/**
	 * \brief Constructs an image reading data from a stream using custom transformation.
	 */
	template<typename transformation>
	inline constexpr image(std::istream& stream, const transformation& transform) noexcept
	{
		read_stream(stream, transform);
	}

	/**
	 * \brief Reads an image from specified file using default converting transform.
	 */
	inline constexpr void read(const std::string& filename) noexcept
	{
		read(filename, transform_convert());
	}

	/**
	 * \brief Reads an image from specified file using custom transformaton.
	 */
	template<typename transformation>
	inline constexpr void read(const std::string& filename, const transformation& transform) noexcept
	{
		read(filename.c_str(), transform);
	}

	/**
	 * \brief Reads an image from specified file using default converting transform.
	 */
	inline constexpr void read(const char* filename) noexcept
	{
		read(filename, transform_convert());
	}

	/**
	 * \brief Reads an image from specified file using custom transformaton.
	 */
	template<typename transformation>
	inline constexpr void read(const char* filename, const transformation& transform)
	{
		std::ifstream stream(filename, std::ios::binary);
		if (!stream.is_open())
		{
			throw std_error(filename);
		}
		stream.exceptions(std::ios::badbit);
		read_stream(stream, transform);
	}

	/**
	 * \brief Reads an image from a stream using default converting transform.
	 */
	inline constexpr void read(std::istream& stream) noexcept
	{
		read_stream(stream, transform_convert());
	}

	/**
	 * \brief Reads an image from a stream using custom transformation.
	 */
	template<typename transformation>
	inline constexpr void read(std::istream& stream, const transformation& transform) noexcept
	{
		read_stream(stream, transform);
	}

	/**
	 * \brief Reads an image from a stream using default converting transform.
	 */
	template<typename istream>
	inline constexpr void read_stream(istream& stream) noexcept
	{
		read_stream(stream, transform_convert());
	}

	/**
	 * \brief Reads an image from a stream using custom transformation.
	 */
	template<typename istream, typename transformation>
	inline constexpr void read_stream(istream& stream, const transformation& transform) noexcept
	{
		pixel_consumer pixcon(m_info, m_pixbuf);
		pixcon.read(stream, transform);
	}

	/**
	 * \brief Writes an image to specified file.
	 */
	inline constexpr void write(const std::string& filename) noexcept
	{
		write(filename.c_str());
	}

	/**
	 * \brief Writes an image to specified file.
	 */
	inline constexpr void write(const char* filename) noexcept
	{
		std::ofstream stream(filename, std::ios::binary);
		if (!stream.is_open())
		{
			throw std_error(filename);
		}
		stream.exceptions(std::ios::badbit);
		write_stream(stream);
	}

	/**
	 * \brief Writes an image to a stream.
	 */
	template<typename ostream>
	inline constexpr void write_stream(ostream& stream) noexcept
	{
		pixel_generator pixgen(m_info, m_pixbuf);
		pixgen.write(stream);
	}

	/**
	 * \brief Returns a reference to image pixel buffer.
	 */
	inline constexpr pixbuf& get_pixbuf() noexcept
	{
		return m_pixbuf;
	}

	/**
	 * \brief Returns a const reference to image pixel buffer.
	 */
	inline constexpr const pixbuf& get_pixbuf() const noexcept
	{
		return m_pixbuf;
	}

	/**
	 * \brief Replaces the image pixel buffer.
	 *
	 * \param buffer a pixel buffer object to take a copy from
	 */
	inline constexpr void set_pixbuf(const pixbuf& buffer) noexcept
	{
		m_pixbuf = buffer;
	}

	inline constexpr uint_32 get_width() const noexcept
	{
		return m_pixbuf.get_width();
	}

	inline constexpr uint_32 get_height() const noexcept
	{
		return m_pixbuf.get_height();
	}

	/**
	 * \brief Resizes the image pixel buffer.
	 */
	inline constexpr void resize(uint_32 width, uint_32 height) noexcept
	{
		m_pixbuf.resize(width, height);
		m_info.set_width(width);
		m_info.set_height(height);
	}

	/**
	 * \brief Returns a reference to the row of image data at specified index.
	 *
	 * \see pixel_buffer::get_row()
	 */
	inline constexpr row_access get_row(size_t index) noexcept
	{
		return m_pixbuf.get_row(index);
	}

	/**
	 * \brief Returns a const reference to the row of image data at specified index.
	 *
	 * \see pixel_buffer::get_row()
	 */
	inline constexpr row_const_access get_row(size_t index) const noexcept
	{
		return m_pixbuf.get_row(index);
	}

	/**
	 * \brief The non-checking version of get_row() method.
	 */
	inline constexpr row_access operator[](size_t index) noexcept
	{
		return m_pixbuf[index];
	}

	/**
	 * \brief The non-checking version of get_row() method.
	 */
	inline constexpr row_const_access operator[](size_t index) const noexcept
	{
		return m_pixbuf[index];
	}

	/**
	 * \brief Returns a pixel at (x,y) position.
	 */
	inline constexpr pixel get_pixel(size_t x, size_t y) const noexcept
	{
		return m_pixbuf.get_pixel(x, y);
	}

	/**
	 * \brief Replaces a pixel at (x,y) position.
	 */
	inline constexpr void set_pixel(size_t x, size_t y, pixel p) noexcept
	{
		m_pixbuf.set_pixel(x, y, p);
	}

	inline constexpr interlace_type get_interlace_type() const noexcept
	{
		return m_info.get_interlace_type();
	}

	inline constexpr void set_interlace_type(interlace_type interlace) noexcept
	{
		m_info.set_interlace_type(interlace);
	}

	inline constexpr compression_type get_compression_type() const noexcept
	{
		return m_info.get_compression_type();
	}

	inline constexpr void set_compression_type(compression_type compression) noexcept
	{
		m_info.set_compression_type(compression);
	}

	inline constexpr filter_type get_filter_type() const noexcept
	{
		return m_info.get_filter_type();
	}

	inline constexpr void set_filter_type(filter_type filter) noexcept
	{
		m_info.set_filter_type(filter);
	}

	/**
	 * \brief Returns a reference to the image palette.
	 */
	inline constexpr palette& get_palette() noexcept
	{
		return m_info.get_palette();
	}

	/**
	 * \brief Returns a const reference to the image palette.
	 */
	inline constexpr const palette& get_palette() const noexcept
	{
		return m_info.get_palette();
	}

	/**
	 * \brief Replaces the image palette.
	 */
	inline constexpr void set_palette(const palette& plte) noexcept
	{
		m_info.set_palette(plte);
	}

	inline constexpr const tRNS& get_tRNS() const noexcept
	{
		return m_info.get_tRNS();
	}

	inline constexpr tRNS& get_tRNS() noexcept
	{
		return m_info.get_tRNS();
	}

	inline constexpr void set_tRNS(const tRNS& trns) noexcept
	{
		m_info.set_tRNS(trns);
	}

	inline constexpr double get_gamma() const noexcept
	{
		return m_info.get_gamma();
	}

	inline constexpr void set_gamma(double gamma) noexcept
	{
		m_info.set_gamma(gamma);
	}

protected:
	/**
	 * \brief A common base class template for pixel_consumer and pixel_generator classes.
	 */
	template<typename base_impl>
	class streaming_impl : public base_impl
	{
	public:
		inline constexpr streaming_impl(image_info& info, pixbuf& pixels) noexcept
			: base_impl(info), m_pixbuf(pixels) {}

		/**
		 * \brief Returns the starting address of a \c pos-th row in the image's pixel buffer.
		 */
		inline constexpr byte* get_next_row(size_t pos) noexcept
		{
			using row_traits = typename pixbuf::row_traits;
			return reinterpret_cast<byte*>(row_traits::get_data(m_pixbuf.get_row(pos)));
		}

	protected:
		pixbuf& m_pixbuf;
	};

	/**
	 * \brief The pixel buffer adapter for reading pixel data.
	 */
	class pixel_consumer
		: public streaming_impl<consumer<pixel, pixel_consumer, image_info_ref_holder, /* interlacing = */ true>>
	{
	public:
		inline constexpr pixel_consumer(image_info& info, pixbuf& pixels) noexcept
			: streaming_impl<consumer<pixel, pixel_consumer, image_info_ref_holder, true>>(info, pixels) {}

		inline constexpr void reset(size_t pass) noexcept
		{
			if (pass == 0)
			{
				this->m_pixbuf.resize(this->get_info().get_width(), this->get_info().get_height());
			}
		}
	};

	/**
	 * \brief The pixel buffer adapter for writing pixel data.
	 */
	class pixel_generator
		: public streaming_impl<generator<pixel, pixel_generator, image_info_ref_holder, /* interlacing = */ true>>
	{
	public:
		inline constexpr pixel_generator(image_info& info, pixbuf& pixels) noexcept
			: streaming_impl<generator<pixel, pixel_generator, image_info_ref_holder, true>>(info, pixels) {}
	};

	image_info m_info;
	pixbuf m_pixbuf;
};

} // namespace png

#endif // PNGPP_IMAGE_HPP_INCLUDED
