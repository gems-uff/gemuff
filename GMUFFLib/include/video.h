#ifndef VIDEO_H
#define VIDEO_H

#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
extern "C" {
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libswscale/swscale.h"
    #include "libavutil/imgutils.h"
	#include "libavutil/opt.h"
}
#endif

#include "imageregister.h"
#include "GEMUFFLib_global.h"
#include "clock.h"
#include <boost/filesystem.hpp>

namespace GEMUFF
{
    namespace VIMUFF
    {
    	class Video;
        typedef boost::shared_ptr<Video> VideoPtr;

        class GEMUFFLIB_EXPORT Video
        {
        public:


        	/*
        	 * Load a video using the specified hash type
        	 */
        	static Video* loadVideo(std::string filename, Hash::HashType hashType);


        	/*
        	 * Load a video from a collection of images
        	 */
        	static Video* loadFromImages(Hash::HashType hashType, std::vector<Hash::AbstractHashPtr> _seq, int _width, int _height,
                                      AVCodecID _codec, AVPixelFormat _src_pixelfmt,
                                      AVPixelFormat _dst_pixelfmt, int _bitrate,
                                      AVRational fps, int gop_size, int max_bframes, std::string file_format);

        	/*
        	 * Save the video as a collection of images
        	 */
            void toImageSequence(std::string directory);


            int LoadFromSeq(std::vector<Hash::AbstractHashPtr> &seq,
                            int width, int height, float fps);



            std::vector<Hash::AbstractHashPtr> getSequenceHash(){ return frame_sequence_hash; }


            int LoadFromImages(std::vector<ImagePtr> _sequence, int _width, int _height,
            		AVCodecID _codec, AVPixelFormat _src_pixelfmt,
    				AVPixelFormat _dst_pixelfmt, int _bitrate,
    				AVRational fps, int gop_size, int max_bframes, char* format);


            int getFrameWidth(){ return video_ctx->width; }
            int getFrameHeight(){ return video_ctx->height; }
            int getNumFrames(){ return frame_sequence_hash.size(); }
            int getBitRate(){ return video_ctx->bit_rate;}
            int getGopSize(){ return video_ctx->gop_size; }
            int getMaxBFrames(){ return video_ctx->max_b_frames; }
            AVFormatContext* getFormatContext(){ return fmt_ctx; }
            int Save(const char* filename);
            AVCodecID getCodecID(){ return video_ctx->codec_id; }
            AVPixelFormat getPixelFormat(){ return video_ctx->pix_fmt; }
            int getFPS(){ getAVRational().num; }
            //AVRational getAVRational(){ return video_ctx->time_base; }
            AVRational getAVRational(){ return av_guess_frame_rate(fmt_ctx, fmt_ctx->streams[videostream], NULL); }
            Hash::HashType getHashType(){ return hashType; }
            Hash::AbstractHashPtr getHashAtFrame(int index){
                return frame_sequence_hash[index];
            }

            void Debug(std::string folder);
            void PrintHashSequence(std::string file);
            std::string getFormat(){
            	std::string _ss = std::string(fmt_ctx->filename);
            	_ss = _ss.substr(_ss.find_last_of(".") + 1);
            	return _ss;
            }

            ~Video();

        private:
            Video(Hash::HashType hashType);


        private:
            AVFormatContext *fmt_ctx;
            AVCodecContext* video_ctx;
            int videostream;

            Hash::HashType hashType;

            std::vector<Hash::AbstractHashPtr> frame_sequence_hash;

        private:
            int RetrieveHashTable();

            AVStream *add_video_stream(AVCodecID codec_id, int _width, int _height,
                                       AVPixelFormat _pixel_format, int _bitrate);
            AVFrame *alloc_picture(AVCodecContext *c);
            int write_video_frame(std::vector<ImagePtr> imgSeq, AVStream* avstream, SwsContext *sws_ctx);

        private:
            float mFPS;
            AVFrame *frame;
        };
    }
}

#endif // VIDEO_H
