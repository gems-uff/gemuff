#ifndef GEMUFF_H
#define GEMUFF_H

#if defined( _MSC_VER) || defined(_BORLANDC_)
typedef unsigned _uint64 ulong64;
typedef signed _int64 long64;
#else
typedef unsigned long long ulong64;
typedef signed long long long64;
#endif

namespace GEMUFF {
    namespace VIMUFF {
        class Definitions {
            public:
                static const int Context_Size = 5;
        };
    }
}

#endif // GEMUFF_H
