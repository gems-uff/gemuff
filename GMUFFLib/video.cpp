#include "video.h"


namespace GEMUFF
{
    namespace VIMUFF
    {
    	Video::Video(Hash::HashType hashType){
    		this->hashType = hashType;
    		this->fmt_ctx = NULL;
    		frame = NULL;
    		videostream = -1;
    		video_ctx = NULL;
    		mFPS = -1;
    	}


        Video::~Video(){
            if (video_ctx != NULL){
                avio_close(fmt_ctx->pb);
            	avcodec_close(video_ctx);
            }

            /*if (fmt_ctx != NULL) {
            	avformat_close_input(&fmt_ctx);
            	avformat_free_context(fmt_ctx);
            }*/

            video_ctx = NULL;
            fmt_ctx = NULL;
        }

        void Video::Debug(std::string folder){


            for (int i = 0; i < frame_sequence_hash.size(); i++ ){
                std::stringstream ss;
                ss << folder << "/" << i << ".png";

                std::string filename("Img");
                filename = filename.append(std::to_string(i)).append(".png");

                ImageRegister::ImageAt(frame_sequence_hash[i])->save(folder + "/" + filename);
            }
        }



        Video* Video::loadVideo(std::string filename, Hash::HashType hashType)
        {
        	Video* _v = new Video(hashType);

#ifdef VIMUFF_INFO
            printf("Opening file %s\n", filename.c_str());
#endif
            // Look for file header in order to detect the type of video
            if (avformat_open_input(&_v->fmt_ctx, filename.c_str(), NULL, NULL) != 0){
                av_log(NULL, AV_LOG_ERROR, "Error opening file");
                return  NULL;
            }

            // Find information about the stream in the file
            if (avformat_find_stream_info(_v->fmt_ctx, NULL) < 0){
                av_log(NULL, AV_LOG_ERROR, "No video stream!");
                return NULL; // Sem informacao
            }

#ifdef VIMUFF_INFO
            // Print information regarding the video format
            av_dump_format(v->fmt_ctx, 0, filename.c_str(), 0);
#endif


            // Get the video stream and the codec used by it
            AVCodec *pDec = NULL;
            _v->videostream = av_find_best_stream(_v->fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &pDec, 0);

            if (_v->videostream < 0){
                av_log(NULL, AV_LOG_ERROR, "No video stream available!");
                return NULL;
            }


            // Create a context to be used
            _v->video_ctx = avcodec_alloc_context3(pDec);

            if (!_v->video_ctx){
                //return AVERROR(ENOMEM);
            	return NULL;
            }

            // Fill the codec context
            avcodec_parameters_to_context(_v->video_ctx, _v->fmt_ctx->streams[_v->videostream]->codecpar);

            // Initialzie the codec to be used
            if (avcodec_open2(_v->video_ctx, pDec, NULL) < 0){
                av_log(NULL, AV_LOG_ERROR, "Cannot open the video codec!");
                return NULL;
            }

            // Recuperar a tabela de hash de cada filme
#ifdef VIMUFF_INFO
            GEMUFF::Util::Clock timer;
            timer.reset();
#endif
            _v->RetrieveHashTable();

#ifdef VIMUFF_INFO
            printf("Hash Calculation time(ms): %f\n", timer.getMilliseconds());
            printf("Total Frames: %d\n", frame_sequence_hash.size());

            FILE *f;
            f = fopen("/home/josericardo/video.txt", "w");

            for (int i = 0; i < frame_sequence_hash.size(); i++){
                fprintf(f, "%s\n", frame_sequence_hash[i]->toString().c_str());
            }

            fclose(f);
#endif


            return _v;
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

        int Video::Save(const char* filename){
        	boost::filesystem::copy_file(
        			boost::filesystem::path(fmt_ctx->url),
        			boost::filesystem::path(filename), boost::filesystem::copy_option::overwrite_if_exists);

        	return 0;
        }

        Video* Video::loadFromImages(Hash::HashType hashType, std::vector<Hash::AbstractHashPtr> _seq, int _width, int _height,
                                  AVCodecID _codec, AVPixelFormat _src_pixelfmt,
                                  AVPixelFormat _dst_pixelfmt, int _bitrate,
                                  AVRational fps, int gop_size, int max_bframes, std::string file_format){

        	Video *res = new Video(hashType);
        	res->frame_sequence_hash = _seq;

        	// Create a temporary file
        	boost::filesystem::path temp = boost::filesystem::temp_directory_path() /
        			boost::filesystem::unique_path();

        	const char *filename = temp.c_str();

            std::vector<ImagePtr> _sequence;
            AVOutputFormat *avOutputFormat = NULL;
            AVStream *m_video_st = NULL;

            int ret = 0;
            int kfps = 0;


            for (size_t i = 0; i < _seq.size(); i++)
                _sequence.push_back(ImageRegister::ImageAt(_seq[i]));


            res->fmt_ctx = avformat_alloc_context();
            if (res->fmt_ctx == NULL){
            	fprintf(stderr, "avformat_alloc_context Error\n");
            	exit(-1);
            }


            avOutputFormat = av_guess_format(file_format.c_str(), NULL, NULL);
            if (avOutputFormat == NULL) {
                fprintf(stderr, "Could not guess output format\n");
                exit(-1);
              }


            AVCodec *encoder = avcodec_find_encoder(_codec);//AV_CODEC_ID_H264);
            if (encoder == NULL){
            	fprintf(stderr, "avcodec_find_encoder Error\n");
            	exit(-1);
            }

            res->video_ctx = avcodec_alloc_context3(encoder);
            res->video_ctx->codec_id = avOutputFormat->video_codec;
            res->video_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
            res->video_ctx->gop_size = gop_size;
            res->video_ctx->bit_rate = _bitrate;
            res->video_ctx->width = _width;
            res->video_ctx->height = _height;
            res->video_ctx->time_base = (AVRational){1, fps.num};
            res->video_ctx->max_b_frames = max_bframes;
            res->video_ctx->pix_fmt = _dst_pixelfmt;//PIX_FMT_YUV420P;


            res->fmt_ctx->oformat = avOutputFormat;
            res->fmt_ctx->video_codec_id = avOutputFormat->video_codec;

            snprintf(res->fmt_ctx->filename, sizeof(res->fmt_ctx->filename), "%s", filename);


            // Add an output stream.
            //m_video_st = add_video_stream(_codec, _width, _height, _dst_pixelfmt, _bitrate);
            m_video_st = avformat_new_stream(res->fmt_ctx, encoder);

            if (m_video_st == NULL){
            	fprintf(stderr, "avformat_new_stream() Error\n");
            	exit(-1);
            }


            m_video_st->codec = res->video_ctx;

            if (res->fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            	res->video_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;


            if (avcodec_open2(res->video_ctx, encoder, NULL) < 0){
                fprintf(stderr, "Could not open codec.\n");
                return NULL;
            }

            avio_open2(&res->fmt_ctx->pb, filename, AVIO_FLAG_WRITE, NULL, NULL);
            avformat_write_header(res->fmt_ctx, NULL);

            struct SwsContext *sws_ctx;
            sws_ctx = sws_getContext(_width, _height, _src_pixelfmt,
            		res->video_ctx->width, res->video_ctx->height, _dst_pixelfmt,
                                     SWS_POINT, NULL, NULL, NULL);

            if (!sws_ctx) {
                fprintf(stderr, "sws_getContext\n");
                exit(1);
            }

            res->write_video_frame(_sequence, m_video_st, sws_ctx);


            //Write trailing bits
            av_write_trailer(res->fmt_ctx);


            //if (!(avOutputFormat->flags & AVFMT_NOFILE)) {
                /* close the output file */

            //}
           avcodec_close(m_video_st->codec);
           sws_freeContext(sws_ctx);

            return res;
        }


        int Video::LoadFromImages(std::vector<ImagePtr> _sequence, int _width, int _height,
        		AVCodecID _codec, AVPixelFormat _src_pixelfmt,
				AVPixelFormat _dst_pixelfmt, int _bitrate,
				AVRational fps, int gop_size, int max_bframes, char* format ){


        	/*// Create a temporary file
        	boost::filesystem::path temp = boost::filesystem::temp_directory_path() /
        			boost::filesystem::unique_path();

        	const char *filename = temp.c_str();

        	AVOutputFormat *avOutputFormat = NULL;
        	AVStream *m_video_st = NULL;

        	int ret = 0;
        	int kfps = 0;



        	fmt_ctx = avformat_alloc_context();
        	if (fmt_ctx == NULL){
        		fprintf(stderr, "avformat_alloc_context Error\n");
        		exit(-1);
        	}


        	avOutputFormat = av_guess_format(format, NULL, NULL);
        	if (avOutputFormat == NULL) {
        		fprintf(stderr, "Could not guess output format\n");
        		exit(-1);
        	}


        	AVCodec *encoder = avcodec_find_encoder(_codec);//AV_CODEC_ID_H264);
        	//AVCodec *encoder = avcodec_find_encoder_by_name("libx264rgb");//AV_CODEC_ID_H264);
        	if (encoder == NULL){
        		fprintf(stderr, "avcodec_find_encoder Error\n");
        		exit(-1);
        	}

        	codec_context = avcodec_alloc_context3(encoder);
        	codec_context->bit_rate = _bitrate;
        	codec_context->width = _width;
        	codec_context->height = _height;
        	codec_context->time_base = fps;

        	codec_context->codec_id = avOutputFormat->video_codec;
        	codec_context->codec_type = AVMEDIA_TYPE_VIDEO;
        	codec_context->gop_size = gop_size;


        	codec_context->max_b_frames = max_bframes;
        	codec_context->pix_fmt = _dst_pixelfmt;//PIX_FMT_YUV420P;


        	fmt_ctx->oformat = avOutputFormat;
        	fmt_ctx->video_codec_id = avOutputFormat->video_codec;

        	snprintf(fmt_ctx->filename, sizeof(fmt_ctx->filename), "%s", filename);


        	// Add an output stream.
        	//m_video_st = add_video_stream(_codec, _width, _height, _dst_pixelfmt, _bitrate);
        	m_video_st = avformat_new_stream(fmt_ctx, encoder);

        	if (m_video_st == NULL){
        		fprintf(stderr, "avformat_new_stream() Error\n");
        		exit(-1);
        	}


        	m_video_st->codec->pix_fmt = _dst_pixelfmt;
        	m_video_st->codec->width = _width;
        	m_video_st->codec->height = _height;
        	m_video_st->codecpar->width = _width;
        	m_video_st->codecpar->height = _height;
        	m_video_st->codecpar->format = _dst_pixelfmt;
        	m_video_st->time_base = fps;
        	fmt_ctx->video_codec = encoder;


        	if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        		codec_context->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;


        	if (avcodec_open2(codec_context, encoder, NULL) < 0){
        		fprintf(stderr, "Could not open codec.\n");
        		return NULL;
        	}

        	avio_open2(&fmt_ctx->pb, filename, AVIO_FLAG_WRITE, NULL, NULL);
        	avformat_write_header(fmt_ctx, NULL);

        	struct SwsContext *sws_ctx;
        	sws_ctx = sws_getContext(_width, _height, _src_pixelfmt,
        			codec_context->width, codec_context->height, _dst_pixelfmt,
					SWS_POINT, NULL, NULL, NULL);

        	if (!sws_ctx) {
        		fprintf(stderr, "sws_getContext\n");
        		exit(1);
        	}*/

        	// Create a temporary file
        	        	boost::filesystem::path temp = boost::filesystem::temp_directory_path() /
        	        			boost::filesystem::unique_path();

        	        	const char *filename = temp.c_str();

        	            AVOutputFormat *avOutputFormat = NULL;
        	            AVStream *m_video_st = NULL;

        	            int ret = 0;
        	            int kfps = 0;


        	            fmt_ctx = avformat_alloc_context();
        	            if (fmt_ctx == NULL){
        	            	fprintf(stderr, "avformat_alloc_context Error\n");
        	            	exit(-1);
        	            }


        	            avOutputFormat = av_guess_format(format, NULL, NULL);
        	            if (avOutputFormat == NULL) {
        	                fprintf(stderr, "Could not guess output format\n");
        	                exit(-1);
        	              }
        	            //av_dict_set(&param, "crf", "0", 0);

        	            AVCodec *encoder =  avcodec_find_encoder_by_name("libx264rgb");//avcodec_find_encoder(_codec);//AV_CODEC_ID_H264);
        	            if (encoder == NULL){
        	            	fprintf(stderr, "avcodec_find_encoder Error\n");
        	            	exit(-1);
        	            }

        	            video_ctx = avcodec_alloc_context3(encoder);
        	            video_ctx->codec_id = avOutputFormat->video_codec;
        	            video_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
        	            video_ctx->gop_size = gop_size;
        	            video_ctx->bit_rate = _bitrate;
        	            video_ctx->width = _width;
        	            video_ctx->height = _height;
        	            video_ctx->time_base = fps;
        	            video_ctx->max_b_frames = max_bframes;
        	            video_ctx->pix_fmt = _dst_pixelfmt;//PIX_FMT_YUV420P;
        	            av_opt_set(video_ctx->priv_data, "preset", "ultrafast", 0);
        	            av_opt_set(video_ctx->priv_data, "crf", 0, 0);//lossless


        	            fmt_ctx->oformat = avOutputFormat;
        	            fmt_ctx->video_codec_id = avOutputFormat->video_codec;

        	            snprintf(fmt_ctx->filename, sizeof(fmt_ctx->filename), "%s", filename);


        	            // Add an output stream.
        	            //m_video_st = add_video_stream(_codec, _width, _height, _dst_pixelfmt, _bitrate);
        	            m_video_st = avformat_new_stream(fmt_ctx, encoder);

        	            if (m_video_st == NULL){
        	            	fprintf(stderr, "avformat_new_stream() Error\n");
        	            	exit(-1);
        	            }


        	            m_video_st->codec = video_ctx;

        	            if (fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
        	            	video_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;


        	            if (avcodec_open2(video_ctx, encoder, NULL) < 0){
        	                fprintf(stderr, "Could not open codec.\n");
        	                return NULL;
        	            }

        	            avio_open2(&fmt_ctx->pb, filename, AVIO_FLAG_WRITE, NULL, NULL);
        	            avformat_write_header(fmt_ctx, NULL);

        	            struct SwsContext *sws_ctx;
        	            sws_ctx = sws_getContext(_width, _height, _src_pixelfmt,
        	            		video_ctx->width, video_ctx->height, _dst_pixelfmt,
        	                                     SWS_POINT, NULL, NULL, NULL);

        	            if (!sws_ctx) {
        	                fprintf(stderr, "sws_getContext\n");
        	                exit(1);
        	            }

        	write_video_frame(_sequence, m_video_st, sws_ctx);


        	//Write trailing bits
        	av_write_trailer(fmt_ctx);


        	//if (!(avOutputFormat->flags & AVFMT_NOFILE)) {
        	/* close the output file */

        	//}
        	avcodec_close(m_video_st->codec);
        	sws_freeContext(sws_ctx);

        	return 0;
        }

        int Video::RetrieveHashTable()
        {
        	AVFormatContext *avfmtctx = this->fmt_ctx;
        	AVCodecContext *avcdctx = this->video_ctx;

            AVFrame *pFrame = NULL;
            AVFrame *pFrameRGB = NULL;
            AVPacket *packet = (AVPacket*) av_malloc(sizeof(AVPacket));

            // Alloc buffer to a frame
            pFrame = av_frame_alloc();
            pFrameRGB = av_frame_alloc();

            if (pFrame == NULL || pFrameRGB == NULL) {
            	fprintf(stderr, "Cannot allocate AVFrame Buffer!\n", 0);
            	exit(-1);
            }


            // We need a buffer to place the frame we read from a video
            int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB32, avcdctx->width,
                                                    avcdctx->height, 1);
            uint8_t *buffer = (uint8_t*) av_malloc(numBytes * sizeof(uint8_t));

            // Assign appropriate parts of buffer to image planes in pFrameRGB
            // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
            // of AVPicture
            av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize,
                buffer, AV_PIX_FMT_RGB32,
                avcdctx->width, avcdctx->height, 1);


            av_init_packet(packet);


            struct SwsContext *sws_ctx;
            sws_ctx = sws_getContext(
                        avcdctx->width,
                        avcdctx->height,
                        avcdctx->pix_fmt,
                        avcdctx->width,
                        avcdctx->height,
                        AV_PIX_FMT_RGB32,
                        SWS_POINT,
                        NULL,
                        NULL,
                        NULL);

            // read data
            int ret = -1;


            while (1)
            {
                ret = av_read_frame(avfmtctx, packet);

                if (ret < 0) break;


                if (packet->stream_index == videostream)
                {
                    ret = avcodec_send_packet(avcdctx, packet);

                    if (ret < 0){
                        av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder!\n");
                        break;
                    }

                    while (ret >= 0){
                        ret = avcodec_receive_frame(avcdctx, pFrame);

                        if (ret >= 0){
                            sws_scale(sws_ctx,
                                  (uint8_t const * const*)pFrame->data,
                                  pFrame->linesize,
                                  0,
                                  avcdctx->height,
                                  pFrameRGB->data,
                                  pFrameRGB->linesize);

                                Hash::AbstractHashPtr _hash = ImageRegister::RegisterFrame(
                                    avcdctx->width, avcdctx->height, CV_8UC4, hashType, (uchar*)buffer);

                                frame_sequence_hash.push_back(_hash);
                        } else if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
                            break;
                        } else {
                            av_log(NULL, AV_LOG_ERROR, "Error while receiving a frame from the decoder.");
                            av_packet_free(&packet);
                            sws_freeContext(sws_ctx);
                            //avformat_close_input(&fmt_ctx);
                            av_free(pFrameRGB);
                            av_free(pFrame);
                            av_free(buffer);
                            return 0;
                        }
                    }
                }

                // liberar package
                av_packet_unref(packet);
            }
            av_packet_free(&packet);
            sws_freeContext(sws_ctx);
            //avformat_close_input(&fmt_ctx);
            av_free(pFrameRGB);
            av_free(pFrame);
            av_free(buffer);
            sws_ctx = NULL;

            return 0;
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
                c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;


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


        int Video::write_video_frame(std::vector<ImagePtr> imgSeq, AVStream* avstream, SwsContext *sws_ctx){

        	AVFrame* frame = av_frame_alloc();

        	int numBytes = avpicture_get_size(video_ctx->pix_fmt, video_ctx->width, video_ctx->height);
        	uchar *pic_buffer = (uchar*) av_malloc(numBytes);

            av_image_fill_arrays(frame->data, frame->linesize,
                pic_buffer, video_ctx->pix_fmt, video_ctx->width, video_ctx->height, 1);

            frame->width = video_ctx->width;
            frame->height = video_ctx->height;
            frame->format = video_ctx->pix_fmt;


            int64_t frame_pts = 0;
            uchar nb_frames = 0;
            bool end_of_stream = false;
            int got_pkt = 0;

            for (int i = 0; i < imgSeq.size(); i++){
            	VIMUFF::ImagePtr _img = imgSeq[i];

            	cv::Mat m = _img->getOpenCVMat();

            	static bool sav = true;

            	if (sav){
            		cv::imwrite("/home/josericardo/teste.jpg", m);
            		sav = false;
            	}


            	//Convert RGB frame (m_src_picture) to and YUV frame (m_dst_picture)
            	const int stride[] = {static_cast<int>(m.step[0])};
            	sws_scale(sws_ctx,
            			&m.data, stride, 0, m.rows, frame->data, frame->linesize);
            	frame->pts = frame_pts++;

				//Some inits for encoding the frame
				AVPacket pkt;
				pkt.data = NULL;
				pkt.size = 0;
				av_init_packet(&pkt);

	            //Encode the frame
	            if (avcodec_encode_video2(video_ctx, &pkt, frame, &got_pkt) < 0){
	            	fprintf(stderr, "avcodec_encode_video2() error!\n");
	            	exit(-1);
	            }


				if (got_pkt){

					pkt.duration = 1;
					av_packet_rescale_ts(&pkt, avstream->codec->time_base , avstream->time_base);
					av_write_frame(fmt_ctx, &pkt);
					++nb_frames;
				}


				av_packet_unref(&pkt);
            }

            av_frame_free(&frame);
            free(pic_buffer);

            return 0;
        }

        AVFrame *Video::alloc_picture(AVCodecContext *c)
        {
            AVFrame *pic;
            /*AVPicture dst_pic;

            avpicture_alloc(&dst_pic, c->pix_fmt, c->width, c->height);
            *((AVPicture *)pic) = dst_pic;*/

            int size;
            uint8_t *pbuf;
            pic = av_frame_alloc();
            if (!pic) return NULL;
            size = avpicture_get_size(c->pix_fmt, c->width, c->height);
            pbuf = (uint8_t*) av_malloc(size);
            if (!pbuf) return NULL;
            avpicture_fill((AVPicture*)pic, pbuf, c->pix_fmt, c->width, c->height);
            return pic;
        }

        void Video::PrintHashSequence(std::string file){
        	std::ofstream f;
        	f.open(file.c_str());

        	if (f.good()){
        		for (int i = 0; i < frame_sequence_hash.size(); i++){
        			f << frame_sequence_hash[i]->toString() << std::endl;
        		}
        	}

        	f.close();
        }

        void Video::toImageSequence(std::string directory){
        	int _listSize = frame_sequence_hash.size();
        	std::ofstream hashFile;

        	hashFile.open((directory + "/Output.txt").c_str(),
        			std::ios::out | std::ios::app);

        	for (int i = 0; i < _listSize; i++){
        		std::string filename("Img");
        		filename = filename.append(std::to_string(i)).append(".jpg");

        		ImagePtr _img = ImageRegister::ImageAt(frame_sequence_hash[i]);

        		hashFile << frame_sequence_hash[i]->toString() << std::endl;
        		_img->save(directory + "/" + filename);
        	}

        	hashFile.close();

        }
    }



}
