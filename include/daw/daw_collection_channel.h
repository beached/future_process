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

#include "daw_channel.h"

namespace daw::process {
	namespace impl {
		template<typename T, size_t Size>
		struct buffer_t {
			std::array<T, Size> m_values = {};
			size_t m_value_count = 0;
		};
	} // namespace impl
	template<typename T, size_t max_items_per_message = 10>
	class collection_channel {
		using buffer_t = impl::buffer_t<T, max_items_per_message>;
		daw::process::channel<std::optional<buffer_t>> m_channel{};

	public:
		collection_channel( ) noexcept = default;

		template<typename Collection,
		         std::enable_if_t<!std::is_same_v<collection_channel,
		                                          daw::remove_cvref_t<Collection>>,
		                          std::nullptr_t> = nullptr>
		inline void write( Collection &&collection ) {
			auto first = std::begin( collection );
			auto last = std::end( collection );
			while( first != last ) {
				buffer_t buff = {};
				for( ; buff.m_value_count < max_items_per_message and first != last;
				     ++buff.m_value_count, ++first ) {

					buff.m_values[buff.m_value_count] = *first;
				}
				if( buff.m_value_count > 0 ) {
					m_channel.write( buff );
				}
			}
			m_channel.write( std::nullopt );
		}

		template<typename Result = std::vector<T>>
		inline Result read( ) {
			auto result = Result( );
			auto msg = m_channel.read( );
			auto it_out = std::back_inserter( result );
			while( msg ) {
				std::copy_n( msg->m_values.begin( ), msg->m_value_count, it_out );
				msg = m_channel.read( );
			}
			return result;
		}
	};
} // namespace daw::process
