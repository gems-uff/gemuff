#include "diffalgorithms.h"

#define NUM_BORDER 3

struct toProcess {
    int seq1_idx;
    int seq2_idx;
    int insertidx;
};

namespace GEMUFF {
    namespace Diff {

        int FindKey(Hash::AbstractHashPtr _key, VECTOR_ABSTRACT_HASH_PTR seq, int startIndex, float threshold){

            for (; startIndex < seq.size(); startIndex++){
                if (_key->isSimilar(seq[startIndex], threshold)){
                    return startIndex;
                }
            }

            return -1;
        }

        void CheckSimilarity(VECTOR_ABSTRACT_HASH_PTR _seq1,
            VECTOR_ABSTRACT_HASH_PTR _seq2, int &seq_1_offset,
            int &seq_2_offset, int seq1_count, int seq2_count,
            float threshold, std::vector<DiffChunk> &diffChunks){

#ifdef VIMUFF_INFO
            StopWatchInterface *timer = NULL;
            sdkCreateTimer(&timer);
#endif

            // Create a vector of similarity
            std::vector<Hash::AbstractHashPtr> _sim1, _sim2;
            int current_idx = seq_1_offset;
            std::vector<toProcess> lateProcessing;
            int maxElements = 25;


            for (int k = 0; k < seq1_count; k++){
                VIMUFF::ImagePtr _img = VIMUFF::ImageRegister::ImageAt(_seq1[seq_1_offset+k]);
                Hash::AbstractHashPtr _h1(Hash::DCTHash::GenerateHash(_img));
                _h1->setData(_seq1[seq_1_offset + k]);
                _sim1.push_back(_h1);
            }

            for (int k = 0; k < seq2_count; k++){
                VIMUFF::ImagePtr _img = VIMUFF::ImageRegister::ImageAt(_seq2[seq_2_offset + k]);
                Hash::AbstractHashPtr _h2(Hash::DCTHash::GenerateHash(_img));
                _h2->setData(_seq2[seq_2_offset + k]);
                _sim2.push_back(_h2);
            }

            std::vector<LCSEntry> _simLCS = Helper::LCS(_sim1, _sim2, threshold);


            if (_simLCS.size() == 0){

                for (int j = 0; j < seq1_count; j++){
                    // Removed
                   AddChunk(diffChunks, VIMUFF::ImagePtr(), boost::static_pointer_cast<Hash::MD5Hash>(_seq1[seq_1_offset]),
                           VIMUFF::ImagePtr(), Hash::MD5HashPtr(), DO_Remove, current_idx);
                   seq_1_offset++;
                }

                for (int j = 0; j < seq2_count; j++){
                    // Added
                   AddChunk(diffChunks, VIMUFF::ImagePtr(), Hash::MD5HashPtr(),
                            VIMUFF::ImageRegister::ImageAt(_seq2[seq_2_offset]), Hash::MD5HashPtr(),
                           DO_Add, current_idx);
                   seq_2_offset++;
                }
            } else {

                int l1_added = 0;
                int l2_added = 0;



                for (int k = 0; k < _simLCS.size(); k++){
                    Hash::AbstractHashPtr _or1 = _simLCS[k].l1_ref->getData();
                    Hash::AbstractHashPtr _or2 = _simLCS[k].l2_ref->getData();

                    int _seq1_lcs_sim = FindKey(_or1, _seq1, seq_1_offset, 1.0f);
                    int _seq2_lcs_sim = FindKey(_or2, _seq2, seq_2_offset, 1.0f);

                    int size_subseq1_sim = _seq1_lcs_sim - seq_1_offset;
                    int size_subseq2_sim = _seq2_lcs_sim - seq_2_offset;

                    if (size_subseq1_sim > 0 || size_subseq2_sim > 0){
                        if (lateProcessing.size() > 0){
                            int _currentLateIdx = 0;
                            int _steps = ceil(lateProcessing.size() / maxElements);

                            for (int _uu = 0; _uu < _steps; _uu++){
                                int tt = maxElements < lateProcessing.size() - _currentLateIdx ? maxElements : lateProcessing.size() - _currentLateIdx;

                                QImage __im1 = VIMUFF::ImageRegister::ImageAt(_seq1[lateProcessing[_currentLateIdx].seq1_idx])->toQImage();
                                QImage __im2 = VIMUFF::ImageRegister::ImageAt(_seq2[lateProcessing[_currentLateIdx].seq2_idx])->toQImage();
                                int _w = __im1.width();
                                int _h = __im2.height();
                                int _size = _w * _h;

                                uchar* imageData1 = (uchar*) malloc(sizeof(uchar) * 4 * tt * _size);
                                uchar* imageData2 = (uchar*) malloc(sizeof(uchar) * 4 * tt * _size);
                                uchar* imageDataRes = (uchar*) malloc(sizeof(uchar) * 4 * tt * _size);


                                for (int _cindex = 0; _cindex < tt; _cindex++){
                                    __im1 = VIMUFF::ImageRegister::ImageAt(_seq1[lateProcessing[_currentLateIdx + _cindex].seq1_idx])->toQImage();
                                    __im2 = VIMUFF::ImageRegister::ImageAt(_seq2[lateProcessing[_currentLateIdx + _cindex].seq2_idx])->toQImage();
                                    memcpy(&imageData1[4 * _size * _cindex], __im1.constBits(), sizeof(uchar) * 4 * _size);
                                    memcpy(&imageData2[4 * _size * _cindex], __im2.constBits(), sizeof(uchar) * 4 * _size);
                                }


                                // Process the diff
#ifdef VIMUFF_INFO
                                sdkResetTimer(&timer);
                                sdkStartTimer(&timer);
#endif

#ifdef VIMUFF_GPU

                                VIMUFF::ImageRegister::ProcessGPUDiffD(imageData1, imageData2, imageDataRes, tt * _size);
#else
                                VIMUFF::ImageRegister::ProcessCPUDiffD(imageData1, imageData2, imageDataRes, tt * _size);
#endif

#ifdef VIMUFF_INFO
                                sdkStopTimer(&timer);
                                qDebug() << "**** Total changed: " << tt <<
                                            " Mem. Used: " << (( (float)(tt * _size * sizeof(uchar)) * 4 * 3) / (float)(1024*1024)) <<
                                            " Time elapsed (ms): " << sdkGetTimerValue(&timer);
#endif

                                for (int _cindex = 0; _cindex < tt; _cindex++){

                                    QImage _diff =  QImage(&imageDataRes[4 * _size * _cindex],
                                            _w, _h, QImage::Format_RGB32);

                                    AddChunk(diffChunks, VIMUFF::ImagePtr(), boost::static_pointer_cast<Hash::MD5Hash>(_seq1[lateProcessing[_currentLateIdx + _cindex].seq1_idx]),
                                        VIMUFF::ImageRegister::toImage(&_diff), Hash::MD5HashPtr(), DO_Change, lateProcessing[_currentLateIdx + _cindex].insertidx);
                                }

                                _currentLateIdx += tt;
                                free(imageData1);
                                free(imageData2);
                                free(imageDataRes);
                            }
                        }
                        lateProcessing.clear();
                    }

                    for (int j = 0; j < size_subseq1_sim; j++){
                        // Removed
                       AddChunk(diffChunks, VIMUFF::ImagePtr(), boost::static_pointer_cast<Hash::MD5Hash>(_seq1[seq_1_offset]),
                                VIMUFF::ImagePtr(), Hash::MD5HashPtr(), DO_Remove, current_idx);
                       l1_added++;
                       seq_1_offset++;
                    }


                    for (int j = 0; j < size_subseq2_sim; j++){
                       AddChunk(diffChunks, VIMUFF::ImagePtr(), Hash::MD5HashPtr(),
                                VIMUFF::ImageRegister::ImageAt(_seq2[seq_2_offset]), Hash::MD5HashPtr(),
                               DO_Add, current_idx);
                       l2_added++;
                       seq_2_offset++;
                    }


                    toProcess tp;
                    tp.insertidx = current_idx;
                    tp.seq1_idx = seq_1_offset;
                    tp.seq2_idx = seq_2_offset;
                    lateProcessing.push_back(tp);

                    l1_added++;
                    l2_added++;

                    seq_1_offset++;
                    seq_2_offset++;
                }


                if (lateProcessing.size() > 0){
                    int _currentLateIdx = 0;
                    int _steps = ceil(lateProcessing.size() / maxElements);

                    for (int _uu = 0; _uu < _steps; _uu++){
                        int tt = maxElements < lateProcessing.size() - _currentLateIdx ? maxElements : lateProcessing.size() - _currentLateIdx;

                        QImage __im1 = VIMUFF::ImageRegister::ImageAt(_seq1[lateProcessing[_currentLateIdx].seq1_idx])->toQImage();
                        QImage __im2 = VIMUFF::ImageRegister::ImageAt(_seq2[lateProcessing[_currentLateIdx].seq2_idx])->toQImage();
                        int _w = __im1.width();
                        int _h = __im2.height();
                        int _size = _w * _h;

                        uchar* imageData1 = (uchar*) malloc(sizeof(uchar) * 4 * tt * _size);
                        uchar* imageData2 = (uchar*) malloc(sizeof(uchar) * 4 * tt * _size);
                        uchar* imageDataRes = (uchar*) malloc(sizeof(uchar) * 4 * tt * _size);

                        for (int _cindex = 0; _cindex < tt; _cindex++){
                            __im1 = VIMUFF::ImageRegister::ImageAt(_seq1[lateProcessing[_currentLateIdx + _cindex].seq1_idx])->toQImage();
                            __im2 = VIMUFF::ImageRegister::ImageAt(_seq2[lateProcessing[_currentLateIdx + _cindex].seq2_idx])->toQImage();
                            memcpy(&imageData2[4 * _size * _cindex], __im2.constBits(), sizeof(uchar) * 4 * _size);
                            memcpy(&imageData1[4 * _size * _cindex], __im1.constBits(), sizeof(uchar) * 4 * _size);

                        }

                        // Process the diff
#ifdef VIMUFF_INFO
                        sdkResetTimer(&timer);
                        sdkStartTimer(&timer);
#endif

#ifdef VIMUFF_GPU
                        VIMUFF::ImageRegister::ProcessGPUDiffD(imageData1, imageData2, imageDataRes, tt * _size);
#else
                        VIMUFF::ImageRegister::ProcessCPUDiffD(imageData1, imageData2, imageDataRes, tt * _size);
#endif

#ifdef VIMUFF_INFO
                        sdkStopTimer(&timer);
                        qDebug() << "**** Total changed: " << tt <<
                                    " Mem. Used: " << (( (float)(tt * _size * sizeof(uchar)) * 4 * 3) / (float)(1024*1024)) <<
                                    " Time elapsed (ms): " << sdkGetTimerValue(&timer);
#endif

                        for (int _cindex = 0; _cindex < tt; _cindex++){

                            QImage _diff =  QImage(&imageDataRes[4 * _size * _cindex],
                                    _w, _h, QImage::Format_RGB32);

                            AddChunk(diffChunks, VIMUFF::ImagePtr(), boost::static_pointer_cast<Hash::MD5Hash>(_seq1[lateProcessing[_currentLateIdx + _cindex].seq1_idx]),
                                VIMUFF::ImageRegister::toImage(&_diff), Hash::MD5HashPtr(), DO_Change, lateProcessing[_currentLateIdx + _cindex].insertidx);
                        }

                        _currentLateIdx += tt;
                        free(imageData1);
                        free(imageData2);
                        free(imageDataRes);
                    }
                }
                lateProcessing.clear();



                for (int j = 0; j < seq1_count - l1_added; j++){
                    // Removed
                   AddChunk(diffChunks, VIMUFF::ImagePtr(), boost::static_pointer_cast<Hash::MD5Hash>(_seq1[seq_1_offset]),
                            VIMUFF::ImagePtr(), Hash::MD5HashPtr(), DO_Remove, current_idx);
                   seq_1_offset++;
                }

                for (int j = 0; j < seq2_count - l2_added; j++){
                    // Added
                   AddChunk(diffChunks, VIMUFF::ImagePtr(), Hash::MD5HashPtr(),
                            VIMUFF::ImageRegister::ImageAt(_seq2[seq_2_offset]), Hash::MD5HashPtr(),
                           DO_Add, current_idx);
                   seq_2_offset++;
                }
            }

#ifdef VIMUFF_INFO
            sdkDeleteTimer(&timer);
#endif

        }

