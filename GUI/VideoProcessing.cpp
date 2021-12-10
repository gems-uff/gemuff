#include "VideoProcessing.h"




/*void debug_frames(std::vector<GEMUFF::VIMUFF::Frame_Diff> &frames)
{
    for (int i = 0; i < frames.size(); i++)
    {
        printf("v1: %s\n", frames[i].v1_frame.c_str());
        printf("v2: %s\n", frames[i].v2_frame.c_str());

        switch (frames[i].op)
        {
        case GEMUFF::VIMUFF::ADD:
            printf("op: ADD\n");
            break;

        case GEMUFF::VIMUFF::NONE:
            printf("op: NONE\n");
            break;

        case GEMUFF::VIMUFF::XOR:
            printf("op: XOR\n");
            break;
        }
    }
}*/

/*
  LCS
 */



namespace GEMUFF
{
    namespace VIMUFF
    {



        void PatchProcessing::Save(std::string filename)
        {
            // Carregar o arquivo de diferencas
           /* FILE *fp;
            fp = fopen(delta.c_str(), "rb");

            if (!fp)
                exit(1);

            AVCodec *codec;
            AVCodecContext *c= NULL;
            int i, out_size, size, x, y, outbuf_size;
            FILE *f;
            AVFrame *pFrameRGB = NULL;
            AVFrame *pFrameYUV = NULL;
            uint8_t *outbuf, *picture_buf;

            struct SwsContext *sws_ctx = NULL;*/



             /* find the mpeg1 video encoder */
             /*codec = avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);
             if (!codec) {
                 fprintf(stderr, "codec not found\n");
                 exit(1);
             }


             c= avcodec_alloc_context();


             // Alocar video frame
             pFrameYUV = avcodec_alloc_frame();
             pFrameRGB = avcodec_alloc_frame();

             if (pFrameYUV == NULL)
                 return;*/

        }



        Video PatchProcessing::Patch(Video *video, Diff2Processing &delta)
        {
          /*  std::vector<std::string> &_seq_hash = video->getSequenceHash();
            std::vector<QImage> patchedSequence;*/
            Video result;

            int _frame_size = sizeof(uchar) * 4 * delta.getFrameWidth() * delta.getFrameHeight();

            int _video_current_index = 0;

           /* while (_video_current_index < _seq_hash.size())
            {
                // Verificar se existe alguma operacao nesse frame
                Frame_Diff *_diff_at_frame = delta.DiffAtFrame(_video_current_index);

                if (_diff_at_frame == NULL)
                {
                    patchedSequence.push_back(*ImageRegister::ImageAt(_seq_hash[_video_current_index]));
                    _video_current_index++;
                }
                else
                {
                    for (int k = 0; k < _diff_at_frame->numFrames; k++)
                    {
                        // Verificar o tipo de operacao
                        if (_diff_at_frame->op == XOR)
                        {
                            QImage* _img = ImageRegister::ImageAt(_seq_hash[_video_current_index++]);
                            uchar* _xor_data = (uchar*) malloc(_frame_size);
                              gIMUFFPatch(_img->constBits(), (const uchar*) &_diff_at_frame->buffer[k*_frame_size],
                                        _xor_data, delta.getFrameWidth() * delta.getFrameHeight());

                            QImage _xor_image = QImage(_xor_data, delta.getFrameWidth(),
                                                       delta.getFrameHeight(), QImage::Format_RGB32);
                            patchedSequence.push_back(_xor_image);
                        }
                    }
               }
            }

            result.LoadFromImageSeq(patchedSequence, delta.getFrameWidth(),
                                    delta.getFrameHeight(), delta.getFPS());
            patchedSequence.clear();*/

            return result;
        }




