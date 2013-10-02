#include "diffalgorithms.h"

namespace GEMUFF {
    namespace Diff {

        std::vector<Node> HierarchicalDiff::CalculateDiff(
            std::vector<Hash::AbstractHashPtr> _seq_1, std::vector<Hash::AbstractHashPtr> _seq_2, float threshold)
        {
            // Process LCS
            std::vector<LCSEntry> _lcs =
                    Helper::LCS(_seq_1, _seq_2, threshold);

            //Helper::PrintList(_lcs, "LCS: ");
           // Helper::PrintList(_seq_1, "Seq1: ");
            //Helper::PrintList(_seq_2, "Seq2: ");

            int seq1_current_index = 0;
            int seq2_current_index = 0;
            std::vector<Node> result;

            // Calcular os intervalos do lcs
            // Primeiramente devemos verificar se o LCS e zero
            if (_lcs.size() == 0)
            {
                InsertNode(NULL, NULL, _seq_1, _seq_2, seq1_current_index,
                           seq2_current_index, result, threshold);
            }
            else
            {
                // Check for nodes before the starting element
                InsertNode(NULL, &_lcs[0], _seq_1, _seq_2,
                    seq1_current_index, seq2_current_index, result, threshold);

                // Percorrer o lcs
                for (int i = 0; i < _lcs.size() - 1; i++)
                    InsertNode(&_lcs[i], &_lcs[i+1], _seq_1, _seq_2,
                        seq1_current_index, seq2_current_index, result, threshold);

                InsertNode(&_lcs[_lcs.size()-1], NULL, _seq_1, _seq_2,
                    seq1_current_index, seq2_current_index, result, threshold);
            }

            return result;

        }


        void HierarchicalDiff::InsertNode(LCSEntry* startLCSElement,
            LCSEntry* endLCSElement, std::vector<Hash::AbstractHashPtr> &seq1,
            std::vector<Hash::AbstractHashPtr> &seq2, int &seq1_curIndex, int &seq2_curIndex,
            std::vector<Node> &interval, float threshold)
        {
            Node node;

            if (startLCSElement == NULL)
                //node.id1 = node.id2 = NULL;
                int xx;
            else {
                node.id1 = startLCSElement->l1_ref;
                node.id2 = startLCSElement->l2_ref;
            }

            for (; seq1_curIndex < seq1.size(); seq1_curIndex++)
            {
                if (endLCSElement != NULL)
                    if (seq1[seq1_curIndex]->toString() == endLCSElement->l1_ref->toString())
                        break;

                node.seq1.push_back(seq1[seq1_curIndex]);
            }


            for (; seq2_curIndex < seq2.size(); seq2_curIndex++)
            {
                if (endLCSElement != NULL)
                    if (seq2[seq2_curIndex]->toString() == endLCSElement->l2_ref->toString())
                        break;

                node.seq2.push_back(seq2[seq2_curIndex]);
            }

            /*if (node.id1 != NULL && node.id2 != NULL){
                node.confidence = node.id1->getSimilarity(node.id2);
                qDebug() << "n1: " << node.id1->toString().c_str();
                qDebug() << "n1: " << node.id2->toString().c_str();
                qDebug() << "confidence: " << node.confidence <<  "\n";
            }*/

            seq1_curIndex++; seq2_curIndex++;


            interval.push_back(node);
        }
    }
}


