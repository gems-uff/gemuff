#include "video.h"

namespace GEMUFF
{
    namespace VIMUFF
    {
        Video::Video()
        {
            fmt_ctx = NULL;
            codec_context = NULL;
        }

        Video::~Video(){
            if (codec_context != NULL) avcodec_close(codec_context);
            //if (fmt_ctx != NULL) avformat_close_input(&fmt_ctx);

            codec_context = NULL;
            fmt_ctx = NULL;
        }

        void Video::Debug(std::string folder){

            for (int i = 0; i < frame_sequence_hash.size(); i++ ){
                std::stringstream ss;
                ss << folder << "/" << i << ".png";

                ImageRegister::ImageAt(frame_sequence_hash[i])->toQImage().save(ss.str().c_str());
            }
        }



        int Video::LoadVideo(std::string filename)
        {
            if (avformat_open_input(&fmt_ctx, filename.c_str(), NULL, NULL) != 0){
                fprintf(stderr, "Error opening file: %s\n", filename.c_str());
                return  -1;
            }

            // Recuperar informacao do stream
            if (avformat_find_stream_info(fmt_ctx, NULL) < 0){
                fprintf(stderr, "No stream info!: %s\n", filename.c_str());
                return -1; // Sem informacao
            }

            // Dump information about the format
            av_dump_format(fmt_ctx, 0, filename.c_str(), 0);

            // Recuperar o stream de video
            ffmpeg_videostream = -1;

            for (int i = 0; i < fmt_ctx->nb_streams; i++ ){
                if (fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
                {
                    ffmpeg_videostream = i;
                    break;
                }
            }

            if (ffmpeg_videostream == -1){
                fprintf(stderr, "No video stream available on file %s\n", filename.c_str());
                return -1;
            }

            // Recuperar o ponteiro para o codec do video
            codec_context = fmt_ctx->streams[ffmpeg_videostream]->codec;

            // Abrir o codec
            AVCodec *codec = avcodec_find_decoder(codec_context->codec_id);

            if (codec == NULL){
                fprintf(stderr, "Unsupported codec on file %s\n", filename.c_str());
                return -1;
            }

            // Abrir codec
            AVDictionary *optionsDict = NULL;
            if (avcodec_open2(codec_context, codec, &optionsDict) < 0){
                fprintf(stderr, "Could not open codec on file %s\n", filename.c_str());
                return -1;
            }

            // Recuperar a tabela de hash de cada filme
            QTime time;
            time.restart();
            RetrieveHashTable(fmt_ctx, codec_context);
            //qDebug() << "Calculo da tabela de hash(ms): " << time.elapsed();
            //qDebug() << "Numero de Frames: " << frame_sequence_hash.size();*/

            return 0;
        }

        int Video::LoadFromSeq(std::vector<Hash::AbstractHashPtr> &seq,
                                    int width, int height, float fps)
        {
            /*frame_width = width;
            frame_height = height;
            mFPS = fps;*/

            for (size_t i = 0; i < seq.size(); i++)
                frame_sequence_hash.push_back(seq[i]);

            return 0;
        }

        int Save(char* filename){

        }

        int Video::LoadFromImages(std::vector<Hash::AbstractHashPtr>& _seq, int _width, int _height,
                                  AVCodecID _codec, AVPixelFormat _src_pixelfmt,
                                  AVPixelFormat _dst_pixelfmt, int _bitrate,
                                  AVRational fps, int gop_size, int max_bframes, AVFormatContext *_fmtctx){


            int kfps = 0;
            std::vector<ImagePtr> _sequence;
            AVOutputFormat *avOutputFormat = NULL;
            AVStream *m_video_st = NULL;
            AVCodecContext *c = NULL;
            AVFrame *dstFrame;

            int ret = 0;
            char *filename = "/Users/josericardo/tmp/outtestmov2.avi";

            for (size_t i = 0; i < _seq.size(); i++)
                _sequence.push_back(ImageRegister::ImageAt(_seq[i]));

            //fmt_ctx = (AVFormatContext*) malloc(sizeof(AVFormatContext));

            //memccpy(fmt_ctx, _fmtctx, 1, sizeof(AVFormatContext));
            //fmt_ctx = _fmtctx;

            //AVOutputFormat *avOutputFormat2 = av_guess_format(NULL, filename, NULL);


            AVCodec *encoder = avcodec_find_encoder(_codec);//AV_CODEC_ID_H264);
            if (encoder != NULL){
                const AVCodecDescriptor *codecDesc = avcodec_descriptor_get(_codec);
                if (avformat_alloc_output_context2(&fmt_ctx, NULL, codecDesc->name, NULL) < 0){
                    fprintf(stderr, "avformat_alloc_output_context2\n");
                    exit(1);
                }
            } else {
                fprintf(stderr, "avcodec_find_encoder\n");
                exit(1);
            }


            while (avOutputFormat = av_oformat_next(avOutputFormat)) {
                if (strcmp(avOutputFormat->name, _fmtctx->iformat->name) == 0)
                    break;
              }

            //avOutputFormat->video_codec = _codec;
            //fmt_ctx = avformat_alloc_context();
            fmt_ctx->oformat = avOutputFormat;



            // Add an output stream.
            //m_video_st = add_video_stream(_codec, _width, _height, _dst_pixelfmt, _bitrate);
            m_video_st = avformat_new_stream(fmt_ctx, encoder);

            if (m_video_st == NULL){
                fprintf(stderr, "Error creating new stream!\n");
                exit(1);
            }
            if (m_video_st->codec == NULL) {
                fprintf(stderr, "AVStream codec is NULL\n");
                exit(1);
            }
            snprintf(fmt_ctx->filename, sizeof(fmt_ctx->filename), "%s", filename);

            //AVCodec* encoder = avcodec_find_encoder(avOutputFormat->video_codec);
            //if (!encoder) {
             //   fprintf(stderr, "Codec not found\n");
               // exit(1);
            //}


            c = m_video_st->codec;
            c->bit_rate = _bitrate; //400000;
            c->width = _width;
            c->height = _height;
            c->time_base.den = fps.den;
            c->time_base.num = fps.num;
            c->gop_size = gop_size;
            c->max_b_frames = max_bframes;
            c->pix_fmt = _dst_pixelfmt;//PIX_FMT_YUV420P;

            if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                c->flags |= CODEC_FLAG_GLOBAL_HEADER;


            if (_codec == AV_CODEC_ID_H264) {
                //av_opt_set(c->priv_data, "preset", "superfast", 0);
                //av_opt_set(c->priv_data, "tune", "zerolatency", 0);
            }

            if (avcodec_open2(c, encoder, NULL) < 0){
                fprintf(stderr, "Could not open codec.\n");
                return NULL;
            }

            // Alloc Frame
            //frame = alloc_picture(c);



            dstFrame = avcodec_alloc_frame();
            if (!dstFrame ) {
                fprintf(stderr, "Could not allocate video frame\n");
                exit(1);
            }
            dstFrame ->format = c->pix_fmt;
            dstFrame->width  = c->width;
            dstFrame->height = c->height;


            /* the image can be allocated by any means and av_image_alloc() is
                 * just the most convenient way if av_malloc() is to be used */
                ret = av_image_alloc(dstFrame->data, dstFrame->linesize, c->width, c->height,
                                     c->pix_fmt, 32);
                if (ret < 0) {
                    fprintf(stderr, "Could not allocate raw picture buffer\n");
                    exit(1);
                }

                // some formats want stream headers to be separate
                if(fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                    c->flags |= CODEC_FLAG_GLOBAL_HEADER;

                        /* open the output file, if needed */
                if (!(avOutputFormat->flags & AVFMT_NOFILE)) {
                    if (avio_open(&fmt_ctx->pb, filename, AVIO_FLAG_WRITE) < 0) {
                        fprintf(stderr, "Could not open '%s'\n", filename);
                        exit(1);
                    }
                }
            ret = avformat_write_header(fmt_ctx, NULL);

            //Allocate RGB frame that we can pass to the YUV frame
            AVFrame* m_src_frame;
            m_src_frame = avcodec_alloc_frame();
            m_src_frame ->format = _src_pixelfmt;
            m_src_frame->width  = _width;
            m_src_frame->height = _height;
            int size = avpicture_get_size(_src_pixelfmt, _width, _height);
            uint8_t *pPicBuf = (uint8_t*) malloc(size);
            ret = avpicture_fill((AVPicture*) m_src_frame, pPicBuf, _src_pixelfmt, _width, _height);

            if (ret < 0) {
                fprintf(stderr, "Could not allocate raw picture buffer\n");
                exit(1);
            }


            //Tell FFMPEG that we are going to write encoded frames to a file
           // av_dump_format(fmt_ctx, 0, filename, 1);


            struct SwsContext *sws_ctx;
            sws_ctx = sws_getContext(_width, _height, _src_pixelfmt,
                                     c->width, c->height, _dst_pixelfmt,
                                     SWS_POINT, NULL, NULL, NULL);

            if (!sws_ctx) {
                fprintf(stderr, "sws_getContext\n");
                exit(1);
            }

            AVPacket pkt;
            int got_packet;
            for (int k = 0; k < _sequence.size(); k++){

                ImagePtr img = _sequence[k];
                //cv::Mat frameRGBA;
                //cv::cvtColor(img->getOpenCVMat(), frameRGBA, cv::COLOR_RGB2RGBA);
                av_init_packet(&pkt);
                pkt.data = NULL;    // packet data will be allocated by the encoder
                pkt.size = 0;
                dstFrame->pts = kfps;

                int w = img->getWidth();

                const unsigned char* buffer = (const unsigned char*) img->getOpenCVMat().data;
                if (avpicture_fill((AVPicture*) m_src_frame, &buffer[0], _src_pixelfmt,
                                   img->getWidth(), img->getHeight()) < 0){
                    fprintf(stderr, "avpicture_fill\n");
                    exit(1);
                }


                //Convert RGB frame (m_src_picture) to and YUV frame (m_dst_picture)
                ret = sws_scale(sws_ctx,
                          m_src_frame->data, m_src_frame->linesize,
                          0, img->getWidth(), dstFrame->data, dstFrame->linesize);


                //Encode the frame
                if (avcodec_encode_video2(c, &pkt, dstFrame, &got_packet) < 0){
                    fprintf(stderr, "Error encoding frame\n");
                    exit(1);
                }

                if (got_packet)
                {
                    if (pkt.pts != AV_NOPTS_VALUE) pkt.pts = av_rescale_q(pkt.pts, m_video_st->codec->time_base, fmt_ctx->streams[0]->time_base);
                    if (pkt.dts != AV_NOPTS_VALUE) pkt.dts = av_rescale_q(pkt.dts, m_video_st->codec->time_base, fmt_ctx->streams[0]->time_base);

                     m_video_st->codec->coded_frame->pts = kfps;

                    if(m_video_st->codec->coded_frame->key_frame)
                    {
                        pkt.flags |= AV_PKT_FLAG_KEY;
                    }

                    pkt.stream_index = m_video_st->index;

                    std::cout << "pts: " << pkt.pts << ", dts: "  << pkt.dts << std::endl;

                    av_write_frame(fmt_ctx, &pkt);
                    av_free_packet(&pkt);

                } else {
                           printf("got_output false: %d", kfps);
                       }

                kfps++;


            }


            //Write trailing bits
            av_write_trailer(fmt_ctx);


            //Close Video codec
            avcodec_close(c);
            av_free(c);
            //free(&dstFrame->data[0]);
            av_free(dstFrame);

            if (!(avOutputFormat->flags & AVFMT_NOFILE)) {
                /* close the output file */
                avio_close(fmt_ctx->pb);
            }

           sws_freeContext(sws_ctx);

            return 0;
        }

        void Video::RetrieveHashTable(AVFormatContext *avfmtctx, AVCodecContext *avcdctx)
        {

            AVFrame *pFrame = NULL;
            AVFrame *pFrameRGB = NULL;

            // Alocar video frame
            pFrame = avcodec_alloc_frame();
            pFrameRGB = avcodec_alloc_frame();

            if (pFrameRGB == NULL)
                return;

            uint8_t *buffer;
            int numBytes;

            numBytes = avpicture_get_size(PIX_FMT_RGB32, avcdctx->width,
                                          avcdctx->height);
            buffer = (uint8_t*) av_malloc(numBytes * sizeof(uint8_t));
            uint8_t* rgbx =  (uint8_t*) malloc(sizeof(uint8_t) *numBytes);

            //sprintf((char *) buffer, "P6\n%d %d\n255\n", avcdctx->width, avcdctx->height);
            struct SwsContext *sws_ctx;
            sws_ctx = sws_getContext(
                        avcdctx->width,
                        avcdctx->height,
                        avcdctx->pix_fmt,
                        avcdctx->width,
                        avcdctx->height,
                        PIX_FMT_RGB32,
                        SWS_POINT,
                        NULL,
                        NULL,
                        NULL);

            avpicture_fill((AVPicture*)pFrameRGB, &buffer[0], PIX_FMT_RGB32,
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



                        Hash::AbstractHashPtr _hash = ImageRegister::RegisterFrame(
                            (uchar*)buffer, avcdctx->width, avcdctx->height, 4);
                        frame_sequence_hash.push_back(_hash);
                    }
                }

                // liberar package
                av_free_packet(&packet);
            }
            av_free(buffer);
            av_free(pFrameRGB);
            av_free(pFrame);
            free(rgbx);
            sws_freeContext(sws_ctx);
            sws_ctx = NULL;
        }