         Diff2Info Diff2(GEMUFF::VIMUFF::Video *v1,
            GEMUFF::VIMUFF::Video *v2, float threshold){

             //v1->Debug("/Users/josericardo/v1");
             //v2->Debug("/Users/josericardo/v2");

#ifdef VIMUFF_INFO
            qDebug() << "Processing diff2...";
#endif
            std::vector<Hash::AbstractHashPtr> _seq1 = v1->getSequenceHash();
            std::vector<Hash::AbstractHashPtr> _seq2 = v2->getSequenceHash();

            Diff2Info diff2Info;
            diff2Info.v1_fps = v1->getFPS();
            diff2Info.v2_fps = v2->getFPS();
            diff2Info.base_width = v1->getFrameWidth();
            diff2Info.base_height = v2->getFrameHeight();

#ifdef VIMUFF_INFO
            QTime time;
            time.restart();
#endif
            std::vector<LCSEntry> _lcs = Helper::LCS(_seq1, _seq2, threshold);
#ifdef VIMUFF_INFO
           qDebug() << "LCS Calculation time (ms): " << time.elapsed();
           time.restart();
#endif

            Process2Diff(_lcs, diff2Info.diffChunks, _seq1, _seq2, threshold);

#ifdef VIMUFF_INFO
           qDebug() << "Diff2 Calculation time (ms): " << time.elapsed();
           time.restart();
#endif

            return diff2Info;
        }



