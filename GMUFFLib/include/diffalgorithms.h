#ifndef DIFFALGORITHMS_H
#define DIFFALGORITHMS_H

#include <vector>
#include <string>
#include "helper_timer.h"
#include "video.h"
#include "GEMUFFLib_global.h"
#include "helperfunctions.h"
#include <zip.h>
#include <zlib.h>

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384
#define B_FRAME_SIZE 10

namespace GEMUFF {
    namespace Diff {

    	enum Frame_Type {
    		Frame_Base,
			Frame_Diff
    	};

        typedef std::vector<Hash::AbstractHashPtr> VECTOR_ABSTRACT_HASH_PTR;


        /***************** Type definitions ********************/
        struct Node
        {
            Hash::AbstractHashPtr id1;
            Hash::AbstractHashPtr id2;
            float confidence;
            std::vector<Hash::AbstractHashPtr> seq1;
            std::vector<Hash::AbstractHashPtr> seq2;
        };


        enum DiffOperation {
            DO_Add,
            DO_Remove,
            DO_Change,
            DO_None
        };

        enum ChangeSide {
            CS_LEFT,
            CS_RIGHT,
            CS_BOTH,
            CS_None
        };

        // New struct for saving
        struct DiffSaveData {
            std::vector<DiffOperation> ops;
            std::vector<VIMUFF::ImagePtr> images;
            std::vector<Hash::MD5HashPtr> hashes;
            std::vector<int> indexes;
        };

        struct DiffData {
            DiffOperation op;
            VIMUFF::ImagePtr v1_Image;
            VIMUFF::ImagePtr v2_Image;
            Hash::MD5HashPtr v1_HashPtr;
            Hash::MD5HashPtr v2_HashPtr;
        };

        struct DiffChunk {
        	LCSEntry lcsEntry;
            std::vector<DiffData> diffData;
        };

        struct SeqMark {
        	int offset;
        	int nFrames;
        };


        struct Diff3Chunk {
            LCSEntry lcsEntry;
            std::vector<Hash::AbstractHashPtr> base;
            SeqMark v1;
            SeqMark v2;
        };



        struct Diff2Info {

            int v1_fps;
            int v2_fps;
            int base_width;
            int base_height;

            std::vector<DiffChunk> diffChunks;

            int compressFile(FILE *source, FILE *dest, int level)
                {
                    int ret, flush;
                    unsigned have;
                    z_stream strm;
                    unsigned char in[CHUNK];
                    unsigned char out[CHUNK];

                    /* allocate deflate state */
                    strm.zalloc = Z_NULL;
                    strm.zfree = Z_NULL;
                    strm.opaque = Z_NULL;
                    ret = deflateInit(&strm, level);
                    if (ret != Z_OK)
                        return ret;

                    /* compress until end of file */
                    do {
                        strm.avail_in = fread(in, 1, CHUNK, source);
                        if (ferror(source)) {
                            (void)deflateEnd(&strm);
                            return Z_ERRNO;
                        }
                        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
                        strm.next_in = in;

                        /* run deflate() on input until output buffer not full, finish
                           compression if all of source has been read in */
                        do {
                            strm.avail_out = CHUNK;
                            strm.next_out = out;
                            ret = deflate(&strm, flush);    /* no bad return value */
                            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
                            have = CHUNK - strm.avail_out;
                            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                                (void)deflateEnd(&strm);
                                return Z_ERRNO;
                            }
                        } while (strm.avail_out == 0);
                        assert(strm.avail_in == 0);     /* all input will be used */

                        /* done when last data in file processed */
                    } while (flush != Z_FINISH);
                    assert(ret == Z_STREAM_END);        /* stream will be complete */

                    /* clean up and return */
                    (void)deflateEnd(&strm);
                    return Z_OK;
                }

            void saveAsImageSequence(std::string directory){
            	int _listSize = diffChunks.size();
            	std::ofstream _diffFile;

            	_diffFile.open((directory + "/Output.txt").c_str(),
            			std::ios::out | std::ios::app);

            	int imageCount = 0;
            	for (int i = 0; i < _listSize; i++){
            		DiffChunk _chunk = diffChunks[i];
            		std::string filename("Img");
            		filename = filename.append(std::to_string(imageCount)).append(".jpg");

            	/*	_diffFile << _chunk.diffData.op << ";" << filename << std::endl;

            		if (_chunk.diffData.op == DO_Add){
            			_chunk.diffData.v2_Image->save(directory + "/" + filename);
            			imageCount++;
            		} else if (_chunk.diffData.op == DO_Change){
            			_chunk.diffData.v2_Image->save(directory + "/" + filename);
            			imageCount++;
            		}*/
            	}

            	_diffFile.close();
            }

