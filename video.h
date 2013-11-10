#ifndef VIDEO_H
#define VIDEO_H

#ifdef __cplusplus
extern "C" {
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
}
#endif

#include "imageregister.h"
#include <QTime>
#include <QDebug>
#include <opencv/cv.h>
#include <opencv/highgui.h>

namespace GEMUFF
{
    namespace VIMUFF
    {
        class Video
        {
        public:

            int LoadVideo(std::string filename);
            //int LoadFromImageSeq(std::vector<QImage>& imageSeq, int width, int height, float fps);

            std::vector<Hash::AbstractHashPtr>& getSequenceHash(){ return frame_sequence_hash; }

            int getFrameWidth(){ return frame_width; }
            int getFrameHeight(){ return frame_height; }
            int getNumFrames(){ return frame_sequence_hash.size(); }
            float getFPS(){ return mFPS; }

            Video();

        private:
            int ffmpeg_videostream;

            std::vector<Hash::AbstractHashPtr> frame_sequence_hash;

        private:
            void RetrieveHashTable(AVFormatContext *avfmtctx,
                                   AVCodecContext* avcdctx);

        private:
            int frame_width;
            int frame_height;
            float mFPS;
        };
    }
}

#endif // VIDEO_H