        Diff3Info Diff3(VIMUFF::Video *vbase, VIMUFF::Video *v1, VIMUFF::Video *v2, float threshold){
            std::vector<Hash::AbstractHashPtr> _seqBase = vbase->getSequenceHash();
            std::vector<Hash::AbstractHashPtr> _seq1 = v1->getSequenceHash();
            std::vector<Hash::AbstractHashPtr> _seq2 = v2->getSequenceHash();

            Diff3Info diff3Info;
            diff3Info.vbase_fps = vbase->getFPS();
            diff3Info.v1_fps = v1->getFPS();
            diff3Info.v2_fps = v2->getFPS();
            diff3Info.base_width = vbase->getFrameWidth();
            diff3Info.base_height = vbase->getFrameHeight();

            std::vector<LCSEntry> _lcs_base_v1 = Helper::LCS(_seqBase, _seq1, threshold);
            std::vector<LCSEntry> _lcs_base_v2 = Helper::LCS(_seqBase, _seq2, threshold);

            // For the first time, we have the SHA1Hash so they will be equals if having the same image
            std::vector<Hash::AbstractHashPtr> _lcs_seq_base_v1;
            for (int i = 0; i < _lcs_base_v1.size(); i++){
                _lcs_seq_base_v1.push_back(_lcs_base_v1[i].l1_ref);
            }

            std::vector<Hash::AbstractHashPtr> _lcs_seq_base_v2;
            for (int i = 0; i < _lcs_base_v2.size(); i++){
                _lcs_seq_base_v2.push_back(_lcs_base_v2[i].l1_ref);
            }

            // Calculate the lcs again
            diff3Info.lcs = Helper::LCS(_lcs_seq_base_v1, _lcs_seq_base_v2, threshold);

            // Process the diff between each video against the base
            std::vector<DiffChunk> bv1_chunks, bv2_chunks;
            Process2Diff(_lcs_base_v1, bv1_chunks, _seqBase, _seq1, threshold);
            Process2Diff(_lcs_base_v2, bv2_chunks, _seqBase, _seq2, threshold);


            int idx = 0;
            int idx_v1 = 0, idx_v2 = 0;

            while (idx < _seqBase.size()){

                Diff3Chunk chunk;


                while (idx_v1 < bv1_chunks.size() && bv1_chunks[idx_v1].index == idx){
                    DiffData diffData;

                    diffData.op = bv1_chunks[idx_v1].diffData.op;

                    if (diffData.op == DO_Add){
                        diffData.v2_Image = bv1_chunks[idx_v1].diffData.v2_Image;
                    } else if (diffData.op == DO_Remove){
                        diffData.v1_Image = VIMUFF::ImageRegister::ImageAt(bv1_chunks[idx_v1].diffData.v1_HashPtr);
                    } else if (diffData.op == DO_Change){
                        diffData.v1_Image = VIMUFF::ImageRegister::ImageAt(bv1_chunks[idx_v1].diffData.v1_HashPtr);
                        diffData.v2_Image = bv1_chunks[idx_v1].diffData.v2_Image;
                    } else {
                        diffData.v1_Image = bv1_chunks[idx_v1].diffData.v1_Image;
                        diffData.v2_Image = bv1_chunks[idx_v1].diffData.v2_Image;
                    }
                    chunk.basev1.push_back(diffData);
                    idx_v1++;
                }


                while (idx_v2 < bv2_chunks.size() && bv2_chunks[idx_v2].index == idx){
                    DiffData diffData;

                    diffData.op = bv2_chunks[idx_v2].diffData.op;

                    if (diffData.op == DO_Add){
                        diffData.v2_Image = bv2_chunks[idx_v2].diffData.v2_Image;
                    } else if (diffData.op == DO_Remove){
                        diffData.v1_Image = VIMUFF::ImageRegister::ImageAt(bv2_chunks[idx_v2].diffData.v1_HashPtr);
                    } else if (diffData.op == DO_Change){
                        diffData.v1_Image = VIMUFF::ImageRegister::ImageAt(bv2_chunks[idx_v2].diffData.v1_HashPtr);
                        diffData.v2_Image = bv2_chunks[idx_v2].diffData.v2_Image;
                    } else {
                        diffData.v1_Image = bv2_chunks[idx_v2].diffData.v1_Image;
                        diffData.v2_Image = bv2_chunks[idx_v2].diffData.v2_Image;
                    }


                    chunk.basev2.push_back(diffData);
                    idx_v2++;
                }

                if (chunk.basev1.size() > 0 || chunk.basev2.size() > 0){
                    chunk.index = idx;
                    diff3Info.diff3chunks.push_back(chunk);
                }

                idx++;
            }


            if (idx_v1 < bv1_chunks.size() || idx_v2 < bv2_chunks.size()){
                Diff3Chunk chunk;

                while (idx_v1 < bv1_chunks.size()){
                    DiffData diffData;
                    diffData.op = bv1_chunks[idx_v1].diffData.op;

                    if (diffData.op == DO_Add){
                        diffData.v2_Image = bv1_chunks[idx_v1].diffData.v2_Image;
                    } else if (diffData.op == DO_Remove){
                        diffData.v1_Image = VIMUFF::ImageRegister::ImageAt(bv1_chunks[idx_v1].diffData.v1_HashPtr);
                    } else if (diffData.op == DO_Change){
                        diffData.v1_Image = VIMUFF::ImageRegister::ImageAt(bv1_chunks[idx_v1].diffData.v1_HashPtr);
                        diffData.v2_Image = bv1_chunks[idx_v1].diffData.v2_Image;
                    } else {
                        diffData.v1_Image = bv1_chunks[idx_v1].diffData.v1_Image;
                        diffData.v2_Image = bv1_chunks[idx_v1].diffData.v2_Image;
                    }

                    chunk.basev1.push_back(diffData);
                    idx_v1++;
                }


                while (idx_v2 < bv2_chunks.size()){
                    DiffData diffData;
                    diffData.op = bv2_chunks[idx_v2].diffData.op;

                    if (diffData.op == DO_Add){
                        diffData.v2_Image = bv2_chunks[idx_v2].diffData.v2_Image;
                    } else if (diffData.op == DO_Remove){
                        diffData.v1_Image = VIMUFF::ImageRegister::ImageAt(bv2_chunks[idx_v2].diffData.v1_HashPtr);
                    } else if (diffData.op == DO_Change){
                        diffData.v1_Image = VIMUFF::ImageRegister::ImageAt(bv2_chunks[idx_v2].diffData.v1_HashPtr);
                        diffData.v2_Image = bv2_chunks[idx_v2].diffData.v2_Image;
                    } else {
                        diffData.v1_Image = bv2_chunks[idx_v2].diffData.v1_Image;
                        diffData.v2_Image = bv2_chunks[idx_v2].diffData.v2_Image;
                    }

                    chunk.basev2.push_back(diffData);
                    idx_v2++;
                }

                if (chunk.basev1.size() > 0 || chunk.basev2.size() > 0){
                    chunk.index = idx;
                    diff3Info.diff3chunks.push_back(chunk);
                }
            }

            return diff3Info;
        }

