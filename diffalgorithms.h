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

        struct DiffData {
            DiffOperation op;
            VIMUFF::ImagePtr v1_Image;
            VIMUFF::ImagePtr v2_Image;
        };

        struct DiffChunk {
            int index;
            DiffData diffData;
        };


        struct Diff3Chunk {
            int index;
            std::vector<DiffData> basev1;
            std::vector<DiffData> basev2;
        };



        struct Diff2Info {

            int v1_fps;
            int v2_fps;
            int base_width;
            int base_height;

            std::vector<DiffChunk> diffChunks;

            void write(std::ofstream &_ofstream){
                int _listSize = diffChunks.size();

                _ofstream.write((char*)&v1_fps, sizeof(int));
                _ofstream.write((char*)&v2_fps, sizeof(int));
                _ofstream.write((char*)&base_width, sizeof(int));
                _ofstream.write((char*)&base_height, sizeof(int));
                _ofstream.write((char*)&_listSize, sizeof(int));

                for (int i = 0; i < _listSize; i++){
                    DiffChunk _chunk = diffChunks[i];

                    _ofstream.write((char*)&_chunk.diffData.op, sizeof(int));
                    _ofstream.write((char*)&_chunk.index, sizeof(int));

                    const unsigned char *img1 = NULL; int img1_width = 0, img1_height = 0;
                    const unsigned char *img2 = NULL; int img2_width = 0, img2_height = 0;

                    if (_chunk.diffData.v1_Image){
                        img1 = _chunk.diffData.v1_Image->data();
                        img1_width = _chunk.diffData.v1_Image->getWidth();
                        img1_height = _chunk.diffData.v1_Image->getHeight();
                    }

                    if (_chunk.diffData.v2_Image){
                        img2 = _chunk.diffData.v2_Image->data();
                        img2_width = _chunk.diffData.v2_Image->getWidth();
                        img2_height = _chunk.diffData.v2_Image->getHeight();
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
                    DiffChunk _c;

                    _ifstream.read((char*)&_c.diffData.op, sizeof(int));
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
                        _c.diffData.v1_Image.reset(vImg);
                        free(img1);
                    }

                    if (img2){
                        VIMUFF::Image *vImg = new VIMUFF::Image();
                        vImg->setData(img2, img2_width, img2_height, 4);
                        _c.diffData.v2_Image.reset(vImg);
                        free(img2);
                    }

                    diffChunks.push_back(_c);
                }
            }


            void Debug(){
                for (int i = 0; i < diffChunks.size(); i++){
                    qDebug() << diffChunks[i].index << "op: " << diffChunks[i].diffData.op;
                }
            }

        };

        struct Diff3Info {
            int vbase_fps;
            int v1_fps;
            int v2_fps;
            int base_width;
            int base_height;
            std::vector<Diff3Chunk> diff3chunks;
            std::vector<LCSEntry> lcs;
        };

        /* ***************** Functions *********************** */
        Diff2Info Diff2(GEMUFF::VIMUFF::Video *v1, GEMUFF::VIMUFF::Video *v2, float threshold);

        VIMUFF::Video Patch(Diff2Info &_diff2, VIMUFF::Video *_v1);

        Diff3Info Diff3(GEMUFF::VIMUFF::Video *vbase, GEMUFF::VIMUFF::Video *v1,
                        GEMUFF::VIMUFF::Video *v2, float threshold);

        void Process2Diff(std::vector<LCSEntry> &_lcs, std::vector<DiffChunk> &_diffChunks,
                          std::vector<Hash::AbstractHashPtr> &_seq1, std::vector<Hash::AbstractHashPtr> &_seq2,
                          float threshold);

        void AddChunk(std::vector<DiffChunk> &_diffChunks, VIMUFF::ImagePtr _img1,
                      VIMUFF::ImagePtr _img2, DiffOperation _op, int _index);

        int FindKey(Hash::AbstractHashPtr _key, VECTOR_ABSTRACT_HASH_PTR seq, int startIndex, float threshold);
    }
}


#endif // DIFFALGORITHMS_H
