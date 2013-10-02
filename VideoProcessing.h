#ifndef VIDEOPROCESSING_H
#define VIDEOPROCESSING_H

#include "video.h"
#include <vector>
#include <string>
#include "K_IMUFF.h"
#include <QDebug>
#include <QPainter>
#include <QLabel>
#include "diffprocessing.h"

namespace GEMUFF
{
    namespace VIMUFF
    {


        struct FrameInfo{
            std::string id;
            uint8_t* aux_buffer;
        };

        /* Operacao realizada no frame */
        enum _Operation
        {
            NONE,
            XOR,
            ADD,
            REMOVE,
            UNKNOWN
        };

        enum Position
        {
            LEFT,
            RIGHT
        };


        enum IntervalType
        {
            NORMAL,
            BEGIN,
            END
        };

        struct sPoint
        {
            IntervalType type;
            int point;
        };

        struct sInterval
        {
            sPoint start;
            sPoint end;
        };

        // Estrutura que armazena diff entre blocos de imagens
        struct Frame_Diff
        {
            int numFrames;
            _Operation op;
            uint8_t *buffer;
         };









        class PatchProcessing
        {
        public:
            Video Patch(Video *video, Diff2Processing& delta );
            void Save(std::string filename);

            //Frame_Diff* DiffAtFrame(int index);
            //void SaveDiff(std::string filename);
            //void LoadDiff(std::string filename);

        private:
            float mFPS;
            int mFrameWidth;
            int mFrameHeight;

         };


        struct sTableForViewItem
        {
            std::string baseFrame;
            Frame_Diff* v1_diff;
            Frame_Diff* v2_diff;
            int v1_idx;
            int v2_idx;
        };


        struct sMergeFrame
        {
            _Operation op;
            uint8_t* data;
            bool show;
            std::string frame_by_key;
        };

        struct sMergeResult
        {
            _Operation op;
            uint8_t* data;
            bool paused;
            std::string frame_by_key;
            bool conflicted;
        };

        struct sMergeLines
        {
            std::vector<sMergeFrame> channelBase;
            std::vector<sMergeFrame> channelA;
            std::vector<sMergeFrame> channelB;
            std::vector<sMergeResult> channelMerge;
        };

        class MergeProcessing
        {
        public:

            struct MergeItem
            {
                std::string baseFrame;

                Frame_Diff* v1_frame_diff;

                Frame_Diff* v2_frame_diff;

                int v1_offset;
                int v2_offset;
            };

        public:
            void Merge(Video *base, Video *v1, Video *v2);
            void DebugMergeTable();
            void GenerateShowTable();
            std::map<int,MergeItem>& getMergeTable(){ return mergeTable; }
            sMergeLines& getMergeChannels();

        private:
            std::map<int,MergeItem> mergeTable;
            Diff2Processing diff_base_v1;
            Diff2Processing diff_base_v2;
            sMergeLines mergeChannels;

        };
    }
}

#endif // VIDEOPROCESSING_H
