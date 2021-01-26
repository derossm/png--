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
#ifndef PNGPP_ERROR_HPP_INCLUDED
#define PNGPP_ERROR_HPP_INCLUDED

#pragma once

#include <array>
#include <string>
#include <stdexcept>
#include <cerrno>
#include <cstring>

namespace png
{

// the longest errors seem to be under 100 characters, so 512 seems safe
inline constexpr const size_t MAX_ERROR_LEN{512};

/**
 * \brief Exception class to represent runtime errors related to png++ operation.
 */
class error : public std::runtime_error
{
private:
	error() = delete;

	error(const error&) = delete;
	error(error&&) = delete;

	error& operator=(const error&) = delete;
	error& operator=(error&&) = delete;

public:
	/**
	 * \param message error description
	 */
	explicit inline error(const std::string& message) noexcept : std::runtime_error(message) {}

	inline ~error() noexcept = default;
};

/**
 * \brief Exception class to represent standard library errors (generally IO).
 *
 * \see reader, writer
 */

class std_error : public std::runtime_error
{
private:
	std_error() = delete;

	std_error(const std_error&) = delete;
	std_error(std_error&&) = delete;

	std_error& operator=(const std_error&) = delete;
	std_error& operator=(std_error&&) = delete;

protected:
	inline ::std::string thread_safe_strerror(int errnum) const noexcept
	{
		::std::array<char, MAX_ERROR_LEN> str;
		strerror_s(str.data(), MAX_ERROR_LEN, errnum);
		return ::std::string(str.data());
	}

public:
	/**
	 * Constructs an std_error object.
	 * The \a message string is appended with <tt>": "</tt> and the error description as returned by \c strerror(\a error).
	 *
	 * \param message error description
	 * \param error error number
	 */
	explicit inline std_error(const std::string& message, int errnum = errno) noexcept
		: std::runtime_error((message + ": ") + thread_safe_strerror(errnum)) {}

	inline ~std_error() noexcept = default;
};

} // namespace png

#endif // PNGPP_ERROR_HPP_INCLUDED
