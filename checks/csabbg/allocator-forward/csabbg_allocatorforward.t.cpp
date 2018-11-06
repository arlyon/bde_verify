// csabbg_allocatorforward.t.cpp                                      -*-C++-*-

#include "csabbg_allocatorforward.t.hpp"
#include <bdes_ident.h>
#include <bslma_testallocator.h>

// -----------------------------------------------------------------------------

namespace bde_verify
{
    namespace csabbg
    {
        namespace
        {
            class test:
                public bde_verify::csabbg::allocatorforward_alloc_unused,
                public bde_verify::csabbg::allocatorforward_alloc_used
            {
            public:
                test(int);                                          // IMPLICIT
                test(int, BloombergLP::bslma::Allocator*);
                test(bool, BloombergLP::bslma::Allocator*);
            private:
                BloombergLP::bslma::Allocator* allocator_;
                bde_verify::csabbg::allocatorforward_alloc_unused    unused_;
                bde_verify::csabbg::allocatorforward_alloc_used      used0_;
                bde_verify::csabbg::allocatorforward_alloc_used      used1_;
                bde_verify::csabbg::allocatorforward_alloc_used      used2_;
            };
        }
    }
}

// -----------------------------------------------------------------------------

bde_verify::csabbg::test::test(int i):
    unused_(),
    used1_(i)
{
}

// -----------------------------------------------------------------------------

bde_verify::csabbg::test::test(int i, BloombergLP::bslma::Allocator* alloc):
    allocatorforward_alloc_used(i),
    used0_(i),
    used1_(i, alloc)
{
}

// -----------------------------------------------------------------------------

namespace bde_verify
{
    namespace csabbg
    {
        namespace
        {
            int dummy(BloombergLP::bslma::Allocator* a)
            {
                return a? 1: 0;
            }
        }
    }
}

bde_verify::csabbg::test::test(bool, BloombergLP::bslma::Allocator* alloc):
    allocatorforward_alloc_used(-1, alloc),
    allocator_(alloc),
    used0_(bde_verify::csabbg::dummy(alloc)),
    used1_(1, allocator_),
    used2_(2, alloc)
{
}

// -----------------------------------------------------------------------------

namespace bde_verify
{
    namespace csabbg
    {
        namespace
        {
            template <class T>
            struct tbase
            {
                tbase();
                explicit tbase(BloombergLP::bslma::Allocator*);
                tbase(int, BloombergLP::bslma::Allocator*);
                bde_verify::csabbg::allocatorforward_alloc_unused unused_;
                bde_verify::csabbg::allocatorforward_alloc_used   used0_;
            };

            template <class T>
            tbase<T>::tbase()
            {
            }

            template <class T>
            tbase<T>::tbase(BloombergLP::bslma::Allocator*)
            {
            }

            template <class T>
            tbase<T>::tbase(int i, BloombergLP::bslma::Allocator* alloc):
                used0_(i, alloc)
            {
            }
        }
    }
}

void
bde_verify::csabbg::operator+(allocatorforward_alloc_used)
{
    bde_verify::csabbg::tbase<int> tb0;
    bde_verify::csabbg::tbase<int> tb1(static_cast<BloombergLP::bslma::Allocator*>(0));
    bde_verify::csabbg::tbase<int> tb2(2, static_cast<BloombergLP::bslma::Allocator*>(0));
}

// -----------------------------------------------------------------------------
namespace bsl { template <class T> struct allocator { }; }
namespace bde_verify
{
    namespace csabbg
    {
        namespace
        {
            template <class T>
                struct M { M(BloombergLP::bslma::Allocator*) {} };
            template <class T, class A = bsl::allocator/*M*/<T> >
                struct S { S(const T*) {} S(const T*, const A&) {} };
            template class M<char>;  // TBD should not be needed
            struct C {
                S<char> s;
                C(BloombergLP::bslma::Allocator*) : s("") { }
            };
        }
    }
}

// -----------------------------------------------------------------------------

namespace bde_verify
{
    namespace csabbg
    {
        struct not_alloc {
            not_alloc(BloombergLP::bslma::Allocator*) { }
        };
        struct object {
            not_alloc member;
            object(not_alloc na, BloombergLP::bslma::Allocator* = 0)
            : member(na) { }
        };
    }
}

namespace BloombergLP
{
    namespace bslma
    {
        template <>
        struct UsesBslmaAllocator<bde_verify::csabbg::object> : bsl::true_type
        {
        };

    }
}

