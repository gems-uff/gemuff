#ifndef MERGEPLAYER_H
#define MERGEPLAYER_H

#include <QLabel>
#include <QPainter>
#include "diffalgorithms.h"
#include  "player.h"

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
        };

        class MergePlayer : public Player
        {
        public:
            MergePlayer();
            void SetDisplays(QLabel* _vbase, QLabel* _v1, QLabel* _v2, QLabel* _merge);
            void SetData(Diff::Diff3Info *_diff3);
            void SetBase(Video *_base){ vbase = _base; }
            int GetTimelineLenght();
            void SetTime(int time);

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
            Video* vbase;
        };
    }
}
#endif // MERGEPLAYER_H
