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
#ifndef PNGPP_INFO_HPP_INCLUDED
#define PNGPP_INFO_HPP_INCLUDED

#pragma once

//#include <cassert>
#include "info_base.hpp"
#include "image_info.hpp"

namespace png
{

/**
 * \brief Holds information about PNG image. Adapter class for IO image operations.
 */
class info : public info_base, public image_info
{
public:
	inline info(io_base& io, png_struct* png) noexcept : info_base(io, png) {}

	inline void read()
	{
		//assert(m_png);
		//assert(m_info);

		png_read_info(m_png.get(), m_info.get());
		png_get_IHDR(m_png.get(), m_info.get(), &m_width, &m_height,
			reinterpret_cast<int*>(&m_bit_depth),
			reinterpret_cast<int*>(&m_color_type),
			reinterpret_cast<int*>(&m_interlace_type),
			reinterpret_cast<int*>(&m_compression_type),
			reinterpret_cast<int*>(&m_filter_type));

		if (png_get_valid(m_png.get(), m_info.get(), chunk_PLTE) == chunk_PLTE)
		{
			png_color* colors{nullptr};
			int count{0};
			png_get_PLTE(m_png.get(), m_info.get(), &colors, &count);
			m_palette.assign(colors, colors + count);
		}

		if constexpr (png_trns_supported)
		{
			if (png_get_valid(m_png.get(), m_info.get(), chunk_tRNS) == chunk_tRNS)
			{
				if (m_color_type == color_type_palette)
				{
					int count;
					byte* values;
					if (png_get_tRNS(m_png.get(), m_info.get(), &values, &count, nullptr) != PNG_INFO_tRNS)
					{
						throw error("png_get_tRNS() failed");
					}
					m_tRNS.assign(values, values + count);
				}
			}
		}

		if constexpr (png_gama_supported && png_floating_point_supported)
		{
			if (png_get_valid(m_png.get(), m_info.get(), chunk_gAMA) == chunk_gAMA)
			{
				if (png_get_gAMA(m_png.get(), m_info.get(), &m_gamma) != PNG_INFO_gAMA)
				{
					throw error("png_get_gAMA() failed");
				}
			}
		}
		else
		{
			png_fixed_point gamma = 0;
			if (png_get_gAMA_fixed(m_png.get(), m_info.get(), &gamma) != PNG_INFO_gAMA)
			{
				throw error("png_get_gAMA_fixed() failed");
			}
			m_gamma = gamma / 100000.0;
		}
	}

	inline void write() const
	{
		//assert(m_png);
		//assert(m_info);

		sync_ihdr();
		if (m_color_type == color_type_palette)
		{
			if (!m_palette.empty())
			{
				png_set_PLTE(m_png.get(), m_info.get(), const_cast<color*>(&m_palette[0]), static_cast<int>(m_palette.size()));
			}
			if (!m_tRNS.empty())
			{
				if constexpr (png_trns_supported)
				{
					png_set_tRNS(m_png.get(), m_info.get(), const_cast<byte*>(&m_tRNS[0]), static_cast<int>(m_tRNS.size()), NULL);
				}
				else
				{
					throw error("attempted to write tRNS chunk; recompile with PNG_tRNS_SUPPORTED");
				}
			}
		}

		if constexpr (png_gama_supported)
		{
			if (m_gamma > 0)
			{
				if constexpr (png_floating_point_supported)
				{
					png_set_gAMA(m_png.get(), m_info.get(), m_gamma);
				}
				else
				{
						png_set_gAMA_fixed(m_png.get(), m_info.get(), static_cast<png_fixed_point>(m_gamma * 100000));
				}
			}
		}
		else
		{
			throw error("attempted to write gAMA chunk; recompile with PNG_gAMA_SUPPORTED");
		}

		png_write_info(m_png.get(), m_info.get());
	}

	inline void update() noexcept
	{
		//assert(m_png);
		//assert(m_info);

		sync_ihdr();
		png_read_update_info(m_png.get(), m_info.get());
	}

protected:
	inline void sync_ihdr() const noexcept
	{
		png_set_IHDR(m_png.get(), m_info.get(), m_width, m_height, m_bit_depth, m_color_type, m_interlace_type, m_compression_type, m_filter_type);
	}
};

} // namespace png

#endif // PNGPP_INFO_HPP_INCLUDED
