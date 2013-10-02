#ifndef IMAGEREGISTER_H
#define IMAGEREGISTER_H

#include <QImage>
#include <string>
#include <map>
#include <QDebug>
#include "hash.h"
#include "K_IMUFF.h"

namespace GEMUFF
{
    namespace VIMUFF
    {

        class ImageRegister
        {
        public:
            static Hash::AbstractHashPtr RegisterFrame(QImage image);
            static QImage* ImageAt(Hash::AbstractHashPtr _hash);
            static QImage ProcessGPUDiff(QImage* img1, QImage* img2);
            static void Debug();

        private:
            static std::map<Hash::AbstractHashPtr,QImage> m_Frames;
        };
    }
}


#endif // IMAGEREGISTER_H
