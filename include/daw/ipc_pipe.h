// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <array>
#include <utility>
#include <unistd.h>

#include <daw/daw_utility.h>

namespace daw {
	class pipe_t {
		std::array<int, 2> m_handles = {-1, -1};

	public:
		pipe_t( );
		~pipe_t( ) noexcept;

		constexpr pipe_t( int in, int out ) noexcept
		  : m_handles{in, out} {}

		constexpr pipe_t( pipe_t &&other ) noexcept
		  : m_handles{daw::exchange( other.m_handles[0], -1 ),
		              daw::exchange( other.m_handles[1], -1 )} {}

		constexpr pipe_t &operator=( pipe_t &&rhs ) noexcept {
			if( this != &rhs ) {
				m_handles = std::exchange( rhs.m_handles, {-1, -1} );
			}
			return *this;
		}

		pipe_t( pipe_t const & ) noexcept = delete;
		pipe_t &operator=( pipe_t const & ) noexcept = delete;

		ssize_t write( void const *buffer, size_t byte_count ) const noexcept;
		ssize_t read( void *buffer, size_t byte_capacity ) const noexcept;
	};
}

