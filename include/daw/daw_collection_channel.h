// The MIT License (MIT)
//
// Copyright (c) 2019 Darrell Wright
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

#include <algorithm>
#include <array>
#include <iterator>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

#include <daw/daw_utility.h>
#include <daw/cpp_17.h>

#include "daw_channel.h"

namespace daw::process {
	template<typename T, size_t max_items_per_message = 10>
	class collection_channel {
		using buffer_t = std::array<std::optional<T>, max_items_per_message>;
		daw::process::channel<buffer_t> m_channel{};

	public:
		collection_channel( ) noexcept = default;

		template<typename Collection,
		         daw::enable_if_t<!std::is_same_v<
		           collection_channel, daw::remove_cvref_t<Collection>>> = nullptr>
		void write( Collection &&collection ) noexcept {
			buffer_t buff = {};
			auto first = std::begin( collection );
			auto last = std::end( collection );
			while( first != last ) {
				auto buff_it = buff.begin( );
				while( first != last and buff_it != std::end( buff ) ) {
					*buff_it = *first;
					++first;
					++buff_it;
				}
				if( buff_it != std::end( buff ) ) {
					*buff_it = std::nullopt;
				}
				m_channel.write( buff );
			}
			buff.front( ) = std::nullopt;
			m_channel.write( buff );
		}

		inline std::vector<T> read( ) noexcept {
			auto result = std::vector<T>( );
			auto msg = m_channel.read( );
			while( msg.front( ) ) {
				auto it = msg.begin( );
				while( *it ) {
					result.push_back( **it );
					++it;
				}
				msg = m_channel.read( );
			}
			return result;
		}
	};
} // namespace daw::process
