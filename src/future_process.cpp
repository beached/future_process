// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "daw/future_process.h"

namespace daw {
	void ipc_promise<void>::get( ) {
		if( m_value.empty( ) ) {
			receive( *this );
		}
		m_value.get( );
	}

	ipc_promise<void>::operator bool( ) const noexcept {
		return !m_value.has_exception( );
	}

	void send( ipc_promise<void> &promise ) noexcept {
		if( !promise ) {
			return;
		}
		char buff = 1;
		if( promise.m_pipe.write( &buff, 1 ) < 0 ) {
			promise.m_value.set_exception(
			  daw::exception::make_exception_ptr<std::runtime_error>(
			    std::strerror( errno ) ) );
		}
	}

	void receive( ipc_promise<void> &promise ) noexcept {
		if( !promise ) {
			return;
		}
		char buff = 0;
		if( promise.m_pipe.read( &buff, 1 ) < 0 ) {
			promise.m_value.set_exception(
			  daw::exception::make_exception_ptr<std::runtime_error>(
			    std::strerror( errno ) ) );
			return;
		}
		daw::exception::dbg_precondition_check( buff == 1 );
		promise.m_value = true;
	}
} // namespace daw