        AVStream* Video::add_video_stream(AVCodecID codec_id, int _width, int _height,
                                          AVPixelFormat _pixel_format, int _bitrate){
            AVCodecContext *c;
            AVStream *st;
            AVCodec *codec;

            codec = avcodec_find_encoder(codec_id);
            if (!codec) {
                fprintf(stderr, "Codec not found.\n");
                return NULL;
            }

            st = avformat_new_stream(fmt_ctx, codec);

            if (!st) return NULL;

            c = st->codec;
            c->bit_rate = _bitrate;
            c->width = _width;
            c->height = _height;
            c->time_base.den = 25;
            c->time_base.num = 1;
            c->gop_size = 12;
            c->max_b_frames = 1;
            c->gop_size = 30;
            c->pix_fmt = _pixel_format;//PIX_FMT_YUV420P;

            if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
                c->flags |= CODEC_FLAG_GLOBAL_HEADER;


            if (codec_id == AV_CODEC_ID_H264) {
                //av_opt_set(c->priv_data, "preset", "superfast", 0);
                //av_opt_set(c->priv_data, "tune", "zerolatency", 0);
            }

            if (avcodec_open2(c, codec, NULL) < 0){
                fprintf(stderr, "Could not open codec.\n");
                return NULL;
            }

            // Alloc Frame
            frame = alloc_picture(c);

            return st;
        }


