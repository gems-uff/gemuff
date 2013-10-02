#ifndef DIFFALGORITHMS_H
#define DIFFALGORITHMS_H

#include <vector>
#include <string>
#include "helperfunctions.h"

namespace GEMUFF {
    namespace Diff {

    #define ID_NODE_UNKNOW "UNKNOWN"
    #define ID_NODE_BLANK "NODE_BLANK"

        struct Node
        {
            Hash::AbstractHashPtr id1;
            Hash::AbstractHashPtr id2;
            float confidence;
            std::vector<Hash::AbstractHashPtr> seq1;
            std::vector<Hash::AbstractHashPtr> seq2;
        };

        class HierarchicalDiff {
        public:
            static std::vector<Node> CalculateDiff(
                std::vector<Hash::AbstractHashPtr> _seq_1,
                std::vector<Hash::AbstractHashPtr> _seq_2,
                float threshold);

        private:
            static void InsertNode(LCSEntry* startLCSElement,
                                   LCSEntry* endLCSElement, std::vector<Hash::AbstractHashPtr> &seq1,
                                   std::vector<Hash::AbstractHashPtr> &seq2, int &seq1_curIndex, int &seq2_curIndex,
                                   std::vector<Node> &interval, float threshold);
        };
    }
}


#endif // DIFFALGORITHMS_H
