#pragma once

#include <cmath>

// Workaround for bug in GCC: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=79700
#define GCC_COMPILER (defined(__GNUC__) && !defined(__clang__))
#if GCC_COMPILER
extern "C++"
{
namespace std
{
    using ::modff;
    using ::modfl;
    using ::acosf;
    using ::acosl;
    using ::asinf;
    using ::asinl;
    using ::atan2f;
    using ::atan2l;
    using ::cosf;
    using ::cosl;
    using ::sinf;
    using ::sinl;
    using ::tanf;
    using ::tanl;
    using ::coshf;
    using ::coshl;
    using ::sinhf;
    using ::sinhl;
    using ::tanhf;
    using ::tanhl;
    using ::expf;
    using ::expl;
    using ::frexpf;
    using ::frexpl;
    using ::ldexpf;
    using ::ldexpl;
    using ::logf;
    using ::logl;
    using ::log10f;
    using ::log10l;
    using ::powf;
    using ::powl;
    using ::sqrtf;
    using ::sqrtl;
    using ::ceilf;
    using ::ceill;
    using ::floorf;
    using ::floorl;
    using ::fmodf;
    using ::fmodl;
} // namespace std
} // extern "C++"
#endif

