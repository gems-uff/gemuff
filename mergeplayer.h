#ifndef MERGEPLAYER_H
#define MERGEPLAYER_H

#include <QLabel>
#include <QPainter>
#include "diffalgorithms.h"
#include  "player.h"
#include "Helper.h"

namespace GEMUFF {

    namespace VIMUFF {

        /*
         * Forward declaration
         */
        struct FramePlayer{
            VIMUFF::ImagePtr base;
            VIMUFF::ImagePtr v1;
            VIMUFF::ImagePtr v2;
            Diff::DiffOperation op_b_v1;
            Diff::DiffOperation op_b_v2;

            FramePlayer(){
                op_b_v1 = op_b_v2 = Diff::DO_None;
            }
        };


        class MergePlayer : public Player
        {
        public:
            MergePlayer();
            void SetDisplays(QLabel* _vbase, QLabel* _v1, QLabel* _v2, QLabel* _merge);
            void SetData(Diff::Diff3Info *_diff3);
            void SetBase(VideoPtr _base){ vbase = _base; }
            int GetTimelineLenght();
            void SetTime(int time);
            int getNumFrames(Diff::SeqMark& seqMark, std::vector<Diff::DiffChunk>& diffChunk);
            std::vector<FramePlayer> generateSubSequence(Diff::SeqMark seqMark, std::vector<Diff::DiffChunk>& diffChunk, int side);

        private:
            void GeneratePlayerData();

        private:
            Diff::Diff3Info *diff3;
            int currentIndex;
            std::vector<FramePlayer> mFrames;
            QLabel* video_1;
            QLabel* video_2;
            QLabel* video_merge;
            QLabel* video_base;
            VideoPtr vbase;
        };
    }
}
#endif // MERGEPLAYER_H
