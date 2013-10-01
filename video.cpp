#include "video.h"

namespace GEMUFF
{
    namespace VIMUFF
    {
        Video::Video()
        {
        }

        int Video::LoadVideo(std::string filename)
        {
            AVFormatContext *fmt_ctx = NULL;

            if (avformat_open_input(&fmt_ctx, filename.c_str(), NULL, NULL) != 0)
                return -1;

            // Recuperar informacao do stream
            if (av_find_stream_info(fmt_ctx) < 0)
                return -1; // Sem informacao



            //av_dump_format(v1_fmt_ctx, 0, file1.c_str(), 0);

            // Recuperar o stream de video
            ffmpeg_videostream = -1;

            AVCodecContext *codec_ctx;

            for (int i = 0; i < fmt_ctx->nb_streams; i++ )
                if (fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
                {
                    ffmpeg_videostream = i;
                    break;
                }

            if (ffmpeg_videostream == -1)
                return -1;

            // Recuperar o ponteiro para o codec do video
            codec_ctx = fmt_ctx->streams[ffmpeg_videostream]->codec;

            frame_width = codec_ctx->width;
            frame_height = codec_ctx->height;

            // Abrir o codec
            AVCodec *codec;
            codec = avcodec_find_decoder(codec_ctx->codec_id);

            if (codec == NULL)
                return -1;

            // Abrir codec
            AVDictionary *optionsDict = NULL;
            if (avcodec_open2(codec_ctx, codec, &optionsDict) < 0)
                return -1;

            // Recuperar a tabela de hash de cada filme
            QTime time;
            time.restart();
            RetrieveHashTable(fmt_ctx, codec_ctx);
            qDebug() << "Calculo da tabela de hash(ms): " << time.elapsed();
            qDebug() << "Numero de Frames: " << frame_sequence_hash.size();

            avcodec_close(codec_ctx);
            avformat_close_input(&fmt_ctx);

            return 0;
        }

        int Video::LoadFromImageSeq(std::vector<QImage> &imageSeq, int width, int height, float fps)
        {
            frame_width = width;
            frame_height = height;
            mFPS = fps;

            for (size_t i = 0; i < imageSeq.size(); i++)
            {
                Hash::AbstractHash *_hash = ImageRegister::RegisterFrame(imageSeq[i]);
                frame_sequence_hash.push_back((Hash::AbstractHash*) _hash);
            }

            return 0;
        }


        void Video::RetrieveHashTable(AVFormatContext *avfmtctx, AVCodecContext *avcdctx)
        {
            struct SwsContext *sws_ctx = NULL;
            AVFrame *pFrame = NULL;
            AVFrame *pFrameRGB = NULL;

            // Alocar video frame
            pFrame = avcodec_alloc_frame();
            pFrameRGB = avcodec_alloc_frame();

            if (pFrameRGB == NULL)
                return;

            uint8_t *buffer;
            int numBytes;

            numBytes = avpicture_get_size(PIX_FMT_RGB24, avcdctx->width,
                                          avcdctx->height) + 15;
            buffer = (uint8_t*) av_malloc(numBytes * sizeof(uint8_t));
            sprintf((char *) buffer, "P6\n%d %d\n255\n", avcdctx->width, avcdctx->height);

            sws_ctx = sws_getContext(
                        avcdctx->width,
                        avcdctx->height,
                        avcdctx->pix_fmt,
                        avcdctx->width,
                        avcdctx->height,
                        PIX_FMT_RGB24,
                        SWS_POINT,
                        NULL,
                        NULL,
                        NULL);

            avpicture_fill((AVPicture*)pFrameRGB, &buffer[15], PIX_FMT_RGB24,
                           avcdctx->width, avcdctx->height);

            // Ler dados
            int frameFinished;
            AVPacket packet;

            while (av_read_frame(avfmtctx, &packet) >= 0)
            {
                if (packet.stream_index == ffmpeg_videostream)
                {
                    //decodificar
                    avcodec_decode_video2(avcdctx, pFrame, &frameFinished,
                                          &packet);

                    // Terminou de recuperar o frame de video?
                    if (frameFinished)
                    {
                        // Converter a imagem para o formato final
                        sws_scale(sws_ctx,
                                  (uint8_t const * const*)pFrame->data,
                                  pFrame->linesize,
                                  0,
                                  avcdctx->height,
                                  pFrameRGB->data,
                                  pFrameRGB->linesize);


                        QImage qimg = QImage::fromData((const uchar*)buffer, numBytes, "PPM");
                        Hash::AbstractHash *_hash = ImageRegister::RegisterFrame(qimg);
                        frame_sequence_hash.push_back(_hash);
                    }
                }

                // liberar package
                av_free_packet(&packet);
            }
            av_free(buffer);
            av_free(pFrameRGB);
            av_free(pFrame);
        }
    }
}