            /*void save4(std::string filename){
            	int _listSize = diffChunks.size();
            	VIMUFF::Video* v1;
            	std::vector<VIMUFF::ImagePtr> _images;

            	for (int i = 0; i < _listSize; i++){
            		DiffChunk _chunk = diffChunks[i];

            		if (_chunk.diffData.op == DO_Add){
            			_images.push_back(_chunk.diffData.v2_Image);
            		} else if (_chunk.diffData.op == DO_Change){
            			_images.push_back(_chunk.diffData.v2_Image);
            		}
            	}

            	v1 = VIMUFF::Video::loadFromImages(_images, base_width, base_height, AV_CODEC_ID_H264, AV_PIX_FMT_RGBA, AV_PIX_FMT_RGB24,
            			1662045, (AVRational){1, 60}, 12, 0, "mp4");

            	v1.Save(filename.c_str());

            }*/

            void save(std::string filename){
            	// Create a temporary file
            	boost::filesystem::path temp = boost::filesystem::temp_directory_path() /
            			boost::filesystem::unique_path();

                std::ofstream _diffFile;

                _diffFile.open(filename.c_str(),
                               std::ios::binary | std::ios::out | std::ios::app);
                write(_diffFile);

                _diffFile.flush();
                _diffFile.close();

                // Zip the file
                //FILE* input = fopen(temp.c_str(), "r");
                //FILE* output = fopen(filename.c_str(), "w+");
                //compressFile(input, output, 9);
                //fclose(input);
                //fclose(output);
            }

            void save2(std::string filename){
            	boost::filesystem::path temp = boost::filesystem::temp_directory_path() /
            			boost::filesystem::unique_path();

            	std::ofstream _diffFile;

            	_diffFile.open(temp.c_str(),
            	std::ios::binary | std::ios::out | std::ios::app);


            	DiffSaveData diffSaveData;
				transformToSave(diffSaveData);
            	int _idxs_size = diffSaveData.indexes.size();
            	int _ops_size = diffSaveData.ops.size();
            	int _hashes_size = diffSaveData.hashes.size();
            	int _imgs_size = diffSaveData.images.size();

            	_diffFile.write((char*)&v1_fps, sizeof(int));
            	_diffFile.write((char*)&v2_fps, sizeof(int));
            	_diffFile.write((char*)&base_width, sizeof(int));
            	_diffFile.write((char*)&base_height, sizeof(int));
            	_diffFile.write((char*)&_idxs_size, sizeof(int));
            	_diffFile.write((char*)&_ops_size, sizeof(int));
            	_diffFile.write((char*)&_hashes_size, sizeof(int));
            	_diffFile.write((char*)&_imgs_size, sizeof(int));

            	_diffFile.write((char*)&diffSaveData.indexes[0], _idxs_size * sizeof(int));
            	_diffFile.write((char*)&diffSaveData.ops[0], _ops_size * sizeof(int));
            	_diffFile.write((char*)&diffSaveData.hashes[0], _hashes_size * sizeof(Hash::MD5Hash));

            	int currentImageIndex = 0;
            	int frameSizeInBytes = sizeof(uchar) * base_width * base_height * 4;
            	uchar* images = (uchar*) malloc(frameSizeInBytes * B_FRAME_SIZE);
            	uchar* imagesResult = (uchar*) malloc(frameSizeInBytes * B_FRAME_SIZE);

            	/*while (currentImageIndex < _imgs_size){

            		int amount = std::min(10, _imgs_size - currentImageIndex);

            		for (int i = 0; i < amount; i++, currentImageIndex++)
            			memcpy(&images[frameSizeInBytes * i], diffSaveData.images[currentImageIndex]->getData(), frameSizeInBytes);
#ifdef VIMUFF_GPU
            		VIMUFF::ImageRegister::ProcessImageSequenceDiffGPU(images, imagesResult, base_width, base_height, amount);
#else
                    VIMUFF::ImagePtr img2 = GEMUFF::VIMUFF::ImageRegister::ProcessCPUPatch(img1, diff);
#endif



            		_diffFile.write((char*)imagesResult, frameSizeInBytes * amount);
            	}

            	free(images);
            	free(imagesResult);

            	_diffFile.close();
            	boost::filesystem::copy_file(temp, boost::filesystem::path(filename.c_str()),
            			boost::filesystem::copy_option::overwrite_if_exists);*/

            }

