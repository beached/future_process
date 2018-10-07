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

#include <cstddef>
#include <cstring>
#include <exception>
#include <unistd.h>

#include "daw/ipc_pipe.h"

namespace daw {
	pipe_t::pipe_t( ) {
		if( pipe( m_handles.data( ) ) != 0 ) {
			daw::exception::daw_throw<std::runtime_error>( std::strerror( errno ) );
		}
	}
	pipe_t::~pipe_t( ) noexcept {
		if( m_handles[0] >= 0 ) {
			close( m_handles[0] );
		}
		if( m_handles[1] >= 0 ) {
			close( m_handles[1] );
		}
	}

	ssize_t pipe_t::write( void const *buffer, size_t byte_count ) const noexcept {
		return ::write( m_handles[1], buffer, byte_count );
	}

	ssize_t pipe_t::read( void *buffer, size_t byte_capacity ) const noexcept {
		return ::read( m_handles[0], buffer, byte_capacity );
	}
}

