#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <iostream>
#include <fstream>
#include "video.h"
#include "diff2data.h"
#include "diffalgorithms.h"

namespace GEMUFF {
    namespace VIMUFF {

        enum Operation{
            OP_ADD,
            OP_REMOVED,
            OP_CHANGED,
            OP_NONE
        };

        struct FrameDiff {
            Operation op;
            Hash::AbstractHashPtr frame_id;
        };

        struct FrameDiff2 {
            FrameDiff v1;
            FrameDiff v2;
        };


        struct NodeDiff2 {
            Hash::AbstractHashPtr id_LCS;
            std::vector<FrameDiff2> frame_diff;

            void Serialize(std::ofstream _ofstream){

            }


        };

        struct Diff2Data {
            std::vector<NodeDiff2> mDiffNodes;

            void SaveToFile(std::ofstream& _ofstream){
                int size = mDiffNodes.size();

                _ofstream.write((char*) &size);

                for (int i = 0; i < size; i++){
                    NodeDiff2 _nodeDiff = mDiffNodes[i];


                }

            }

            void LoadFromFile(std::ifstream& _ifstream){

            }
        };



        class Processor
        {
        public:
            static Diff2Data Diff2(Video *v1, Video *v2, float similarity);
            static Video Patch(Video* v2, Diff2Data& diff2);

        private:
            static void GenerateDiffData(std::vector<Diff::Node>& interval, Diff2Data& res);
        };
    }
}


#endif // PROCESSOR_H
