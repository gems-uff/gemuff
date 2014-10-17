#include "diffalgorithms.h"

#define NUM_BORDER 3

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
            float threshold, std::vector<Diff2Chunk> &diff2Chunks){

            // Create a vector of similarity
            std::vector<Hash::AbstractHashPtr> _sim1, _sim2;
            int current_idx = seq_1_offset;


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
                int size_subseq1 = seq1_count - seq_1_offset;
                int size_subseq2 = seq2_count - seq_2_offset;

                for (int j = 0; j < size_subseq1; j++){
                    // Removed
                   AddChunk(diff2Chunks, VIMUFF::ImageRegister::ImageAt(_seq1[seq_1_offset++]),
                           VIMUFF::ImagePtr(), DO_Remove, current_idx);
                }

                for (int j = 0; j < size_subseq2; j++){
                    // Added
                   AddChunk(diff2Chunks, VIMUFF::ImagePtr(),
                            VIMUFF::ImageRegister::ImageAt(_seq2[seq_2_offset++]), DO_Add, current_idx);
                }
            } else {

                for (int k = 0; k < _simLCS.size(); k++){
                    Hash::AbstractHashPtr _or1 = _simLCS[k].l1_ref->getData();
                    Hash::AbstractHashPtr _or2 = _simLCS[k].l2_ref->getData();

                    int _seq1_lcs_sim = FindKey(_or1, _seq1, seq_1_offset, threshold);
                    int _seq2_lcs_sim = FindKey(_or2, _seq2, seq_2_offset, threshold);

                    int size_subseq1_sim = _seq1_lcs_sim - seq_1_offset;
                    int size_subseq2_sim = _seq2_lcs_sim - seq_2_offset;

                    for (int j = 0; j < size_subseq1_sim; j++){
                        // Removed
                       AddChunk(diff2Chunks, VIMUFF::ImageRegister::ImageAt(_seq1[seq_1_offset++]),
                               VIMUFF::ImagePtr(), DO_Remove, current_idx);
                    }

                    for (int j = 0; j < size_subseq2_sim; j++){
                       AddChunk(diff2Chunks, VIMUFF::ImageRegister::ImageAt(_seq2[seq_2_offset++]),
                                VIMUFF::ImagePtr(), DO_Add, current_idx);
                    }


                    // Locate the lcs and mark it as changed
                    QImage _im1 = VIMUFF::ImageRegister::ImageAt(_seq1[seq_1_offset])->toQImage();
                    QImage _im2 = VIMUFF::ImageRegister::ImageAt(_seq2[seq_2_offset])->toQImage();

                    QImage _diff = VIMUFF::ImageRegister::ProcessGPUDiff(&_im1, &_im2);

                    AddChunk(diff2Chunks, VIMUFF::ImageRegister::ImageAt(_seq1[seq_1_offset]),
                             VIMUFF::ImageRegister::toImage(&_diff), DO_Change, current_idx);

                    seq_1_offset++;
                    seq_2_offset++;
                    current_idx = seq_1_offset;
                }
            }
        }

         Diff2Info Diff2(GEMUFF::VIMUFF::Video *v1,
            GEMUFF::VIMUFF::Video *v2, float threshold){

             //v1->Debug("/Users/josericardo/v1");
             //v2->Debug("/Users/josericardo/v2");

            std::vector<Hash::AbstractHashPtr> _seq1 = v1->getSequenceHash();
            std::vector<Hash::AbstractHashPtr> _seq2 = v2->getSequenceHash();

            Diff2Info diff2Info;
            diff2Info.v1_fps = v1->getFPS();
            diff2Info.v2_fps = v2->getFPS();
            diff2Info.base_width = v1->getFrameWidth();
            diff2Info.base_height = v2->getFrameHeight();

            std::vector<LCSEntry> _lcs = Helper::LCS(_seq1, _seq2, threshold);

            Process2Diff(_lcs, diff2Info.diff2Chunks, _seq1, _seq2, threshold);

            return diff2Info;
        }



        Diff3Info Diff3(VIMUFF::Video *vbase, VIMUFF::Video *v1, VIMUFF::Video *v2, float threshold){

            std::vector<Hash::AbstractHashPtr> _seqBase = v1->getSequenceHash();
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
            std::vector<LCSEntry> _lcs_base_v1_base_v2 = Helper::LCS(_lcs_seq_base_v1, _lcs_seq_base_v2, threshold);

            // Process the diff between each video against the base
            Process2Diff(_lcs_base_v1_base_v2, diff3Info.base_v1, _seqBase, _seq1, threshold);
            Process2Diff(_lcs_base_v1_base_v2, diff3Info.base_v1, _seqBase, _seq1, threshold);

            return diff3Info;
         }

        void Process2Diff(std::vector<LCSEntry> &_lcs, std::vector<Diff2Chunk> &_diff2Chunks,
                          std::vector<Hash::AbstractHashPtr> &_seq1, std::vector<Hash::AbstractHashPtr> &_seq2,
                          float threshold){
            int seq1_current_index = 0;
            int seq2_current_index = 0;
            int current_lcs_idx = 0;
            int current_idx = 0;

            while (seq1_current_index < _seq1.size()){

                LCSEntry *_lcsKey = NULL;
                if (current_lcs_idx < _lcs.size() )
                    _lcsKey = &_lcs[current_lcs_idx++];


                if (_lcsKey != NULL){
                    int _seq1_lcs = FindKey(_lcsKey->l1_ref, _seq1, seq1_current_index, threshold);
                    int _seq2_lcs = FindKey(_lcsKey->l2_ref, _seq2, seq2_current_index, threshold);

                    int _seq1_nframes = _seq1_lcs - seq1_current_index;
                    int _seq2_nframes = _seq2_lcs - seq2_current_index;


                    // Check for similarity if both lists are fullfiled
                    if (_seq1_nframes > 0 && _seq2_nframes > 0){

                        CheckSimilarity(_seq1, _seq2, seq1_current_index, seq2_current_index,
                                        _seq1_nframes, _seq2_nframes, threshold, _diff2Chunks);
                    } else if (_seq1_nframes > 0){

                        for (; seq1_current_index < _seq1_lcs; seq1_current_index++){

                            // Removed
                           AddChunk(_diff2Chunks, VIMUFF::ImageRegister::ImageAt(_seq1[seq1_current_index]),
                                    VIMUFF::ImagePtr(), DO_Remove, current_idx);
                        }
                    } else if (_seq2_nframes > 0){
                        for (; seq2_current_index < _seq2_lcs; seq2_current_index++){
                           AddChunk(_diff2Chunks, VIMUFF::ImagePtr(), VIMUFF::ImageRegister::ImageAt(_seq2[seq2_current_index]),
                                    DO_Add, current_idx);
                        }
                    }

                    seq1_current_index++;
                    seq2_current_index++;
                    current_idx = seq2_current_index;
                } else {

                    int _seq1_nframes = _seq1.size() - seq1_current_index;
                    int _seq2_nframes = _seq2.size() - seq2_current_index;

                    // Check for similarity if both lists are fullfiled
                    if (_seq1_nframes > 0 && _seq2_nframes > 0){

                        CheckSimilarity(_seq1, _seq2, seq1_current_index, seq2_current_index,
                                        _seq1.size(), _seq2.size(), threshold, _diff2Chunks);

                    } else if (_seq1_nframes > 0){

                        for (; seq1_current_index < _seq1.size(); seq1_current_index++){
                            // Removed
                           AddChunk(_diff2Chunks, VIMUFF::ImageRegister::ImageAt(_seq1[seq1_current_index]),
                                    VIMUFF::ImagePtr(), DO_Remove, current_idx);
                        }
                    } else if (_seq2_nframes > 0){
                        for (; seq2_current_index < _seq2.size(); seq2_current_index++){
                           AddChunk(_diff2Chunks, VIMUFF::ImagePtr(),
                                    VIMUFF::ImageRegister::ImageAt(_seq2[seq2_current_index]),
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
                AddChunk(_diff2Chunks, VIMUFF::ImagePtr(),
                         VIMUFF::ImageRegister::ImageAt(_seq2[seq2_current_index++]),
                         DO_Add, _seq1.size());
            }
        }

        void AddChunk(std::vector<Diff2Chunk> &_diff2Chunks, VIMUFF::ImagePtr _img1,
                      VIMUFF::ImagePtr _img2, DiffOperation _op, int _index){
            Diff2Chunk chunk;
            chunk.op = _op;
            chunk.index = _index;

            chunk.v1_Image = _img1;
            chunk.v2_Image = _img2;
            _diff2Chunks.push_back(chunk);
        }

        VIMUFF::Video Patch(Diff2Info &_diff2, VIMUFF::Video *_v1){
            int current_index = 0;
            int current_chunk_idx = 0;

            std::vector<Hash::AbstractHashPtr> _v1Hash =
                    _v1->getSequenceHash();

            std::vector<Hash::AbstractHashPtr> resultHash;

            while (current_index < _v1Hash.size()){

                while (current_chunk_idx < _diff2.diff2Chunks.size()){
                    Diff::Diff2Chunk _chunk = _diff2.diff2Chunks[current_chunk_idx];

                    if (_chunk.index > current_index)
                        break;

                    if (_chunk.op == Diff::DO_Change){
                        // Apply the difference into image
                        QImage img1 = _chunk.v1_Image->toQImage();
                        QImage diff = _chunk.v2_Image->toQImage();
                        QImage img2 = GEMUFF::VIMUFF::ImageRegister::ProcessGPUPatch(&img1, &diff);

                        resultHash.push_back(GEMUFF::VIMUFF::ImageRegister::RegisterFrame(GEMUFF::VIMUFF::ImageRegister::toImage(&img2)));
                        current_index++;
                        current_chunk_idx++;
                        continue;
                    } else if (_chunk.op == Diff::DO_Remove){
                     } else if (_chunk.op == Diff::DO_Add){
                        resultHash.push_back(GEMUFF::VIMUFF::ImageRegister::RegisterFrame(_chunk.v2_Image));
                    }

                    current_chunk_idx++;
                }

                resultHash.push_back(_v1Hash[current_index]);
                current_index++;
            }

            while (current_chunk_idx < _diff2.diff2Chunks.size()){
                Diff::Diff2Chunk _chunk = _diff2.diff2Chunks[current_chunk_idx];

                if (_chunk.index > current_index)
                    break;

                if (_chunk.op == Diff::DO_Change){
                    QImage img1 = _chunk.v1_Image->toQImage();
                    QImage diff = _chunk.v2_Image->toQImage();
                    QImage img2 = GEMUFF::VIMUFF::ImageRegister::ProcessGPUPatch(&img1, &diff);

                    resultHash.push_back(GEMUFF::VIMUFF::ImageRegister::RegisterFrame(GEMUFF::VIMUFF::ImageRegister::toImage(&img2)));

                    current_index++;
                    current_chunk_idx++;
                    continue;
                } else if (_chunk.op == Diff::DO_Remove){
                 } else if (_chunk.op == Diff::DO_Add){
                    resultHash.push_back(GEMUFF::VIMUFF::ImageRegister::RegisterFrame(_chunk.v2_Image));
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


