#include "imageregister.h"

std::map<GEMUFF::Hash::AbstractHashPtr, QImage> GEMUFF::VIMUFF::ImageRegister::m_Frames;

namespace GEMUFF
{

    namespace VIMUFF
    {
        Hash::AbstractHashPtr ImageRegister::RegisterFrame(QImage image)
        {
            Hash::AbstractHashPtr _hash;

            if (0)
                _hash.reset(Hash::MarrHildretchHash::GenerateHash(
                            image.constBits(),
                            image.width(),
                            image.height(), 3));
            else
                _hash.reset(Hash::MD5Hash::GenerateHash(
                        image.constBits(),
                        image.width() * image.height() * 3));


            // Verificar se a imagem existe
            if (m_Frames.find(_hash) == m_Frames.end())
                m_Frames[_hash] = image;

            return _hash;
        }

        QImage* ImageRegister::ImageAt(Hash::AbstractHashPtr _hash)
        {
            if (m_Frames.find(_hash) != m_Frames.end())
                return &ImageRegister::m_Frames[_hash];

            return NULL;
        }

        void ImageRegister::Debug(){
            std::map<Hash::AbstractHashPtr, QImage>::iterator iter;

            for (iter = m_Frames.begin(); iter != m_Frames.end(); iter++){
                qDebug() << iter->first->toString().c_str() << "\n";
            }
        }

        QImage ImageRegister::ProcessGPUDiff(QImage* img1, QImage* img2){

            // Processar a diferenca
            uchar* delta = (uchar*) malloc(sizeof(uchar) * img1->width() * img1->height() * 4);
            gIMUFFDiff(img1->constBits(), img2->constBits(), delta, img1->width() * img1->height());
            QImage img_c =  QImage(delta, img1->width(), img1->height(), QImage::Format_RGB32);

            free(delta);
            return img_c;

        }
    }
}
