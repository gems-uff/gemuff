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
            VECTOR_ABSTRACT_HASH_PTR _seq2, int seq_1_offset,
            int seq_2_offset, int seq1_count, int seq2_count,
            float threshold, std::vector<DiffData> &diffData, int res_w, int res_h){

        	int _tmp_seq1_offset = seq_1_offset;
        	int _tmp_seq2_offset = seq_2_offset;

#ifdef VIMUFF_INFO
            StopWatchInterface *timer = NULL;
            sdkCreateTimer(&timer);
#endif

            // Create a vector of similarity
            std::vector<Hash::AbstractHashPtr> _sim1, _sim2;
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


            fprintf(stderr, "Similarity: %d\n", _simLCS.size());
            if (_simLCS.size() == 0){

                for (int j = 0; j < seq1_count; j++){
                    // Removed
                   AddDiffData(diffData, VIMUFF::ImagePtr(), boost::static_pointer_cast<Hash::MD5Hash>(_seq1[seq_1_offset++]),
                           VIMUFF::ImagePtr(), Hash::MD5HashPtr(), DO_Remove);
                }

                for (int j = 0; j < seq2_count; j++){
                    // Added
                   AddDiffData(diffData, VIMUFF::ImagePtr(), Hash::MD5HashPtr(),
                            VIMUFF::ImageRegister::ImageAt(_seq2[seq_2_offset++]), Hash::MD5HashPtr(),
                           DO_Add);
                }
            } else {

            	std::vector<DiffData> _localDD;

                for (int k = 0; k < _simLCS.size(); k++){
                    Hash::AbstractHashPtr _or1 = _simLCS[k].l1_ref->getData();
                    Hash::AbstractHashPtr _or2 = _simLCS[k].l2_ref->getData();

                    int _seq1_lcs_sim = FindKey(_or1, _seq1, seq_1_offset, 1.0f);
                    int _seq2_lcs_sim = FindKey(_or2, _seq2, seq_2_offset, 1.0f);

                    int size_subseq1_sim = _seq1_lcs_sim - seq_1_offset;
                    int size_subseq2_sim = _seq2_lcs_sim - seq_2_offset;

                    for (int j = 0; j < size_subseq1_sim; j++){
                        // Removed
                       AddDiffData(_localDD, VIMUFF::ImagePtr(), boost::static_pointer_cast<Hash::MD5Hash>(_seq1[seq_1_offset++]),
                                VIMUFF::ImagePtr(), Hash::MD5HashPtr(), DO_Remove);
                    }

                    for (int j = 0; j < size_subseq2_sim; j++){
                       AddDiffData(_localDD, VIMUFF::ImagePtr(), Hash::MD5HashPtr(),
                                VIMUFF::ImageRegister::ImageAt(_seq2[seq_2_offset++]), Hash::MD5HashPtr(),
                               DO_Add);
                    }

                    AddDiffData(_localDD, VIMUFF::ImagePtr(), boost::static_pointer_cast<Hash::MD5Hash>(_seq1[seq_1_offset++]),
                    		VIMUFF::ImagePtr(), boost::static_pointer_cast<Hash::MD5Hash>(_seq2[seq_2_offset++]), DO_Change);
                }

                // Check for remaining frames
                int size_subseq1_sim = seq1_count - (seq_1_offset - _tmp_seq1_offset);
                int size_subseq2_sim = seq2_count - (seq_2_offset - _tmp_seq2_offset);

				for (int j = 0; j < size_subseq1_sim; j++) {
					// Removed
					AddDiffData(_localDD, VIMUFF::ImagePtr(),
							boost::static_pointer_cast<Hash::MD5Hash>(
									_seq1[seq_1_offset++]), VIMUFF::ImagePtr(),
							Hash::MD5HashPtr(), DO_Remove);
				}

				for (int j = 0; j < size_subseq2_sim; j++ ) {
					AddDiffData(_localDD, VIMUFF::ImagePtr(), Hash::MD5HashPtr(),
							VIMUFF::ImageRegister::ImageAt(_seq2[seq_2_offset++]),
							Hash::MD5HashPtr(), DO_Add);
				}

                // Calculate the diff using VIMUFF
                std::vector<int> _changedOpIdx;

                for (int i = 0; i < _localDD.size(); i++)
                	if (_localDD[i].op == DO_Change) _changedOpIdx.push_back(i);

                int _j = 0;
                int _size = res_w * res_h;
                while (_j < _changedOpIdx.size()){
                	int tt = maxElements < _changedOpIdx.size() - _j ? maxElements : _changedOpIdx.size() - _j;

	                uchar* imageData1 = (uchar*) malloc(sizeof(uchar) * 4 * tt * _size);
	                uchar* imageData2 = (uchar*) malloc(sizeof(uchar) * 4 * tt * _size);
	                uchar* imageDataRes = (uchar*) malloc(sizeof(uchar) * 4 * tt * _size);

                    for (int _cindex = 0; _cindex < tt; _cindex++){
                    	VIMUFF::ImagePtr __im1 = VIMUFF::ImageRegister::ImageAt(_localDD[_changedOpIdx[_j + _cindex]].v1_HashPtr);
                    	VIMUFF::ImagePtr __im2 = VIMUFF::ImageRegister::ImageAt(_localDD[_changedOpIdx[_j + _cindex]].v2_HashPtr);

                        assert(__im1->getWidth() == __im2->getWidth() && __im1->getHeight() == __im2->getHeight() &&
                        		__im1->getType() == CV_8SC4 && __im2->getType() == CV_8SC4);
                        assert(__im1->getData() != NULL && __im2->getData() != NULL);

                        memcpy(&imageData1[4 * _size * _cindex], __im1->getData(), sizeof(uchar) * 4 * _size);
                        memcpy(&imageData2[4 * _size * _cindex], __im2->getData(), sizeof(uchar) * 4 * _size);
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
                    fprintf(stdout, "**** Total changed: %d Mem. Used: %d Time elapsed (ms): %f\n",
                    		tt,
							(( (float)(tt * _size * sizeof(uchar)) * 4 * 3) / (float)(1024*1024)),
                            sdkGetTimerValue(&timer));
#endif

                    for (int _cindex = 0; _cindex < tt; _cindex++){

                        VIMUFF::ImagePtr _diff(new VIMUFF::Image(res_w, res_h, CV_8UC4, &imageDataRes[4 * _size * _cindex]));
                        _localDD[_changedOpIdx[_j + _cindex]].v2_Image = _diff;
                    }

                    free(imageData1);
                    free(imageData2);
                    free(imageDataRes);

                    _j += tt;
                }

                for (int i = 0; i < _localDD.size(); i++) diffData.push_back(_localDD[i]);
#ifdef VIMUFF_INFO
            sdkDeleteTimer(&timer);
#endif
            }

        }

         Diff2Info Diff2(GEMUFF::VIMUFF::VideoPtr v1,
            GEMUFF::VIMUFF::VideoPtr v2, float threshold){

        	 if (v1->getFrameWidth() != v2->getFrameWidth() ||
        			 v2->getFrameHeight() != v2->getFrameHeight()){
        		 fprintf(stderr, "Error: Video in different resolutions!\n");
        		 exit(-1);
        	 }

#ifdef VIMUFF_INFO
            fprintf(stdout, "Processing diff2...");
#endif
            std::vector<Hash::AbstractHashPtr> _seq1 = v1->getSequenceHash();
            std::vector<Hash::AbstractHashPtr> _seq2 = v2->getSequenceHash();

            Diff2Info diff2Info;
            diff2Info.v1_fps = v1->getFPS();
            diff2Info.v2_fps = v2->getFPS();
            diff2Info.base_width = v1->getFrameWidth();
            diff2Info.base_height = v2->getFrameHeight();

#ifdef VIMUFF_INFO
            GEMUFF::Util::Clock time;
            time.reset();
#endif
            std::vector<LCSEntry> _lcs = Helper::LCS(_seq1, _seq2);
            //Helper::PrintList(_lcs, "tst");
#ifdef VIMUFF_INFO
           fprintf(stdout, "LCS Calculation time (ms): %f\n", time.getMilliseconds());
           fprintf(stdout, "LCS Size: %d\n", _lcs.size());
           time.reset();
#endif

            Process2Diff(_lcs, diff2Info.diffChunks, _seq1, _seq2, threshold, diff2Info.base_width, diff2Info.base_height);

#ifdef VIMUFF_INFO
           fprintf(stdout, "Diff2 Calculation time (ms): %f\n", time.getMilliseconds());
           time.reset();
#endif

            return diff2Info;
        }



         Diff3Info Diff3(VIMUFF::VideoPtr vbase, VIMUFF::VideoPtr v1, VIMUFF::VideoPtr v2, float threshold){
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

        	 std::vector<LCSEntry> _lcs_base_v1_v2 = Helper::LCS(_lcs_seq_base_v1, _lcs_seq_base_v2);

        	 Process2Diff(_lcs_base_v1, diff3Info.diffBaseToV1, _seqBase, _seq1, threshold, diff3Info.base_width, diff3Info.base_height);
        	 Process2Diff(_lcs_base_v2, diff3Info.diffBaseToV2, _seqBase, _seq2, threshold, diff3Info.base_width, diff3Info.base_height);

        	 int current_lcs_idx = 0;
        	 int current_v1_idx = 0;
        	 int current_v2_idx = 0;
        	 int current_base_idx = 0;

        	 while (current_lcs_idx < _lcs_base_v1_v2.size()){
        		 Diff3Chunk _d3c;

        		 _d3c.lcsEntry = _lcs_base_v1_v2[current_lcs_idx++];

        		 int _seqb_lcs = FindKey(_d3c.lcsEntry.l1_ref, _seqBase, current_base_idx, 1.0f);
        		 int _seqb_nframes = _seqb_lcs - current_base_idx;

        		 // Seq Base
        		 for (; current_base_idx < _seqb_lcs; current_base_idx++){
        			 _d3c.base.push_back(_seqBase[current_base_idx]);
        		 }
        		 current_base_idx++;

        		 // V1
        		 _d3c.v1.nFrames = 0;
        		 for (; current_v1_idx < diff3Info.diffBaseToV1.size(); current_v1_idx++ ){

        			 DiffChunk _df = diff3Info.diffBaseToV1[current_v1_idx];

                     if (_df.lcsEntry.l2_ref->isSimilar(_d3c.lcsEntry.l1_ref, 1.0f)){
        				 break;
        			 }
                     _d3c.v1.nFrames++;
        		 }
        		 _d3c.v1.offset = current_v1_idx;
        		 current_v1_idx++;


        		 // V2
        		 _d3c.v2.nFrames = 0;
        		 for (; current_v2_idx < diff3Info.diffBaseToV2.size(); current_v2_idx++ ){

        			 DiffChunk _df = diff3Info.diffBaseToV2[current_v2_idx];

                     if (_df.lcsEntry.l2_ref->isSimilar(_d3c.lcsEntry.l1_ref, 1.0f)){
        				 break;
        			 }
                     _d3c.v2.nFrames++;
        		 }
        		 _d3c.v2.offset = current_v2_idx;
        		 current_v2_idx++;


				 diff3Info.diff3Chunks.push_back(_d3c);
        	 }



        	 // Remaining Data
        	 int _seqb_nframes = _seqBase.size() - current_base_idx;
        	 int _seq1_nframes = diff3Info.diffBaseToV1.size() - current_v1_idx;
        	 int _seq2_nframes = diff3Info.diffBaseToV2.size() - current_v2_idx;

        	 if (_seqb_nframes > 0 || _seq1_nframes > 0 || _seq2_nframes > 0){

        		 Diff3Chunk _d3c;


        		 // Seq Base
        		 for (; current_base_idx < _seqBase.size(); current_base_idx++){
        			 _d3c.base.push_back(_seqBase[current_base_idx]);
        		 }

        		 // V1
        		 _d3c.v1.offset = current_v1_idx + _seq1_nframes - 1;
        		 _d3c.v1.nFrames = _seq1_nframes - 1;

        		 // V2
        		 _d3c.v2.offset = current_v2_idx + _seq2_nframes - 1;
        		 _d3c.v2.nFrames = _seq2_nframes - 1;


        		 diff3Info.diff3Chunks.push_back(_d3c);
        	 }


        	 return diff3Info;
         }

        void Process2Diff(std::vector<LCSEntry>& _lcs, std::vector<DiffChunk> &_diffChunks,
                          std::vector<Hash::AbstractHashPtr> &_seq1, std::vector<Hash::AbstractHashPtr> &_seq2,
                          float threshold, int res_w, int res_h){
            int seq1_current_index = 0;
            int seq2_current_index = 0;
            int current_lcs_idx = 0;
            int current_idx = 0;

#ifdef VIMUFF_INFO
            int totalSimiliartyProcessing = 0;
            Util::Clock time;
#endif


            while (current_lcs_idx < _lcs.size()){
                DiffChunk _dc;
                _dc.lcsEntry = _lcs[current_lcs_idx++];

            	int _seq1_lcs = FindKey(_dc.lcsEntry.l1_ref, _seq1, seq1_current_index, 1.0f);
                int _seq2_lcs = FindKey(_dc.lcsEntry.l2_ref, _seq2, seq2_current_index, 1.0f);

                int _seq1_nframes = _seq1_lcs - seq1_current_index;
                int _seq2_nframes = _seq2_lcs - seq2_current_index;

                // Check for similarity if both lists are fullfiled
                if (_seq1_nframes > 0 && _seq2_nframes > 0){
                    CheckSimilarity(_seq1, _seq2, seq1_current_index, seq2_current_index,
                                    _seq1_nframes, _seq2_nframes, threshold, _dc.diffData, res_w, res_h);

                   seq1_current_index += _seq1_nframes + 1;
                   seq2_current_index += _seq2_nframes + 1;

                } else if (_seq1_nframes > 0){

                    for (; seq1_current_index < _seq1_lcs; seq1_current_index++){

                        // Removed
                       AddDiffData(_dc.diffData, VIMUFF::ImagePtr(), boost::static_pointer_cast<Hash::MD5Hash>(_seq1[seq1_current_index]),
                                VIMUFF::ImagePtr(), Hash::MD5HashPtr(), DO_Remove);
                    }
                } else if (_seq2_nframes > 0){
                    for (; seq2_current_index < _seq2_lcs; seq2_current_index++){
                       AddDiffData(_dc.diffData, VIMUFF::ImagePtr(), Hash::MD5HashPtr(),
                                VIMUFF::ImageRegister::ImageAt(_seq2[seq2_current_index]), Hash::MD5HashPtr(),
                                DO_Add);
                    }
                } else {
                	seq1_current_index++;
                	seq2_current_index++;
                }

                _diffChunks.push_back(_dc);
            }

            // Remaining frames
            DiffChunk _dc;
            int _seq1_nframes = _seq1.size() - seq1_current_index;
            int _seq2_nframes = _seq2.size() - seq2_current_index;

            // Check for similarity if both lists are fullfiled
            if (_seq1_nframes > 0 && _seq2_nframes > 0){
            	CheckSimilarity(_seq1, _seq2, seq1_current_index, seq2_current_index,
            			_seq1_nframes, _seq2_nframes, threshold, _dc.diffData, res_w, res_h);
            } else if (_seq1_nframes > 0){

            	for (; seq1_current_index < _seq1.size(); seq1_current_index++){

            		// Removed
					AddDiffData(_dc.diffData, VIMUFF::ImagePtr(), boost::static_pointer_cast<Hash::MD5Hash>(_seq1[seq1_current_index]),
							VIMUFF::ImagePtr(), Hash::MD5HashPtr(), DO_Remove);
            	}
            } else if (_seq2_nframes > 0){
            	for (; seq2_current_index < _seq2.size(); seq2_current_index++){
            		AddDiffData(_dc.diffData, VIMUFF::ImagePtr(), Hash::MD5HashPtr(),
            				VIMUFF::ImageRegister::ImageAt(_seq2[seq2_current_index]), Hash::MD5HashPtr(),
							DO_Add);
            	}
            }

            _diffChunks.push_back(_dc);

        }

        void AddDiffData(std::vector<DiffData> &_diffData, VIMUFF::ImagePtr _img1,
                      Hash::MD5HashPtr hash_v1, VIMUFF::ImagePtr _img2, Hash::MD5HashPtr hash_v2,
                      DiffOperation _op){
            DiffData data;
            data.op = _op;

            data.v1_Image = _img1;
            data.v2_Image = _img2;
            data.v1_HashPtr = hash_v1;
            data.v2_HashPtr = hash_v2;
            _diffData.push_back(data);
        }

        VIMUFF::Video* Patch(Diff2Info &_diff2, VIMUFF::VideoPtr _v1){
            int current_index = 0;
            int current_chunk_idx = 0;

            std::vector<Hash::AbstractHashPtr> _v1Hash =
                    _v1->getSequenceHash();

            std::vector<Hash::AbstractHashPtr> resultHash;

            /*while (current_index < _v1Hash.size()){

            	fprintf(stderr, "Patch Index: %d", current_index);

                while (current_chunk_idx < _diff2.diffChunks.size()){
                    Diff::DiffChunk _chunk = _diff2.diffChunks[current_chunk_idx];

                    if (_chunk.index > current_index)
                        break;

                    if (_chunk.diffData.op == Diff::DO_Change){
                        // Apply the difference into image
                    	VIMUFF::ImagePtr img1 = VIMUFF::ImageRegister::ImageAt(_chunk.diffData.v1_HashPtr);
                    	VIMUFF::ImagePtr diff = _chunk.diffData.v2_Image;

#ifdef VIMUFF_GPU
                    	VIMUFF::ImagePtr img2 = GEMUFF::VIMUFF::ImageRegister::ProcessGPUPatch(img1, diff);
#else
                    	VIMUFF::ImagePtr img2 = GEMUFF::VIMUFF::ImageRegister::ProcessCPUPatch(img1, diff);
#endif
                        resultHash.push_back(GEMUFF::VIMUFF::ImageRegister::RegisterFrame(img2, _v1->getHashType()));
                        current_index++;
                        current_chunk_idx++;
                        continue;
                    } else if (_chunk.diffData.op == Diff::DO_Remove){
                     } else if (_chunk.diffData.op == Diff::DO_Add){
                        resultHash.push_back(GEMUFF::VIMUFF::ImageRegister::RegisterFrame(_chunk.diffData.v2_Image, _v1->getHashType()));
                    }

                    current_chunk_idx++;
                }

                resultHash.push_back(_v1Hash[current_index]);
                current_index++;
            }

            while (current_chunk_idx < _diff2.diffChunks.size()){
                Diff::DiffChunk _chunk = _diff2.diffChunks[current_chunk_idx];
                fprintf(stderr, "Patch Index: %d", current_index);

                if (_chunk.index > current_index)
                    break;

                if (_chunk.diffData.op == Diff::DO_Change){
                    VIMUFF::ImagePtr img1 = _chunk.diffData.v1_Image;
                    VIMUFF::ImagePtr diff = _chunk.diffData.v2_Image;

#ifdef VIMUFF_GPU
                    VIMUFF::ImagePtr img2 = GEMUFF::VIMUFF::ImageRegister::ProcessGPUPatch(img1, diff);
#else
                    VIMUFF::ImagePtr img2 = GEMUFF::VIMUFF::ImageRegister::ProcessCPUPatch(img1, diff);
#endif

                    resultHash.push_back(GEMUFF::VIMUFF::ImageRegister::RegisterFrame(img2, _v1->getHashType()));

                    current_index++;
                    current_chunk_idx++;
                    continue;
                } else if (_chunk.diffData.op == Diff::DO_Remove){
                 } else if (_chunk.diffData.op == Diff::DO_Add){
                    resultHash.push_back(GEMUFF::VIMUFF::ImageRegister::RegisterFrame(_chunk.diffData.v2_Image, _v1->getHashType()));
                }

                current_chunk_idx++;
            }*/

            //GEMUFF::VIMUFF::Video v_t;
            //v_t.LoadFromImages(v1.getSequenceHash(), 960, 540,
              //                 AV_CODEC_ID_RAWVIDEO, AV_PIX_FMT_RGB32, AV_PIX_FMT_YUV420P, 400000, 30, v1.getFormatContext());

         //   res.LoadFromImages(resultHash, _diff2.base_width, _diff2.base_height,
           //                    AV_CODEC_ID_RAWVIDEO, AV_PIX_FMT_RGB32, AV_PIX_FMT_YUV420P, 400000, 30, _v1->getFormatContext());

            GEMUFF::VIMUFF::Video *res = GEMUFF::VIMUFF::Video::loadFromImages(_v1->getHashType(), resultHash, _diff2.base_width, _diff2.base_height,
                               _v1->getCodecID(), AV_PIX_FMT_RGB32, _v1->getPixelFormat(),
                               _v1->getBitRate(), _v1->getAVRational(), _v1->getGopSize(), _v1->getMaxBFrames(), _v1->getFormat());

            return res;

        }




    }
}
