#ifndef GEMUFFLIB_GLOBAL_H
#define GEMUFFLIB_GLOBAL_H

# define NDEBUG
# include <assert.h>

#  ifdef WIN32
#    define Q_DECL_EXPORT     __declspec(dllexport)
#    define Q_DECL_IMPORT     __declspec(dllimport)
#  else
#    define Q_DECL_EXPORT     __attribute__((visibility("default")))
#    define Q_DECL_IMPORT     __attribute__((visibility("default")))
#    define Q_DECL_HIDDEN     __attribute__((visibility("hidden")))
#  endif

#if defined(GEMUFFLIB_LIBRARY)
#  define GEMUFFLIB_EXPORT Q_DECL_EXPORT
#else
#  define GEMUFFLIB_EXPORT Q_DECL_IMPORT
#endif

#if defined( _MSC_VER) || defined(_BORLANDC_)
typedef unsigned _uint64 ulong64;
typedef signed _int64 long64;
#else
typedef unsigned long long ulong64;
typedef signed long long long64;
#endif

#define VIMUFF_GPU

namespace GEMUFF {
    namespace VIMUFF {
        class Definitions {
            public:
                static const int Context_Size = 5;
        };

        enum Device {
        	D_CPU,
			D_GPU
        };
    }
}



#endif // GEMUFFLIB_GLOBAL_H
