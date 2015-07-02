#ifndef VIDEO_H
#define VIDEO_H

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
extern "C" {
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
    #include "libavutil/imgutils.h"
}
#endif

#include "imageregister.h"
#include <QTime>
#include <QDebug>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "GEMUFF.h"

namespace GEMUFF
{
    namespace VIMUFF
    {
        class Video
        {
        public:


            int LoadVideo(std::string filename);
            int LoadFromSeq(std::vector<Hash::AbstractHashPtr> &seq,
                            int width, int height, float fps);

            std::vector<Hash::AbstractHashPtr> getSequenceHash(){ return frame_sequence_hash; }

            int LoadFromImages(std::vector<Hash::AbstractHashPtr>& _seq, int _width, int _height,
                                      AVCodecID _codec, AVPixelFormat _src_pixelfmt,
                                      AVPixelFormat _dst_pixelfmt, int _bitrate,
                                      AVRational fps, int gop_size, int max_bframes, AVFormatContext *_fmtctx);

            int getFrameWidth(){ return codec_context->width; }
            int getFrameHeight(){ return codec_context->height; }
            int getNumFrames(){ return frame_sequence_hash.size(); }
            int getBitRate(){ return fmt_ctx->bit_rate;}
            int getGopSize(){ return codec_context->gop_size; }
            int getMaxBFrames(){ return codec_context->max_b_frames; }
            AVFormatContext* getFormatContext(){ return fmt_ctx; }
            AVCodecID getCodecID(){ return codec_context->codec_id; }
            AVPixelFormat getPixelFormat(){ return codec_context->pix_fmt; }
            int getFPS(){ mFPS; }
            AVRational getAVRational(){ return codec_context->time_base; }
            Hash::AbstractHashPtr getHashAtFrame(int index){
                return frame_sequence_hash[index];
            }

            void Debug(std::string folder);

            Video();
            ~Video();

        private:
            int ffmpeg_videostream;
            AVCodecContext* codec_context;
            AVFormatContext *fmt_ctx;

            std::vector<Hash::AbstractHashPtr> frame_sequence_hash;

        private:
            void RetrieveHashTable(AVFormatContext *avfmtctx,
                                   AVCodecContext* avcdctx);

            AVStream *add_video_stream(AVCodecID codec_id, int _width, int _height,
                                       AVPixelFormat _pixel_format, int _bitrate);
            AVFrame *alloc_picture(AVCodecContext *c);
            int write_video_frame(ImagePtr img, AVPixelFormat _src_pixelfmt, AVStream *st,
                                   SwsContext *sws_ctx, AVPicture *_src, AVFrame *_dst, int frame_number);

        private:
            float mFPS;
                        AVFrame *frame;
        };
    }
}

#endif // VIDEO_H
