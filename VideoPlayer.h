#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QSlider>
#include <QLabel>
#include <QPainter>
#include <QObject>
#include "player.h"
#include "video.h"
#include "diffalgorithms.h"
#include "diffplayer.h"

namespace GEMUFF {
    namespace VIMUFF {

        class VideoPlayer : public QObject {
            Q_OBJECT
        private:
            Video* video;
            QSlider* slider;
            QLabel* display;
            int currentIndex;
            int totalFrames;
            int bufferSize;


        public:
            VideoPlayer(){
                video = NULL;
                slider = NULL;
                display = NULL;
                currentIndex = 0;
            }

            void setVideo(Video* _video){
                video = _video;
                totalFrames = _video->getSequenceHash().size();

                if (slider){
                    slider->setMinimum(0);
                    slider->setMaximum(_video->getSequenceHash().size()-1);
                }
            }

            void setSlider(QSlider* _slider);

            void setDisplay(QLabel* _display){
                display = _display;
            }


        public slots:
            void setTimer(int time);
        };



        class PatchPlayer : public QObject {
            Q_OBJECT
        private:
            Video* v1;
            QSlider* slider;
            QLabel* video_1;
            QLabel* video_2;
            QLabel* video_diff;
            int currentIndex;
            int totalFrames;
            int bufferSize;
            GEMUFF::Diff::Diff2Info *diff2;
            std::vector<DiffFramePlayer> mFrames;

            void InternalProcess();

        public:
            PatchPlayer(){
                v1 = NULL;
                slider = NULL;
                video_1 = video_2 = video_diff = NULL;
                currentIndex = 0;
                bufferSize = 5;
            }

            void setVideo(Video* _video){
                v1 = _video;
            }

            void Process(GEMUFF::Diff::Diff2Info *_diff2Info){
                diff2 = _diff2Info;

                InternalProcess();
            }


            void setSlider(QSlider* _slider);

            void setDisplay(QLabel *v1, QLabel *diff, QLabel *patched){
                video_1 = v1;
                video_2 = diff;
                video_diff = patched;

            }



        public slots:
            void setTimer(int time);
        };
    }

}

#endif // VIDEOPLAYER_H