        void Process2Diff(std::vector<LCSEntry> &_lcs, std::vector<DiffChunk> &_diffChunks,
                          std::vector<Hash::AbstractHashPtr> &_seq1, std::vector<Hash::AbstractHashPtr> &_seq2,
                          float threshold){
            int seq1_current_index = 0;
            int seq2_current_index = 0;
            int current_lcs_idx = 0;
            int current_idx = 0;

#ifdef VIMUFF_INFO
            int totalSimiliartyProcessing = 0;
            QTime time;
#endif

            while (seq1_current_index < _seq1.size()){

                LCSEntry *_lcsKey = NULL;
                if (current_lcs_idx < _lcs.size() )
                    _lcsKey = &_lcs[current_lcs_idx++];


                if (_lcsKey != NULL){
                    int _seq1_lcs = FindKey(_lcsKey->l1_ref, _seq1, seq1_current_index, 1.0f);
                    int _seq2_lcs = FindKey(_lcsKey->l2_ref, _seq2, seq2_current_index, 1.0f);

                    int _seq1_nframes = _seq1_lcs - seq1_current_index;
                    int _seq2_nframes = _seq2_lcs - seq2_current_index;

                    //qDebug() << "Seq1_nFrames: " << _seq1_nframes << "Seq2_nFrames: " << _seq2_nframes;

                    // Check for similarity if both lists are fullfiled
                    if (_seq1_nframes > 0 && _seq2_nframes > 0){

                        CheckSimilarity(_seq1, _seq2, seq1_current_index, seq2_current_index,
                                        _seq1_nframes, _seq2_nframes, threshold, _diffChunks);
                    } else if (_seq1_nframes > 0){

                        for (; seq1_current_index < _seq1_lcs; seq1_current_index++){

                            // Removed
                           AddChunk(_diffChunks, VIMUFF::ImagePtr(), boost::static_pointer_cast<Hash::MD5Hash>(_seq1[seq1_current_index]),
                                    VIMUFF::ImagePtr(), Hash::MD5HashPtr(), DO_Remove, current_idx);
                        }
                    } else if (_seq2_nframes > 0){
                        for (; seq2_current_index < _seq2_lcs; seq2_current_index++){
                           AddChunk(_diffChunks, VIMUFF::ImagePtr(), Hash::MD5HashPtr(),
                                    VIMUFF::ImageRegister::ImageAt(_seq2[seq2_current_index]), Hash::MD5HashPtr(),
                                    DO_Add, current_idx);
                        }
                    }

                    seq1_current_index++;
                    seq2_current_index++;
                    current_idx = seq1_current_index;
                } else {

                    int _seq1_nframes = _seq1.size() - seq1_current_index - 1;
                    int _seq2_nframes = _seq2.size() - seq2_current_index - 1;

                    // Check for similarity if both lists are fullfiled
                    if (_seq1_nframes > 0 && _seq2_nframes > 0){

                        CheckSimilarity(_seq1, _seq2, seq1_current_index, seq2_current_index,
                                        _seq1_nframes, _seq2_nframes, threshold, _diffChunks);


                    } else if (_seq1_nframes > 0){

                        for (; seq1_current_index < _seq1.size(); seq1_current_index++){
                            // Removed
                           AddChunk(_diffChunks, VIMUFF::ImagePtr(), boost::static_pointer_cast<Hash::MD5Hash>(_seq1[seq1_current_index]),
                                    VIMUFF::ImagePtr(), Hash::MD5HashPtr(), DO_Remove, current_idx);
                        }
                    } else if (_seq2_nframes > 0){
                        for (; seq2_current_index < _seq2.size(); seq2_current_index++){
                           AddChunk(_diffChunks, VIMUFF::ImagePtr(), Hash::MD5HashPtr(),
                                    VIMUFF::ImageRegister::ImageAt(_seq2[seq2_current_index]), Hash::MD5HashPtr(),
                                    DO_Add, current_idx);
                        }
                    }

                    seq1_current_index++;
                    seq2_current_index++;
                    current_idx = seq1_current_index;

                }

            }


            // Process the remaining frames in seq2
            for (; seq2_current_index < _seq2.size(); seq2_current_index){
                AddChunk(_diffChunks, VIMUFF::ImagePtr(), Hash::MD5HashPtr(),
                         VIMUFF::ImageRegister::ImageAt(_seq2[seq2_current_index]), Hash::MD5HashPtr(),
                         DO_Add, _seq1.size());
                seq2_current_index++;
            }
        }

