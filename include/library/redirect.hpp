// MIT License
//
// Copyright (c) 2022 Advanced Micro Devices, Inc. All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <iostream>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <string>

namespace omnitrace
{
inline namespace config
{
bool
get_debug();

int
get_verbose();
}  // namespace config

struct redirect
{
    redirect(std::ostream& _os, std::string _expected)
    : m_os{ _os }
    , m_expected{ std::move(_expected) }
    {
        if(!get_debug())
        {
            // save stream buffer
            m_strm_buffer = m_os.rdbuf();
            // redirect to stringstream
            _os.rdbuf(m_buffer.rdbuf());
        }
    }

    ~redirect()
    {
        if(!m_strm_buffer) return;
        // restore stream buffer
        m_os.rdbuf(m_strm_buffer);
        auto _v      = m_buffer.str();
        _v           = replace(m_buffer.str(), '\n');
        auto _expect = replace(m_expected, '\n');
        if(_v != _expect)
        {
            if(get_verbose() > 0)
                std::cerr << "[omnitrace::redirect] Expected:\n[omnitrace::redirect]    "
                          << _expect
                          << "\n[omnitrace::redirect] Found:\n[omnitrace::redirect]    "
                          << _v << "\n";
            if(get_verbose() <= 0 || (&m_os != &std::cerr && &m_os != &std::cout))
                m_os << m_buffer.str() << std::flush;
        }
    }

private:
    template <typename Tp>
    static std::string replace(std::string _v, Tp _c, const std::string& _s = " ")
    {
        while(true)
        {
            auto _pos = _v.find(_c);
            if(_pos == std::string::npos) break;
            _v = _v.replace(_pos, 1, _s);
        }
        return _v;
    }

    std::ostream&     m_os;
    std::string       m_expected = {};
    std::stringstream m_buffer{};
    std::streambuf*   m_strm_buffer = nullptr;
};
}  // namespace omnitrace
