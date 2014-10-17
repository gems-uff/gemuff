#ifndef HELPERFUNCTIONS_H
#define HELPERFUNCTIONS_H

#include <vector>
#include <string>
#include "hash.h"
#include <QDebug>

namespace GEMUFF {

    struct LCSEntry {
        Hash::AbstractHashPtr l1_ref;
        Hash::AbstractHashPtr l2_ref;
    };

    class Helper {
    public:
        static void PrintList(std::vector<Hash::AbstractHash*>& list, char* name);
        static void PrintList(std::vector<LCSEntry> &list, char *name);
        static std::vector<LCSEntry> LCS(std::vector<Hash::AbstractHashPtr> _s,
                                         std::vector<Hash::AbstractHashPtr> _t,
                                         float thresold);

        static void PrintList(std::vector<Hash::AbstractHashPtr> &list, char *name);
    };
}

#endif // HELPERFUNCTIONS_H
