template <class T> inline void call_h(joe<T>& j) { j.h(); }
                   inline void call_k(moe&    j) { j.k(); }

namespace N {
struct G { };
static const G g = {};

template <class M>
struct B {
    template <class A> B(const A&);
};

template <class M>
struct F : B<M> {
    template <class A> F();
    template <class A> F(const A&);
    template <class A> F(G, const A&);
    template <class A> void H() { }
};
}

template <class M>
template <class A>
inline N::B<M>::B(const A&) { }

template <class M>
template <class A>
inline N::F<M>::F(const A& a) : B<M>(a) { }

template <class M>
template <class A>
inline N::F<M>::F(G, const A& a) : B<M>(a) { }

inline void h()
{
    N::F<char> f(N::g, 3);
    f.H<short>();
}

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