        void AddChunk(std::vector<DiffChunk> &_diffChunks, VIMUFF::ImagePtr _img1,
                      Hash::MD5HashPtr hash_v1, VIMUFF::ImagePtr _img2, Hash::MD5HashPtr hash_v2,
                      DiffOperation _op, int _index){
            DiffChunk chunk;
            chunk.diffData.op = _op;
            chunk.index = _index;

            chunk.diffData.v1_Image = _img1;
            chunk.diffData.v2_Image = _img2;
            chunk.diffData.v1_HashPtr = hash_v1;
            chunk.diffData.v2_HashPtr = hash_v2;
            _diffChunks.push_back(chunk);
        }

        VIMUFF::Video Patch(Diff2Info &_diff2, VIMUFF::Video *_v1){
            int current_index = 0;
            int current_chunk_idx = 0;

            std::vector<Hash::AbstractHashPtr> _v1Hash =
                    _v1->getSequenceHash();

            std::vector<Hash::AbstractHashPtr> resultHash;

            while (current_index < _v1Hash.size()){

                while (current_chunk_idx < _diff2.diffChunks.size()){
                    Diff::DiffChunk _chunk = _diff2.diffChunks[current_chunk_idx];

                    if (_chunk.index > current_index)
                        break;

                    if (_chunk.diffData.op == Diff::DO_Change){
                        // Apply the difference into image
                        QImage img1 = VIMUFF::ImageRegister::ImageAt(_chunk.diffData.v1_HashPtr)->toQImage();
                        QImage diff = _chunk.diffData.v2_Image->toQImage();

#ifdef VIMUFF_GPU
                        QImage img2 = GEMUFF::VIMUFF::ImageRegister::ProcessGPUPatch(&img1, &diff);
#else
                        QImage img2 = GEMUFF::VIMUFF::ImageRegister::ProcessCPUPatch(&img1, &diff);
#endif
                        resultHash.push_back(GEMUFF::VIMUFF::ImageRegister::RegisterFrame(GEMUFF::VIMUFF::ImageRegister::toImage(&img2)));
                        current_index++;
                        current_chunk_idx++;
                        continue;
                    } else if (_chunk.diffData.op == Diff::DO_Remove){
                     } else if (_chunk.diffData.op == Diff::DO_Add){
                        resultHash.push_back(GEMUFF::VIMUFF::ImageRegister::RegisterFrame(_chunk.diffData.v2_Image));
                    }

                    current_chunk_idx++;
                }

                resultHash.push_back(_v1Hash[current_index]);
                current_index++;
            }

            while (current_chunk_idx < _diff2.diffChunks.size()){
                Diff::DiffChunk _chunk = _diff2.diffChunks[current_chunk_idx];

                if (_chunk.index > current_index)
                    break;

                if (_chunk.diffData.op == Diff::DO_Change){
                    QImage img1 = _chunk.diffData.v1_Image->toQImage();
                    QImage diff = _chunk.diffData.v2_Image->toQImage();

#ifdef VIMUFF_GPU
                    QImage img2 = GEMUFF::VIMUFF::ImageRegister::ProcessGPUPatch(&img1, &diff);
#else
                    QImage img2 = GEMUFF::VIMUFF::ImageRegister::ProcessCPUPatch(&img1, &diff);
#endif

                    resultHash.push_back(GEMUFF::VIMUFF::ImageRegister::RegisterFrame(GEMUFF::VIMUFF::ImageRegister::toImage(&img2)));

                    current_index++;
                    current_chunk_idx++;
                    continue;
                } else if (_chunk.diffData.op == Diff::DO_Remove){
                 } else if (_chunk.diffData.op == Diff::DO_Add){
                    resultHash.push_back(GEMUFF::VIMUFF::ImageRegister::RegisterFrame(_chunk.diffData.v2_Image));
                }

                current_chunk_idx++;
            }

            //GEMUFF::VIMUFF::Video v_t;
            //v_t.LoadFromImages(v1.getSequenceHash(), 960, 540,
              //                 AV_CODEC_ID_RAWVIDEO, AV_PIX_FMT_RGB32, AV_PIX_FMT_YUV420P, 400000, 30, v1.getFormatContext());

            GEMUFF::VIMUFF::Video res;
         //   res.LoadFromImages(resultHash, _diff2.base_width, _diff2.base_height,
           //                    AV_CODEC_ID_RAWVIDEO, AV_PIX_FMT_RGB32, AV_PIX_FMT_YUV420P, 400000, 30, _v1->getFormatContext());

            res.LoadFromImages(resultHash, _diff2.base_width, _diff2.base_height,
                               _v1->getCodecID(), AV_PIX_FMT_RGB32, _v1->getPixelFormat(),
                               _v1->getBitRate(), _v1->getAVRational(), _v1->getGopSize(), _v1->getMaxBFrames(), _v1->getFormatContext());

            return res;

        }




    }
}