            void transformToSave(DiffSaveData& _result){

            /*	int _listSize = diffChunks.size();

            	for (int i = 0; i < _listSize; i++){
            		DiffChunk _chunk = diffChunks[i];

            		_result.indexes.push_back(_chunk.index);
            		_result.ops.push_back(_chunk.diffData.op);
            		if (_chunk.diffData.op == DO_Add){
            			_result.images.push_back(_chunk.diffData.v2_Image);
            		} else if (_chunk.diffData.op == DO_Remove){
            			_result.hashes.push_back(_chunk.diffData.v2_HashPtr);
            		} else if (_chunk.diffData.op == DO_Change){
            			_result.images.push_back(_chunk.diffData.v2_Image);
            			_result.hashes.push_back(_chunk.diffData.v1_HashPtr);
            		}
            	}*/
            }

            void transformToLoad(DiffSaveData& _data){

            	/*int currentHash = 0; int currentImage = 0;

            	for (int i = 0; i < _data.indexes.size(); i++){
            		DiffChunk _chunk;
            		_chunk.index = _data.indexes[i];
            		_chunk.diffData.op = _data.ops[i];

            		if (_chunk.diffData.op == DO_Add){
            			_chunk.diffData.v2_Image = _data.images[currentImage++];
            		} else if (_chunk.diffData.op == DO_Remove){
            			_chunk.diffData.v2_HashPtr = _data.hashes[currentHash++];
            		} else if (_chunk.diffData.op == DO_Change){
            			_chunk.diffData.v2_Image = _data.images[currentImage++];
            			_chunk.diffData.v1_HashPtr = _data.hashes[currentHash++];
            		}

            		diffChunks.push_back(_chunk);
            	}*/
            }

            void read2(std::string filename){

            	/*DiffSaveData _read;

            	std::ifstream _diffFile;
            	int _idxs_size, _ops_size, _hashes_size, _imgs_size;


            	_diffFile.open(filename.c_str(), std::ios::binary | std::ios::app | std::ios::out);
            	_diffFile.seekg(0, std::ios::beg);



            	_diffFile.read((char*)&v1_fps, sizeof(int));
            	_diffFile.read((char*)&v2_fps, sizeof(int));
            	_diffFile.read((char*)&base_width, sizeof(int));
            	_diffFile.read((char*)&base_height, sizeof(int));
            	_diffFile.read((char*)&_idxs_size, sizeof(int));
            	_diffFile.read((char*)&_ops_size, sizeof(int));
            	_diffFile.read((char*)&_hashes_size, sizeof(int));
            	_diffFile.read((char*)&_imgs_size, sizeof(int));

            	int frameSizeInBytes = sizeof(uchar) * base_width * base_height * 4;
            	int _idx[_idxs_size];
            	DiffOperation _ops[_ops_size];
            	Hash::MD5Hash* _hashes = malloc(sizeof(Hash::MD5Hash) * _hashes_size);

            	uchar* _imageKeyed = malloc(frameSizeInBytes * B_FRAME_SIZE);
            	uchar* _imageFinal = malloc(frameSizeInBytes * B_FRAME_SIZE);

            	_diffFile.read((char*)&_idx[0], _idxs_size * sizeof(int));
            	_diffFile.read((char*)&_ops[0], _ops_size * sizeof(int));
            	_diffFile.read((char*)&_hashes[0], _hashes_size * sizeof(Hash::MD5Hash));


            	int currentImageIndex = 0;
              	while (currentImageIndex < _imgs_size){
              		int amount = std::min(10, _imgs_size - currentImageIndex);

              		_diffFile.read((char*)_imageKeyed, frameSizeInBytes * amount);

              		VIMUFF::ImageRegister::ProcessImageSequencePatchGPU(
              				_imageKeyed, _imageFinal, base_width, base_height, amount);

              		for (int i = 0; i < amount; i++, currentImageIndex++){
              			VIMUFF::ImagePtr image(
              					new VIMUFF::Image(base_width, base_height, CV_8UC4, &_imageFinal[i * frameSizeInBytes]));

              			_read.images.push_back(image);
              		}
              	}

              	for (int i = 0; i < _idxs_size; i++) _read.indexes.push_back(_idx[i]);
              	for (int i = 0; i < _ops_size; i++) _read.ops.push_back(_ops[i]);
              	for (int i = 0; i < _hashes_size; i++) _read.hashes.push_back(Hash::AbstractHashPtr(_hashes[i]));

              	transformToLoad(_read);

              	free(_imageKeyed);
              	free(_imageFinal);
              	_diffFile.close();*/
            }