namespace bde_verify
{
    namespace csabbg
    {
        void test(BloombergLP::bslma::Allocator *a)
        {
            object o(a);
        }
        object ra(bool b, const object &o)
        {
            if (b) {
                return o;
            } else {
                return object(o);
            }
        }
    }
}

#include <bslmf_nestedtraitdeclaration.h>
#include <bslma_usesbslmaallocator.h>
#include <bslalg_typetraits.h>
#include <bslalg_typetraitusesbslmaallocator.h>

namespace bde_verify
{
namespace csabbg
{
using namespace BloombergLP;

struct alloc_a {
    BSLMF_NESTED_TRAIT_DECLARATION(alloc_a, bslma::UsesBslmaAllocator);
    explicit alloc_a(bslma::Allocator* = 0) { }
};

struct alloc_b {
    BSLALG_DECLARE_NESTED_TRAITS(alloc_b, bslalg::TypeTraitUsesBslmaAllocator);
    explicit alloc_b(bslma::Allocator* = 0) { }
};

struct alloc_c {
    explicit alloc_c(bslma::Allocator* = 0) { }
};

struct alloc_d {
    explicit alloc_d(bslma::Allocator* = 0) { }
};

struct alloc_e {
    BSLMF_NESTED_TRAIT_DECLARATION_IF(alloc_e, bslma::UsesBslmaAllocator, true);
    explicit alloc_e(bslma::Allocator* = 0) { }
};

struct alloc_f {
    BSLMF_NESTED_TRAIT_DECLARATION_IF(alloc_f, bslma::UsesBslmaAllocator, false);
    explicit alloc_f(bslma::Allocator* = 0) { }
};

template <class TYPE>
struct alloc_g {
    explicit alloc_g(bslma::Allocator* = 0) { }
};

template <class TYPE>
struct alloc_h {
    explicit alloc_h(bslma::Allocator* = 0) { }
};

template <class TYPE>
struct alloc_i {
    BSLALG_DECLARE_NESTED_TRAITS(alloc_i, bslalg::TypeTraitUsesBslmaAllocator);
    explicit alloc_i(bslma::Allocator* = 0) { }
};

template <class TYPE>
struct alloc_j {
    BSLALG_DECLARE_NESTED_TRAITS(alloc_j, bslalg::TypeTraitUsesBslmaAllocator);
    explicit alloc_j(bslma::Allocator* = 0) { }
};

template class alloc_j<int>;

template <class TYPE>
struct alloc_k {
    BSLMF_NESTED_TRAIT_DECLARATION_IF(alloc_k, bslma::UsesBslmaAllocator, true);
    explicit alloc_k(bslma::Allocator* = 0) { }
};

template <class TYPE>
struct alloc_l {
    BSLMF_NESTED_TRAIT_DECLARATION_IF(alloc_l, bslma::UsesBslmaAllocator, false);
    explicit alloc_l(bslma::Allocator* = 0) { }
};

template <class TYPE>
struct alloc_m {
    BSLMF_NESTED_TRAIT_DECLARATION_IF(alloc_m, bslma::UsesBslmaAllocator, true);
    explicit alloc_m(bslma::Allocator* = 0) { }
};

template class alloc_m<int>;

template <class TYPE>
struct alloc_n {
    BSLMF_NESTED_TRAIT_DECLARATION_IF(alloc_n, bslma::UsesBslmaAllocator, false);
    explicit alloc_n(bslma::Allocator* = 0) { }
};

template class alloc_n<int>;

template <class TYPE>
struct alloc_o {
    explicit alloc_o(bslma::Allocator* = 0) { }
};

template <class TYPE>
struct alloc_p {
    explicit alloc_p(bslma::Allocator* = 0) { }
};

template class alloc_p<int>;

}
}

namespace BloombergLP
{
namespace bslma
{
template <>
struct UsesBslmaAllocator<bde_verify::csabbg::alloc_c> : bsl::true_type { };

template <class TYPE>
struct UsesBslmaAllocator<bde_verify::csabbg::alloc_g<TYPE> >
: bsl::true_type { };

template <class TYPE>
struct UsesBslmaAllocator<bde_verify::csabbg::alloc_h<TYPE> >
: bsl::false_type { };
}
}

bde_verify::csabbg::alloc_a a{};
bde_verify::csabbg::alloc_a b{0};
bde_verify::csabbg::alloc_a c[2];
bde_verify::csabbg::alloc_a d[2][2];
bde_verify::csabbg::alloc_f f{};

BloombergLP::bslma::TestAllocator ta;

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
