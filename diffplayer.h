#ifndef DIFFPLAYER_H
#define DIFFPLAYER_H

#include <QLabel>
#include <QPainter>
#include "player.h"
#include "imageregister.h"
#include "diffalgorithms.h"


namespace GEMUFF{
    namespace VIMUFF {

        /*
         * Forward declaration
         */
        struct DiffFramePlayer{
            VIMUFF::ImagePtr v1;
            VIMUFF::ImagePtr v2;
            Diff::DiffOperation op;
        };

        class Diff2Player : public Player
        {
        public:
            Diff2Player();
            void Clear();
            void SetDisplays(QLabel* _v1, QLabel* _v2, QLabel* _diff);
            void SetData(Diff::Diff2Info *_diff2);
            void SetVideo(Video *_v1){ video1 = _v1; }
            int GetTimelineLenght();
            void SetTime(int time);

        private:
            void GenerateDiffPlayerData();

        private:
            Diff::Diff2Info *diff2;
            int currentIndex;
            std::vector<DiffFramePlayer> mFrames;
            QLabel* video_1;
            QLabel* video_2;
            QLabel* video_diff;
            Video* video1;
        };
    }
}
#endif // DIFFPLAYER_H