            void write(std::ofstream &_ofstream){
                int _listSize = diffChunks.size();
            	fprintf(stderr, "Delta size: %d.\n", _listSize);
            	_ofstream.write((char*)&_listSize, sizeof(int));
                _ofstream.write((char*)&v1_fps, sizeof(int));
                _ofstream.write((char*)&v2_fps, sizeof(int));
                _ofstream.write((char*)&base_width, sizeof(int));
                _ofstream.write((char*)&base_height, sizeof(int));

                for (int i = 0; i < _listSize; i++){
                	DiffChunk _chunk = diffChunks[i];

                	int _listDiffDataSize = diffChunks[i].diffData.size();
                	_ofstream.write((char*)&_listDiffDataSize, sizeof(int));

                	bool isValid = _chunk.lcsEntry.isValid();
                	_ofstream.write((char*)&isValid, sizeof(bool));

                	if (isValid){
                		_chunk.lcsEntry.l1_ref->writeKey(_ofstream);
                		_chunk.lcsEntry.l2_ref->writeKey(_ofstream);
                	}

                    for (int j = 0; j < _listDiffDataSize; j++){
                    	DiffData _da = _chunk.diffData[j];

                    	_ofstream.write((char*)&_da.op, sizeof(int));
                    	const unsigned char *img1 = NULL; int img1_width = 0, img1_height = 0;
                    	const unsigned char *img2 = NULL; int img2_width = 0, img2_height = 0;

                    	if (_da.v1_Image){
                    		img1 = (uchar*) _da.v1_Image->getData();
                    		img1_width = _da.v1_Image->getWidth();
                    		img1_height = _da.v1_Image->getHeight();
                    	}


                    	if (_da.v2_Image){
                    		img2 = (uchar*) _da.v2_Image->getData();
                    		img2_width = _da.v2_Image->getWidth();
                    		img2_height = _da.v2_Image->getHeight();
                    	}

                    	_ofstream.write((char*)&img1_width, sizeof(int));
                    	_ofstream.write((char*)&img1_height, sizeof(int));
                    	_ofstream.write((char*)img1, sizeof(unsigned char) *
                                    img1_width * img1_height * 4);
                    	_ofstream.write((char*)&img2_width, sizeof(int));
                    	_ofstream.write((char*)&img2_height, sizeof(int));
                    	_ofstream.write((char*)img2, sizeof(unsigned char) *
                                    img2_width * img2_height * 4);

                    	bool v1HasHash = false;
                    	bool v2HasHash = false;

                    	if (_da.v1_HashPtr != NULL) v1HasHash = true;
                    	if (_da.v2_HashPtr != NULL) v2HasHash = true;


                    	_ofstream.write((char*)&v1HasHash, sizeof(bool));
                    	_ofstream.write((char*)&v2HasHash, sizeof(bool));

                    	if (v1HasHash)_da.v1_HashPtr->writeKey(_ofstream);


                    	if (v2HasHash)_da.v2_HashPtr->writeKey(_ofstream);
                    }
                }
            }

