#include "imageregister.h"

std::map<GEMUFF::Hash::AbstractHashPtr, GEMUFF::VIMUFF::ImagePtr> GEMUFF::VIMUFF::ImageRegister::m_Frames;

namespace GEMUFF
{

    namespace VIMUFF
    {
        Hash::AbstractHashPtr ImageRegister::RegisterFrame(unsigned char* buffer, int width, int height, int bpp)
        {
            Hash::AbstractHashPtr _hash;
            VIMUFF::ImagePtr image(new VIMUFF::Image());
            image->setData(buffer, width, height, bpp);

            if (0)
                _hash.reset(Hash::MarrHildretchHash::GenerateHash(
                        image));
            else
                _hash.reset(Hash::MD5Hash::GenerateHash(
                        image));


            // Verificar se a imagem existe
            if (m_Frames.find(_hash) == m_Frames.end())
                m_Frames[_hash] = image;

            return _hash;
        }

        VIMUFF::ImagePtr ImageRegister::ImageAt(Hash::AbstractHashPtr _hash)
        {
            VIMUFF::ImagePtr res;

            if (m_Frames.find(_hash) != m_Frames.end())
                res = ImageRegister::m_Frames[_hash];

            return res;
        }

        void ImageRegister::Debug(){
            std::map<Hash::AbstractHashPtr, VIMUFF::ImagePtr>::iterator iter;

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
