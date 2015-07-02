#ifndef IMAGEREGISTER_H
#define IMAGEREGISTER_H

#include <QImage>
#include <string>
#include <map>
#include <QDebug>
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

        class ImageRegister
        {
        public:
            static Hash::AbstractHashPtr RegisterFrame(unsigned char* buffer, int width, int height, int bpp);
            static Hash::AbstractHashPtr RegisterFrame(VIMUFF::ImagePtr image);
            static VIMUFF::ImagePtr ImageAt(Hash::AbstractHashPtr _hash);
            static QImage ProcessGPUDiff(QImage* img1, QImage* img2);
            static QImage ProcessCPUDiff(QImage* img1, QImage* img2);
            static QImage ProcessGPUPatch(QImage* img1, QImage* img2);
            static QImage ProcessCPUPatch(QImage* img1, QImage* img2);
            static void Debug();
            static ImagePtr toImage(QImage *im);

        private:
            static std::map<std::string,ImageData> m_Frames;
        };
    }
}


#endif // IMAGEREGISTER_H
