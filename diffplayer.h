#ifndef DIFFPLAYER_H
#define DIFFPLAYER_H

#include <QLabel>
#include <QPainter>
#include "player.h"
#include "diffprocessing.h"


namespace GEMUFF{
    namespace VIMUFF {

        /*
         * Forward declaration
         */
        struct DiffFramePlayer{
            Hash::AbstractHashPtr v1;
            Hash::AbstractHashPtr v2;
            Operation op;
        };

        class Diff2Player : public Player
        {
        public:
            Diff2Player();
            void SetDisplays(QLabel* _v1, QLabel* _v2, QLabel* _diff);
            void SetDiffPlayer(Diff2Processing* diff);
            int GetTimelineLenght();
            void SetTime(int time);

        private:
            void GenerateDiffPlayerData();

        private:
            Diff2Processing* diffProcessing;
            int currentIndex;
            std::vector<DiffFramePlayer> mFrames;
            QLabel* video_1;
            QLabel* video_2;
            QLabel* video_diff;
        };
    }
}
#endif // DIFFPLAYER_H
