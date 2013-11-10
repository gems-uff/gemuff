#ifndef DIFFPROCESSING_H
#define DIFFPROCESSING_H

#include <string>
#include <vector>
#include "GEMUFF.h"
#include "video.h"
#include "helperfunctions.h"
#include "diffalgorithms.h"
#include <algorithm>

#define _DEBUG_TIME


namespace GEMUFF{
    namespace VIMUFF{

        enum Node_Type
        {
            ID_NODE_LCS,
            ID_NODE_ADD,
            ID_NODE_REMOVE,
            ID_NODE_CHANGED,
            ID_NODE_ALIGN
        };

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
        };


        struct Seq_Node
        {
            Operation op;
            std::string v1;
            std::string v2;
            std::string vDiff;
        };

        struct Contextual_Diff_Node {
            std::string elements_before[VIMUFF::Definitions::Context_Size];
            std::vector<std::string> elements;
            std::string elements_after[VIMUFF::Definitions::Context_Size];
        };



        /* No que armazena as diferenças entre os dois videos */
        struct DiffData {
            Operation op;
            std::string v1_id;
            std::string v2_id;
            std::string diff_id;
            DiffData* next;
            DiffData* prior;

            DiffData(std::string _v1_id, std::string _v2_id){
                v1_id = _v1_id;
                v2_id = _v2_id;
            }
        };

        struct LCS_Node{
            std::string id;
            DiffData* left;
            DiffData* right;
        };


        struct DiffNode {
            std::string id;
            DiffData* diffData;
        };


        /*
         *Classe responsável pelo processamento da diferença entre dois vídeos
         */
        class Diff2Processing
        {
        public:
            Diff2Processing();

            int getFrameWidth(){ return mFrameWidth; }
            int getFrameHeight(){ return mFrameHeight; }
            int getFPS(){ return mFPS; }


            void CalculateDiff(Video *v1, Video *v2);
            std::vector<NodeDiff2>& getDiffData(){ return mDiffNodes; }

        private:

            void GenerateDiffData(std::vector<Diff::Node>& interval);

        private:
            //std::vector<Frame_Diff> mFrameDiff;
            std::vector<NodeDiff2> mDiffNodes;
            float mFPS;
            int mFrameWidth;
            int mFrameHeight;
            Video* mV1;
            Video* mV2;
         };
    }
}

#endif // DIFFPROCESSING_H
