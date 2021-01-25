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
#ifndef PNGPP_STREAMING_BASE_HPP_INCLUDED
#define PNGPP_STREAMING_BASE_HPP_INCLUDED

#pragma once

#include <cassert>
#include "image_info.hpp"
#include "pixel_traits.hpp"

namespace png
{

/**
 * \brief The default image_info holder class. Stores image_info member object.
 */
class def_image_info_holder
{
	def_image_info_holder() = delete;
public:
	explicit inline constexpr def_image_info_holder(const image_info& info) noexcept : m_info(info) {}

	inline constexpr image_info& get_info() noexcept
	{
		return m_info;
	}

private:
	image_info m_info;
};

/**
 * \brief An image_info holder class. Stores a reference to the image_info object. The image_info object itself should be stored elsewhere.
 */
class image_info_ref_holder
{
	image_info_ref_holder() = delete;
public:
	explicit inline constexpr image_info_ref_holder(image_info& info) noexcept : m_info(info) {}

	inline constexpr image_info& get_info() noexcept
	{
		return m_info;
	}

private:
	image_info& m_info;
};

/**
 * \brief A base class template for consumer and generator classes. Provides default \c reset() method implementation as well as \c info_holder policy.
 */
template<typename pixel, class info_holder>
class streaming_base
{
	streaming_base() = delete;
public:
	using traits = pixel_traits<pixel>;

	explicit inline constexpr streaming_base(image_info& info) noexcept : m_info_holder(info) {}

	inline constexpr streaming_base(uint32_t width, uint32_t height) noexcept : m_info_holder(make_image_info<pixel>())
	{
		get_info().set_width(width);
		get_info().set_height(height);
	}

	inline constexpr const image_info& get_info() const noexcept
	{
		return m_info_holder.get_info();
	}

protected:
	inline constexpr void reset(size_t /*pass*/) const noexcept
	{
		// nothing to do in the most general case
	}

	inline constexpr image_info& get_info() noexcept
	{
		return m_info_holder.get_info();
	}

	info_holder m_info_holder;
};

} // namespace png

#endif // PNGPP_STREAMING_BASE_HPP_INCLUDED