        int Video::write_video_frame(ImagePtr img, AVPixelFormat _src_pixelfmt, AVStream *st,
                                      SwsContext *sws_ctx, AVPicture *_src, AVFrame *_dst, int frame_number){

            AVCodecContext *capc = st->codec, *c = st->codec;
            const unsigned char *buffer = img->data();

            int numBytes = avpicture_get_size(_src_pixelfmt, img->getWidth(), img->getHeight());
            //buffer = (const unsigned char*) av_malloc(numBytes * sizeof(unsigned char));
            avpicture_fill(_src, &buffer[0], _src_pixelfmt,
                               img->getWidth(), img->getHeight());


            //Convert RGB frame (m_src_picture) to and YUV frame (m_dst_picture)
            sws_scale(sws_ctx,
                      _src->data, _src->linesize,
                      0, img->getWidth(), _dst->data, _dst->linesize);
            _dst->pts = frame_number;
            int pts = frame_number;

            //Some inits for encoding the frame
            AVPacket pkt = { 0 };
            int got_packet;
            av_init_packet(&pkt);
            pkt.data = NULL;
            pkt.size = 0;
            _dst->pts = frame_number;
            fflush(stdout);


            //Encode the frame
            if (avcodec_encode_video2(c, &pkt, _dst, &got_packet) < 0) return -1;

            if (got_packet != 0)
            {
                av_write_frame(fmt_ctx, &pkt);
                av_free_packet(&pkt);

              /*  st->codec->coded_frame->pts = frame_number;

                    if (st->codec->coded_frame->pts != (0x8000000000000000LL))
                    {
                        pts = av_rescale_q(st->codec->coded_frame->pts, st->codec->time_base, fmt_ctx->streams[0]->time_base);
                    }
                    pkt.pts = pts;
                    if(st->codec->coded_frame->key_frame)
                    {
                       pkt.flags |= AV_PKT_FLAG_KEY;
                    }

                    std::cout << "pts: " << pkt.pts << ", dts: "  << pkt.dts << std::endl;

                    av_interleaved_write_frame(fmt_ctx, &pkt);
                    av_free_packet(&pkt);*/
                }

        }

        AVFrame *Video::alloc_picture(AVCodecContext *c)
        {
            AVFrame *pic;
            /*AVPicture dst_pic;

            avpicture_alloc(&dst_pic, c->pix_fmt, c->width, c->height);
            *((AVPicture *)pic) = dst_pic;*/

            int size;
            uint8_t *pbuf;
            pic = avcodec_alloc_frame();
            if (!pic) return NULL;
            size = avpicture_get_size(c->pix_fmt, c->width, c->height);
            pbuf = (uint8_t*) av_malloc(size);
            if (!pbuf) return NULL;
            avpicture_fill((AVPicture*)pic, pbuf, c->pix_fmt, c->width, c->height);
            return pic;
        }
    }



}
