#include "diffprocessing.h"

namespace GEMUFF {
    namespace VIMUFF {

        Diff2Processing::Diff2Processing()
        {
        }

        void Diff2Processing::CalculateDiff(Video *v1, Video *v2)
        {
            mV1 = v1;
            mV2 = v2;
            mFrameWidth = v1->getFrameWidth();
            mFrameHeight = v1->getFrameHeight();
            mFPS = v1->getFPS();

            std::vector<Hash::AbstractHashPtr>& v1_sequence_hash = v1->getSequenceHash();
            std::vector<Hash::AbstractHashPtr>& v2_sequence_hash = v2->getSequenceHash();

            #ifdef _DEBUG_TIME
            QTime time;
            time.restart();
            #endif

             // Processar video final
             int _frameSize = sizeof(uint8_t) * 4 * mFrameWidth * mFrameHeight;

            std::vector<Diff::Node> nodes =
                    Diff::HierarchicalDiff::CalculateDiff(v1_sequence_hash, v2_sequence_hash, 1.0f);

             // Criar as estruturas de diff baseado nos intervalos calculados
            GenerateDiffData(nodes);
        }


        void Diff2Processing::GenerateDiffData(std::vector<Diff::Node> &interval){

            for (int i = 0; i < interval.size(); i++)
            {
                NodeDiff2 node_diff2;
                node_diff2.id_LCS = interval[i].id1;

                // Check if both intervals have modifications
                if (interval[i].seq1.size() > 0 && interval[i].seq2.size() > 0){
                    // New LCS considering this subinterval and using a similar hash algorithm
                    std::vector<Hash::AbstractHashPtr> v1_sequence_hash;
                    std::vector<Hash::AbstractHashPtr> v2_sequence_hash;

                    for (int k = 0; k < interval[i].seq1.size(); k++){
                        QImage *img = ImageRegister::ImageAt(interval[i].seq1[k]);

                        Hash::AbstractHashPtr _h(Hash::MarrHildretchHash::GenerateHash(
                                    img->constBits(), img->height(), img->width(), 3));
                        _h->setData(interval[i].seq1[k]);
                        v1_sequence_hash.push_back(_h);
                    }

                    for (int k = 0; k < interval[i].seq2.size(); k++){
                        QImage *img = ImageRegister::ImageAt(interval[i].seq2[k]);

                        Hash::AbstractHashPtr _h(Hash::MarrHildretchHash::GenerateHash(
                                    img->constBits(), img->width(), img->height(), 3));
                        _h->setData(interval[i].seq2[k]);

                        v2_sequence_hash.push_back(_h);
                    }


                    std::vector<Diff::Node> similarHashRes = Diff::HierarchicalDiff::CalculateDiff(
                                v1_sequence_hash, v2_sequence_hash, 0.1f);

                    for (int k = 0; k < similarHashRes.size(); k++)
                    {
                        if (similarHashRes[k].id1 != NULL &&
                                similarHashRes[k].id2 != NULL) {

                            FrameDiff2 fd2;
                            fd2.v1.op = fd2.v2.op = OP_CHANGED;\

                            fd2.v1.frame_id = similarHashRes[k].id1->getData();
                            fd2.v2.frame_id = similarHashRes[k].id2->getData();
                            node_diff2.frame_diff.push_back(fd2);
                        }

                        // Elements removed from v1
                        for (int z = 0; z < similarHashRes[k].seq1.size(); z++){
                            FrameDiff2 fd2;

                            fd2.v1.op = OP_REMOVED; fd2.v2.op = OP_NONE;
                            fd2.v1.frame_id = similarHashRes[k].seq1[z]->getData();
                            //fd2.v2.frame_id = NULL;
                            node_diff2.frame_diff.push_back(fd2);
                        }

                        // Elements added to v2
                        for (int z = 0; z < similarHashRes[k].seq2.size(); z++){
                            FrameDiff2 fd2;

                            fd2.v1.op = OP_NONE; fd2.v2.op = OP_ADD;
                            //fd2.v1.frame_id = NULL;
                            fd2.v2.frame_id = similarHashRes[k].seq2[z]->getData();
                            node_diff2.frame_diff.push_back(fd2);
                        }
                    }

                    v1_sequence_hash.clear();
                    v2_sequence_hash.clear();
                } else { // Only one side has frames added or removed

                    // Frames removed from v1
                    for (int k = 0; k < interval[i].seq1.size(); k++){
                        FrameDiff2 fd2;
                        fd2.v1.op = OP_REMOVED; fd2.v2.op = OP_NONE;
                        fd2.v1.frame_id = interval[i].seq1[k];
                        //fd2.v2.frame_id = NULL;
                        node_diff2.frame_diff.push_back(fd2);
                    }

                    // Frames added to v2
                    for (int k = 0; k < interval[i].seq2.size(); k++){
                        FrameDiff2 fd2;
                        fd2.v1.op = OP_NONE; fd2.v2.op = OP_ADD;
                        //fd2.v1.frame_id = NULL;
                        fd2.v2.frame_id = interval[i].seq2[k];
                        node_diff2.frame_diff.push_back(fd2);
                    }
                }

                mDiffNodes.push_back(node_diff2);
            }          
        }
    }
}
