#ifndef IMAGEREGISTER_H
#define IMAGEREGISTER_H


#include <string>
#include <map>
#include "hash.h"
#include "K_IMUFF.h"
#include "image.h"

namespace GEMUFF
{
    namespace VIMUFF
    {

        struct ImageData {
            Hash::AbstractHashPtr hash;
            ImagePtr image;

            ImageData(){}

            ImageData(Hash::AbstractHashPtr _hash, ImagePtr _image){
                hash = _hash;
                image = _image;
            }
        };

        class GEMUFFLIB_EXPORT ImageRegister
        {
        public:
            static Hash::AbstractHashPtr RegisterFrame(int width, int height, int type, Hash::HashType hashType, void* buffer);
            static Hash::AbstractHashPtr RegisterFrame(VIMUFF::ImagePtr image, Hash::HashType hashType);
            static VIMUFF::ImagePtr ImageAt(Hash::AbstractHashPtr _hash);
            static VIMUFF::ImagePtr ProcessDIFF(ImagePtr _image1, ImagePtr _image2, Device device);
            static void ProcessGPUDiffD(uchar* elem1, uchar* elem2, uchar* result, int _size);
            static void ProcessCPUDiffD(uchar* elem1, uchar* elem2, uchar* result, int _size);
            static void ProcessGPUPatch(uchar* img1, uchar* img2, uchar* result, int size);
            static void ProcessCPUPatch(uchar* img1, uchar* img2, uchar* result, int size);
            static ImagePtr ProcessCPUPatch(ImagePtr img1, ImagePtr img2);
            static ImagePtr ProcessGPUPatch(ImagePtr img1, ImagePtr img2);
            static void ProcessImageSequenceDiffGPU(uchar* images, uchar* imagesResult, int frame_w, int frame_h, int amount);
            static void Debug();

        private:
            static std::map<std::string,ImageData> m_Frames;
        };
    }
}


#endif // IMAGEREGISTER_H
