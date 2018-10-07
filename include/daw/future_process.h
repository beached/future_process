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

#pragma once

#include <array>
#include <cstddef>
#include <cstring>

#include <daw/cpp_17.h>
#include <daw/daw_exception.h>
#include <daw/daw_expected.h>
#include <daw/daw_traits.h>

#include "ipc_pipe.h"

namespace daw {
	// Ensure that T is trivially copiable(should be memcpy'able) and not a
	// pointer
	template<typename T>
	struct is_future_transferrable
	  : std::bool_constant<daw::is_trivially_copyable_v<T> and
	                       !daw::is_pointer_v<T> &&
	                       !daw::is_member_pointer_v<T>> {};

	template<>
	struct is_future_transferrable<void> : std::true_type {};

	template<typename T>
	CXINLINE bool is_future_transferrable_v = is_future_transferrable<T>::value;

	template<typename T>
	constexpr bool is_future_transferrable_test( ) noexcept {
		static_assert(
		  is_future_transferrable_v<T>,
		  "Type is not trivially copyable or a true_type specialization of "
		  "is_future_transferrable.  It may not be safe to send over IPC" );
		return true;
	}

	template<typename T>
	struct ipc_promise {
		static_assert( is_future_transferrable_test<T>( ) );
		daw::expected_t<T> m_value{};
		pipe_t m_pipe{};

		ipc_promise( ) = default;

		explicit operator bool( ) const noexcept {
			return !m_value.has_exception( );
		}

		T const &get( ) {
			if( m_value.empty( ) ) {
				receive( *this );
			}
			return m_value.get( );
		}
	};

	template<>
	struct ipc_promise<void> {
		expected_t<void> m_value{};
		pipe_t m_pipe{};

		ipc_promise( ) = default;

		explicit operator bool( ) const noexcept;
		void get( );
	};

	struct binary_local_t {};

	static constexpr binary_local_t const binary_local = {};

	template<size_t N>
	using msg_t = std::array<char, N>;

	template<typename T>
	auto serialize( binary_local_t, T &&value ) noexcept {
		msg_t<sizeof( remove_cvref_t<T> )> result = {};
		memcpy( result.data( ), &value, sizeof( remove_cvref_t<T> ) );
		return result;
	}

	template<typename T, size_t N>
	T deserialize( binary_local_t, msg_t<N> msg ) noexcept {
		static_assert( sizeof( T ) == N );
		T result{};
		memcpy( &result, msg.data( ), msg.size( ) );
		return result;
	}

	template<typename T, typename Arg>
	void send( ipc_promise<T> &promise, Arg &&arg ) noexcept {
		static_assert( is_future_transferrable_test<remove_cvref_t<Arg>>( ) );
		if( !promise ) {
			return;
		}
		auto const msg = serialize( binary_local, std::forward<Arg>( arg ) );
		if( promise.m_pipe.write( msg.data( ), msg.size( ) ) < 0 ) {
			promise.m_value.set_exception(
			  daw::exception::make_exception_ptr<std::runtime_error>(
			    std::strerror( errno ) ) );
		}
	}

	template<typename T, typename Arg, typename... Args>
	void send_args( ipc_promise<T> &promise, Arg &&arg,
	                Args &&... args ) noexcept {
		if( !promise ) {
			return;
		}
		auto msg = serialize( binary_local, std::tuple<remove_cvref_t<Args>...>(
		                                      std::forward<Args>( args )... ) );

		if( promise.m_pipe.write( msg.data( ), msg.size( ) ) < 0 ) {
			promise.m_value.set_exception(
			  daw::exception::make_exception_ptr<std::runtime_error>(
			    std::strerror( errno ) ) );
		}
	}

	void send( ipc_promise<void> &promise ) noexcept;

	template<typename T>
	void receive( ipc_promise<T> &promise ) noexcept {
		if( !promise ) {
			return;
		}
		std::array<char, sizeof( T )> buff{};
		intmax_t pos = 0;
		intmax_t num_read;
		while( ( num_read = promise.m_pipe.read(
		           buff.data( ) + pos,
		           buff.size( ) - static_cast<size_t>( pos ) ) ) >= 0 ) {
			pos += num_read;
			if( pos >= static_cast<intmax_t>( sizeof( T ) ) ) {
				break;
			}
		}
		if( num_read < 0 ) {
			promise.m_value.set_exception(
			  daw::exception::make_exception_ptr<std::runtime_error>(
			    std::strerror( errno ) ) );
			return;
		}

		promise.m_value = *reinterpret_cast<T const *>( buff.data( ) );
	}

	template<typename... Args, typename T>
	std::tuple<Args...> receive_args( ipc_promise<T> &promise ) noexcept {
		if( !promise ) {
			return {};
		}
		std::tuple<remove_cvref_t<Args>...> result{};
		intmax_t pos = 0;
		intmax_t num_read;
		while( ( num_read = promise.m_pipe.read(
		           &result + pos,
		           sizeof( result ) - static_cast<size_t>( pos ) ) ) >= 0 ) {
			pos += num_read;
			if( static_cast<size_t>( pos ) >= sizeof( T ) ) {
				break;
			}
		}
		if( num_read < 0 ) {
			promise.m_value.set_exception(
			  daw::exception::make_exception_ptr<std::runtime_error>(
			    std::strerror( errno ) ) );
			return {};
		}
		return result;
	}

	void receive( ipc_promise<void> &promise ) noexcept;

	template<typename Callable, typename... CallableArgs>
	class future_process {
		static_assert( ( is_future_transferrable_test<CallableArgs>( ) && ... ) );

		Callable m_callable;

		using result_t = remove_cvref_t<decltype(
		  m_callable( std::declval<CallableArgs>( )... ) )>;

	public:
		constexpr future_process( Callable &&c )
		  : m_callable( std::forward<Callable>( c ) ) {}

		template<typename... Args>
		constexpr ipc_promise<result_t> operator( )( Args &&... args ) {
			auto result = ipc_promise<result_t>( );
			if( !result ) {
				return result;
			}

			auto const pid = fork( );
			if( pid < 0 ) {
				result.m_value.set_exception(
				  daw::exception::make_exception_ptr<std::runtime_error>(
				    std::strerror( errno ) ) );
				return result;
			}
			if( pid >= 0 ) {
				// parent
				send_args( result, std::tuple<daw::remove_cvref_t<Args>...>(
				                     std::forward<Args>( args )... ) );
				return result;
			}
			// child
			if( !result ) {
				return result;
			}
			auto fargs = receive_args<CallableArgs...>( result );
			send( result, daw::apply( m_callable, fargs ) );
			exit( result.m_value.has_value( ) ? 0 : -1 );
		}
	};

	template<typename... Args, typename Callable>
	auto make_future_process( Callable &&c ) {
		return future_process<Callable, Args...>( std::forward<Callable>( c ) );
	}
} // namespace daw
