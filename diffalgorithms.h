#ifndef DIFFALGORITHMS_H
#define DIFFALGORITHMS_H

#include <vector>
#include <string>
#include "helperfunctions.h"
#include "video.h"

namespace GEMUFF {
    namespace Diff {


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

        struct Diff2Chunk {
            DiffOperation op;
            int index;
            //Hash::AbstractHashPtr before[NUM_BORDER];
            //Hash::AbstractHashPtr after[NUM_BORDER];
            VIMUFF::ImagePtr v1_Image;
            VIMUFF::ImagePtr v2_Image;
        };

        struct Diff2Info {

            int v1_fps;
            int v2_fps;
            int base_width;
            int base_height;

            std::vector<Diff2Chunk> diff2Chunks;

            void write(std::ofstream &_ofstream){
                int _listSize = diff2Chunks.size();

                _ofstream.write((char*)&v1_fps, sizeof(int));
                _ofstream.write((char*)&v2_fps, sizeof(int));
                _ofstream.write((char*)&base_width, sizeof(int));
                _ofstream.write((char*)&base_height, sizeof(int));
                _ofstream.write((char*)&_listSize, sizeof(int));

                for (int i = 0; i < _listSize; i++){
                    Diff2Chunk _chunk = diff2Chunks[i];

                    _ofstream.write((char*)&_chunk.op, sizeof(int));
                    _ofstream.write((char*)&_chunk.index, sizeof(int));

                    const unsigned char *img1 = NULL; int img1_width = 0, img1_height = 0;
                    const unsigned char *img2 = NULL; int img2_width = 0, img2_height = 0;

                    if (_chunk.v1_Image){
                        img1 = _chunk.v1_Image->data();
                        img1_width = _chunk.v1_Image->getWidth();
                        img1_height = _chunk.v1_Image->getHeight();
                    }

                    if (_chunk.v2_Image){
                        img2 = _chunk.v2_Image->data();
                        img2_width = _chunk.v2_Image->getWidth();
                        img2_height = _chunk.v2_Image->getHeight();
                    }

                    _ofstream.write((char*)&img1_width, sizeof(int));
                    _ofstream.write((char*)&img1_height, sizeof(int));
                    _ofstream.write((char*)img1, sizeof(unsigned char) *
                                    img1_width * img1_height * 4);
                    _ofstream.write((char*)&img2_width, sizeof(int));
                    _ofstream.write((char*)&img2_height, sizeof(int));
                    _ofstream.write((char*)img2, sizeof(unsigned char) *
                                    img2_width * img2_height * 4);

                }
            }

            void read(std::ifstream &_ifstream){

                _ifstream.seekg(0, std::ios::beg);

                int _listSize = 0;
                _ifstream.read((char*)&v1_fps, sizeof(int));
                _ifstream.read((char*)&v2_fps, sizeof(int));
                _ifstream.read((char*)&base_width, sizeof(int));
                _ifstream.read((char*)&base_height, sizeof(int));
                _ifstream.read((char*)&_listSize, sizeof(int));

                for (int i = 0; i < _listSize; i++){
                    Diff2Chunk _c;

                    _ifstream.read((char*)&_c.op, sizeof(int));
                    _ifstream.read((char*)&_c.index, sizeof(int));

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
                        VIMUFF::Image *vImg = new VIMUFF::Image();
                        vImg->setData(img1, img1_width, img1_height, 4);
                        _c.v1_Image.reset(vImg);
                        free(img1);
                    }

                    if (img2){
                        VIMUFF::Image *vImg = new VIMUFF::Image();
                        vImg->setData(img2, img2_width, img2_height, 4);
                        _c.v2_Image.reset(vImg);
                        free(img2);
                    }

                    diff2Chunks.push_back(_c);
                }
            }


            void Debug(){
                for (int i = 0; i < diff2Chunks.size(); i++){
                    qDebug() << diff2Chunks[i].index << "op: " << diff2Chunks[i].op;
                }
            }

        };

        struct Diff3Info {
            int vbase_fps;
            int v1_fps;
            int v2_fps;
            int base_width;
            int base_height;
            std::vector<Diff2Chunk> base_v1;
            std::vector<Diff2Chunk> base_v2;

            void AddChunkBaseV1(VIMUFF::ImagePtr _img1, VIMUFF::ImagePtr _img2, DiffOperation _op, int _index){
                Diff2Chunk chunk;
                chunk.op = _op;
                chunk.index = _index;

                chunk.v1_Image = _img1;
                chunk.v2_Image = _img2;
                base_v1.push_back(chunk);
            }

            void AddChunkBaseV2(VIMUFF::ImagePtr _img1, VIMUFF::ImagePtr _img2, DiffOperation _op, int _index){
                Diff2Chunk chunk;
                chunk.op = _op;
                chunk.index = _index;

                chunk.v1_Image = _img1;
                chunk.v2_Image = _img2;
                base_v2.push_back(chunk);
            }
        };

        /* ***************** Functions *********************** */
        Diff2Info Diff2(GEMUFF::VIMUFF::Video *v1, GEMUFF::VIMUFF::Video *v2, float threshold);

        VIMUFF::Video Patch(Diff2Info &_diff2, VIMUFF::Video *_v1);

        Diff3Info Diff3(GEMUFF::VIMUFF::Video *vbase, GEMUFF::VIMUFF::Video *v1,
                        GEMUFF::VIMUFF::Video *v2, float threshold);

        void Process2Diff(std::vector<LCSEntry> &_lcs, std::vector<Diff2Chunk> &_diff2Chunks,
                          std::vector<Hash::AbstractHashPtr> &_seq1, std::vector<Hash::AbstractHashPtr> &_seq2,
                          float threshold);

        void AddChunk(std::vector<Diff2Chunk> &_diff2Chunks, VIMUFF::ImagePtr _img1,
                      VIMUFF::ImagePtr _img2, DiffOperation _op, int _index);
    }
}


#endif // DIFFALGORITHMS_H