        /* Processo de Merge */
        void MergeProcessing::Merge(Video *base, Video *v1, Video *v2)
        {

            // Calcular o diff entre a base e v1           
          /*  diff_base_v1.CalculateDiff(base, v1);

            // Calcular o diff entre a base e v2
            diff_base_v2.CalculateDiff(base, v2);

            // Inicializar a tabela de merge com os valores dos frames do video base
            std::vector<std::string>& baseSeq = base->getSequenceHash();
            printf("Num. Frames (Base): %d\n", baseSeq.size());
            for (int i = 0; i < baseSeq.size(); i++)
            {
                MergeItem it;
                it.baseFrame = baseSeq[i];
                it.v1_frame_diff = NULL; it.v2_frame_diff = NULL;
                it.v1_offset = -1; it.v2_offset = -1;
                mergeTable.insert(std::make_pair<int,MergeItem>(i, it));*/
           // }


            // Preencher a tabela de merge por frame
            /*std::vector<Frame_Diff>& _v1_diff_list = diff_base_v1.getFrameDiffList();
            printf("Num. Frames (V1_Diff): %d\n", _v1_diff_list.size());
            for (int i = 0; i < _v1_diff_list.size(); i++)
            {
                int index_frame = _v1_diff_list[i].index;

                mergeTable[index_frame].v1_frame_diff = &_v1_diff_list[i];
            }

            // Preencher a tabela de merge por frame
            std::vector<Frame_Diff>& _v2_diff_list = diff_base_v2.getFrameDiffList();
            printf("Num. Frames (V2_Diff): %d\n", _v2_diff_list.size());
            for (int i = 0; i < _v2_diff_list.size(); i++)
            {
                int index_frame = _v2_diff_list[i].index;
                printf("\nNum_Frames (v1: %d\n", _v2_diff_list[i].numFrames);

                mergeTable[index_frame].v2_frame_diff = &_v2_diff_list[i];
            }*/
        }
/*
        void MergeProcessing::DebugMergeTable()
        {
            printf("\nFrame\tV1_Operation\tV2_Operation\t\n");

            std::map<int,MergeItem>::iterator iter;

            for (iter = mergeTable.begin(); iter != mergeTable.end(); iter++)
            {
                printf("%d\t", iter->first);
                printf("V1: ");

                MergeItem *_item = &iter->second;

                if (_item->v1_frame_diff != NULL)
                {
                    switch(_item->v1_frame_diff->op)
                    {
                    case XOR: printf("XOR\t"); break;
                    case ADD: printf("ADD\t"); break;
                    case REMOVE: printf("REMOVE\t"); break;
                    }

                    printf("Frames: %d\tOffset: %d\t", _item->v1_frame_diff->numFrames,
                           _item->v1_offset);
                }
                else
                {
                    printf("NONE\t");
                }

                printf("V2: ");

                if (_item->v2_frame_diff != NULL)
                {
                    switch(_item->v2_frame_diff->op)
                    {
                    case XOR: printf("XOR\t"); break;
                    case ADD: printf("ADD\t"); break;
                    case REMOVE: printf("REMOVE\t"); break;
                    }

                    printf("Frames: %d\tOffset: %d\t", _item->v2_frame_diff->numFrames,
                           _item->v2_offset);
                }
                else
                {
                    printf("NONE");
                }

                printf("\n");
            }
        }

        sMergeLines& MergeProcessing::getMergeChannels()
        {
            int curV1Index = -1;
            int curV2Index = -1;
            int v1_frames_added = 0;
            Frame_Diff* v1_before = NULL;
            Frame_Diff* v2_before = NULL;
            _Operation v1_last_op = NONE;
            _Operation v2_last_op = NONE;

            std::map<int,MergeItem>::iterator iter;
            for (iter = mergeTable.begin(); iter != mergeTable.end(); iter++)
            {
                MergeItem* _merge_item = &iter->second;

                sMergeFrame itemBase;
                itemBase.frame_by_key = _merge_item->baseFrame;
                itemBase.show = true;
                itemBase.data = NULL;
                mergeChannels.channelBase.push_back(itemBase);

                if (_merge_item->v1_frame_diff == NULL && v1_before == NULL)
                {
                    sMergeFrame _aItem;
                    _aItem.op = NONE;
                    _aItem.show = true;
                    _aItem.frame_by_key = _merge_item->baseFrame;
                    _aItem.data = NULL;
                    mergeChannels.channelA.push_back(_aItem);
                    v1_last_op = NONE;
                }

                if (_merge_item->v2_frame_diff == NULL && v2_before == NULL)
                {
                    sMergeFrame _bItem;
                    _bItem.op = NONE;
                    _bItem.show = true;
                    _bItem.frame_by_key = _merge_item->baseFrame;
                    _bItem.data = NULL;
                    mergeChannels.channelB.push_back(_bItem);
                    v2_last_op = NONE;
                }


                if (v1_before == NULL)
                {
                    if (_merge_item->v1_frame_diff != NULL)
                    {
                        switch (_merge_item->v1_frame_diff->op)
                        {
                            case XOR:
                            {
                                sMergeFrame _aItem;
                                _aItem.op = XOR;
                                _aItem.show = true;
                                _aItem.data = &_merge_item->v1_frame_diff->buffer[0];
                                mergeChannels.channelA.push_back(_aItem);

                                if (_merge_item->v1_frame_diff->numFrames > 1)
                                {
                                    v1_before = _merge_item->v1_frame_diff;
                                    curV1Index = 1;

                                }

                                v1_last_op = XOR;
                            }
                            break;

                            case ADD:
                            {
                                if (v1_last_op == NONE)
                                {
                                    sMergeFrame _aItem;
                                    _aItem.op = NONE;
                                    _aItem.show = true;
                                    _aItem.frame_by_key = _merge_item->baseFrame;
                                    _aItem.data = NULL;
                                    mergeChannels.channelA.push_back(_aItem);
                                }

                                v1_frames_added = 0;
                                for (int i = 0; i < _merge_item->v1_frame_diff->numFrames; i++)
                                {
                                    size_t _sbytes = sizeof(uint8_t) * 4 * diff_base_v1.getFrameWidth() * diff_base_v1.getFrameHeight();

                                    sMergeFrame itemBase;
                                    itemBase.frame_by_key = _merge_item->baseFrame;
                                    itemBase.show = false;
                                    itemBase.data = NULL;
                                    mergeChannels.channelBase.push_back(itemBase);

                                    sMergeFrame _aItem;
                                    _aItem.op = ADD;
                                    _aItem.show = true;
                                    _aItem.data = &_merge_item->v1_frame_diff->buffer[i*_sbytes];
                                    mergeChannels.channelA.push_back(_aItem);

                                    v1_frames_added++;
                                }
                                v1_last_op = ADD;
                            }
                            break;

                            case REMOVE:
                            {
                                sMergeFrame _aItem;
                                _aItem.op = REMOVE;
                                _aItem.show = true;
                                _aItem.data = &_merge_item->v1_frame_diff->buffer[0];
                                mergeChannels.channelA.push_back(_aItem);

                                if (_merge_item->v1_frame_diff->numFrames > 1)
                                {
                                    v1_before = _merge_item->v1_frame_diff;
                                    curV1Index = 1;

                                }
                                v1_last_op = REMOVE;
                            }
                            break;

                        }
                    }
                }
                else
                {
                    size_t _sbytes = sizeof(uint8_t) * 4 * diff_base_v1.getFrameWidth() * diff_base_v1.getFrameHeight();


                    switch (v1_before->op)
                    {
                        case XOR:
                        {
                            sMergeFrame _aItem;
                            _aItem.op = XOR;
                            _aItem.show = true;
                            _aItem.data = &v1_before->buffer[curV1Index * _sbytes];
                            mergeChannels.channelA.push_back(_aItem);
                            v1_last_op = XOR;
                        }
                        break;

                        case REMOVE:
                        {
                            sMergeFrame _aItem;
                            _aItem.op = REMOVE;
                            _aItem.show = true;
                            _aItem.data = &v1_before->buffer[curV1Index * _sbytes];
                            mergeChannels.channelA.push_back(_aItem);

                            v1_last_op = REMOVE;
                        }
                        break;
                    }


                    curV1Index++;

                    if (curV1Index >= v1_before->numFrames)
                    {
                        curV1Index = -1;
                        v1_before = NULL;
                    }

                }



                // Video 2
                if (v2_before == NULL)
                {
                    if (_merge_item->v2_frame_diff != NULL)
                    {
                        switch (_merge_item->v2_frame_diff->op)
                        {
                            case XOR:
                            {
                                sMergeFrame _bItem;
                                _bItem.op = XOR;
                                _bItem.show = true;
                                _bItem.data = &_merge_item->v2_frame_diff->buffer[0];
                                mergeChannels.channelB.push_back(_bItem);

                                if (_merge_item->v2_frame_diff->numFrames > 1)
                                {
                                    v2_before = _merge_item->v2_frame_diff;
                                    curV2Index = 1;
                                }

                                v2_last_op = XOR;
                            }
                            break;

                            case ADD:
                            {
                                if (v2_last_op == NONE)
                                {
                                    sMergeFrame _bItem;
                                    _bItem.op = NONE;
                                    _bItem.show = true;
                                    _bItem.frame_by_key = _merge_item->baseFrame;
                                    _bItem.data = NULL;
                                    mergeChannels.channelB.push_back(_bItem);
                                }


                                for (int i = 0; i < _merge_item->v2_frame_diff->numFrames; i++)
                                {
                                    size_t _sbytes = sizeof(uint8_t) * 4 * diff_base_v2.getFrameWidth() * diff_base_v2.getFrameHeight();

                                    if (v1_frames_added == 0)
                                    {
                                        sMergeFrame itemBase;
                                        itemBase.frame_by_key = _merge_item->baseFrame;
                                        itemBase.show = false;
                                        itemBase.data = NULL;
                                        itemBase.op = NONE;
                                        mergeChannels.channelBase.push_back(itemBase);

                                        sMergeFrame _aItem;
                                        _aItem.show = false;
                                        _aItem.op = NONE;
                                        _aItem.data = NULL;
                                        mergeChannels.channelA.push_back(_aItem);

                                    }
                                    else v1_frames_added--;

                                    sMergeFrame _bItem;
                                    _bItem.op = ADD;
                                    _bItem.show = true;
                                    _bItem.data = &_merge_item->v2_frame_diff->buffer[i*_sbytes];
                                    mergeChannels.channelB.push_back(_bItem);

                                }
                                v2_last_op = ADD;
                            }
                            break;

                            case REMOVE:
                            {
                                sMergeFrame _bItem;
                                _bItem.op = REMOVE;
                                _bItem.show = true;
                                _bItem.data = &_merge_item->v2_frame_diff->buffer[0];
                                mergeChannels.channelB.push_back(_bItem);

                                if (_merge_item->v2_frame_diff->numFrames > 1)
                                {
                                    v2_before = _merge_item->v2_frame_diff;
                                    curV2Index = 1;

                                }

                                v2_last_op = REMOVE;
                            }
                            break;
                        }
                    }
                }
                else
                {
                    size_t _sbytes = sizeof(uint8_t) * 4 * diff_base_v2.getFrameWidth() * diff_base_v2.getFrameHeight();

                    switch (v2_before->op)
                    {
                        case XOR:
                        {
                            sMergeFrame _bItem;
                            _bItem.op = XOR;
                            _bItem.show = true;
                            _bItem.data = &v2_before->buffer[_sbytes * curV2Index];
                            mergeChannels.channelB.push_back(_bItem);

                            v2_last_op = XOR;
                        }
                        break;

                        case REMOVE:
                        {
                            sMergeFrame _bItem;
                            _bItem.op = REMOVE;
                            _bItem.show = true;
                            _bItem.data = &v2_before->buffer[_sbytes * curV2Index];
                            mergeChannels.channelB.push_back(_bItem);

                            v2_last_op = REMOVE;
                        }
                        break;
                    }


                    curV2Index++;

                    if (curV2Index >= v2_before->numFrames)
                    {
                        curV2Index = -1;
                        v2_before = NULL;
                    }

                }
            }

            for (int i = 0; i < mergeChannels.channelBase.size(); i++)
            {
                sMergeFrame *_baseItem = &mergeChannels.channelBase[i];
                sMergeFrame *_aItem = &mergeChannels.channelA[i];
                sMergeFrame *_bItem = &mergeChannels.channelB[i];

                // Verificar se houve operacoes
                if (_aItem->op == NONE && _bItem->op == NONE)
                {
                    sMergeResult _mergeRes;
                    _mergeRes.op = NONE;
                    _mergeRes.conflicted = false;
                    _mergeRes.data = NULL;
                    _mergeRes.paused = false;
                    _mergeRes.frame_by_key = _baseItem->frame_by_key;
                    mergeChannels.channelMerge.push_back(_mergeRes);
                }
                else if (_aItem->op == NONE || _bItem->op == NONE)
                {
                    sMergeFrame *_frame = (_aItem->op == NONE) ? _bItem : _aItem;

                    switch (_frame->op)
                    {
                        case XOR:
                        {
                            sMergeResult _mergeRes;
                            _mergeRes.op = XOR;
                            _mergeRes.conflicted = false;
                            _mergeRes.paused = false;
                            _mergeRes.data = _frame->data;

                            mergeChannels.channelMerge.push_back(_mergeRes);
                        }
                        break;

                        case REMOVE:
                        {
                            sMergeResult _mergeRes;
                            _mergeRes.op = REMOVE;
                            _mergeRes.conflicted = false;
                            _mergeRes.paused = true;

                            mergeChannels.channelMerge.push_back(_mergeRes);
                        }
                        break;

                        case ADD:
                        {
                            sMergeResult _mergeRes;
                            _mergeRes.op = ADD;
                            _mergeRes.conflicted = false;
                            _mergeRes.paused = false;
                            _mergeRes.data = _frame->data;

                            mergeChannels.channelMerge.push_back(_mergeRes);
                        }
                        break;
                    }
                }
                else
                {
                    // Conflito
                    sMergeResult _mergeRes;
                    _mergeRes.op = NONE;
                    _mergeRes.conflicted = true;
                    _mergeRes.paused = false;

                    mergeChannels.channelMerge.push_back(_mergeRes);

                }
            }

            return mergeChannels;
        }*/
    }
}