            void read(std::ifstream &_ifstream){

                _ifstream.seekg(0, std::ios::beg);
            	fprintf(stderr, "Reading delta file.\n");

                int _listSize = 0;
                int _listDataSize = 0;

                _ifstream.read((char*)&_listSize, sizeof(int));
                _ifstream.read((char*)&v1_fps, sizeof(int));
                _ifstream.read((char*)&v2_fps, sizeof(int));
                _ifstream.read((char*)&base_width, sizeof(int));
                _ifstream.read((char*)&base_height, sizeof(int));


            	fprintf(stderr, "Delta size: %d.\n", _listSize);

                for (int i = 0; i < _listSize; i++){

                    DiffChunk _c;
                    _ifstream.read((char*)&_listDataSize, sizeof(int));

                	bool isValid; _ifstream.read((char*)&isValid, sizeof(bool));

                	if (isValid){
                		Hash::MD5Hash *l1_ref = new Hash::MD5Hash;
                		Hash::MD5Hash *l2_ref = new Hash::MD5Hash;
                		l1_ref->readKey(_ifstream);
                		l2_ref->readKey(_ifstream);
                		_c.lcsEntry.l1_ref.reset(l1_ref);
                		_c.lcsEntry.l2_ref.reset(l2_ref);
                	}

                    for (int j = 0; j < _listDataSize; j++){
                    	DiffData _da;
                    	_ifstream.read((char*)&_da.op, sizeof(int));

                    	unsigned char *img1 = NULL; int img1_width = 0, img1_height = 0;
                    	unsigned char *img2 = NULL; int img2_width = 0, img2_height = 0;

                    	_ifstream.read((char*)&img1_width, sizeof(int));
                    	_ifstream.read((char*)&img1_height, sizeof(int));

                    	if (img1_width > 0 || img1_height > 0){
                    		img1 = (unsigned char*) malloc(sizeof(unsigned char) *
                    				img1_width * img1_height * 4);
                    		_ifstream.read((char*)img1, sizeof(unsigned char) *
                                    img1_width * img1_height * 4);
                    	}

                    	_ifstream.read((char*)&img2_width, sizeof(int));
                    	_ifstream.read((char*)&img2_height, sizeof(int));

                    	if (img2_width > 0 || img2_height > 0){
                    		img2 = (unsigned char*) malloc(sizeof(unsigned char) *
                    				img2_width * img2_height * 4);
                    		_ifstream.read((char*)img2, sizeof(unsigned char) *
                                    img2_width * img2_height * 4);
                    	}

                    	if (img1){
                    		VIMUFF::Image *vImg = new VIMUFF::Image(img1_width, img1_height, CV_8UC4, img1);
                    		_da.v1_Image.reset(vImg);
                    		free(img1);
                    	}

                    	if (img2){
                    		VIMUFF::Image*vImg = new VIMUFF::Image(img2_width, img2_height, CV_8UC4, img2);
                    		_da.v2_Image.reset(vImg);
                    		free(img2);
                    	}

                    	bool v1HasHash, v2HasHash;
                    	_ifstream.read((char*)&v1HasHash, sizeof(bool));
                    	_ifstream.read((char*)&v2HasHash, sizeof(bool));

                    	if (v1HasHash){
                    		Hash::MD5Hash *v1 = new Hash::MD5Hash;
                    		v1->readKey(_ifstream);
                    		_da.v1_HashPtr.reset(v1);
                    	}

                    	if (v2HasHash){
                    		Hash::MD5Hash *v2 = new Hash::MD5Hash;
                    		v2->readKey(_ifstream);
                    		_da.v2_HashPtr.reset(v2);
                    	}

                    	_c.diffData.push_back(_da);
                    }

                    diffChunks.push_back(_c);
                }
            }


            void Debug(){
                for (int i = 0; i < diffChunks.size(); i++){
                    //fprintf(stdout, "%d op: %d\n", diffChunks[i].index, diffChunks[i].diffData.op);
                }
            }

            void Summary(){
               /* int changes = 0;
                int added = 0;
                int removed = 0;
                for (int i = 0; i < diffChunks.size(); i++){
                    if (diffChunks[i].diffData.op == Diff::DO_Add) added++;
                    if (diffChunks[i].diffData.op == Diff::DO_Remove) removed++;
                    if (diffChunks[i].diffData.op == Diff::DO_Change) changes++;
                }

                fprintf(stderr, "Added: %d | Removed: %d | Changed: %d\n", added, removed, changes);;*/
            }

        };

        struct Diff3Info {
            int vbase_fps;
            int v1_fps;
            int v2_fps;
            int base_width;
            int base_height;
            std::vector<Diff3Chunk> diff3Chunks;
            std::vector<DiffChunk> diffBaseToV1;
            std::vector<DiffChunk> diffBaseToV2;

        };

        /* ***************** Functions *********************** */
        GEMUFFLIB_EXPORT Diff2Info Diff2(GEMUFF::VIMUFF::VideoPtr v1, GEMUFF::VIMUFF::VideoPtr v2, float threshold);

        GEMUFFLIB_EXPORT VIMUFF::Video* Patch(Diff2Info &_diff2, VIMUFF::VideoPtr _v1);

        GEMUFFLIB_EXPORT Diff3Info Diff3(GEMUFF::VIMUFF::VideoPtr vbase, GEMUFF::VIMUFF::VideoPtr v1,
                        GEMUFF::VIMUFF::VideoPtr v2, float threshold);

        GEMUFFLIB_EXPORT void Process2Diff(std::vector<LCSEntry>& _lcs, std::vector<DiffChunk> &_diffChunks,
                          std::vector<Hash::AbstractHashPtr> &_seq1, std::vector<Hash::AbstractHashPtr> &_seq2,
                          float threshold, int res_w, int res_h);

        GEMUFFLIB_EXPORT void AddDiffData(std::vector<DiffData> &_diffData, VIMUFF::ImagePtr _img1,
                      Hash::MD5HashPtr hash_v1, VIMUFF::ImagePtr _img2, Hash::MD5HashPtr hash_v2,
                      DiffOperation _op);

        GEMUFFLIB_EXPORT int FindKey(Hash::AbstractHashPtr _key, VECTOR_ABSTRACT_HASH_PTR seq, int startIndex, float threshold);
    }


}


#endif // DIFFALGORITHMS_H
