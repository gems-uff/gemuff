#include "imageregister.h"

std::map<std::string,GEMUFF::VIMUFF::ImageData> GEMUFF::VIMUFF::ImageRegister::m_Frames;

namespace GEMUFF
{

    namespace VIMUFF
    {
        Hash::AbstractHashPtr ImageRegister::RegisterFrame(VIMUFF::ImagePtr image, Hash::HashType hashType)
        {
            return RegisterFrame(image->getWidth(), image->getHeight(), image->getType(), hashType, image->getData());
        }

        Hash::AbstractHashPtr ImageRegister::RegisterFrame(int width, int height, int type, Hash::HashType hashType, void* buffer)
        {
            Hash::AbstractHashPtr _hash;
            VIMUFF::ImagePtr image(new VIMUFF::Image(width, height, type, buffer));

            if (hashType == Hash::HashType::T_MH)
                _hash.reset(Hash::MarrHildretchHash::GenerateHash(
                        image));
            else if (hashType == Hash::HashType::T_MD5)
                _hash.reset(Hash::MD5Hash::GenerateHash(
                        image));

            // Verificar se a imagem existe
            if (m_Frames.find(_hash->toString()) == m_Frames.end()){
                m_Frames[_hash->toString()] = ImageData(_hash, image);
            }

            return _hash;
        }

        VIMUFF::ImagePtr ImageRegister::ImageAt(Hash::AbstractHashPtr _hash)
        {
            VIMUFF::ImagePtr res;
            std::string _r = _hash->toString();
            if (m_Frames.find(_hash->toString()) != m_Frames.end())
                res = ImageRegister::m_Frames[_hash->toString()].image;

            if (res.get() == NULL)
                printf("Hash code %s returned null\n", _hash->toString().c_str());

            return res;
        }

        void ImageRegister::Debug(){
            std::map<std::string, ImageData>::iterator iter;

            for (iter = m_Frames.begin(); iter != m_Frames.end(); iter++){
                printf("%s\n", iter->second.hash->toString().c_str());
            }
        }

		void ImageRegister::ProcessImageSequenceDiffGPU(uchar* images, uchar* imagesResult, int frame_w, int frame_h, int amount){
			gIMUFFDiffSequence(images, imagesResult, frame_w, frame_h, amount);
		}

        void ImageRegister::ProcessGPUDiffD(uchar* elem1, uchar* elem2, uchar* result, int _size){
            gIMUFFDiff(elem1, elem2, result, _size);
        }

        void ImageRegister::ProcessCPUDiffD(uchar* elem1, uchar* elem2, uchar* result, int _size){

            for (int i = 0; i < _size; i++){
                result[i*4] = elem1[i*4] ^ elem2[i*4];
                result[i*4+1] = elem1[i*4+1] ^ elem2[i*4+1];
                result[i*4+2] = elem1[i*4+2] ^ elem2[i*4+2];
                result[i*4+3] = 0;
            }
        }

        void ImageRegister::ProcessGPUPatch(uchar* img1, uchar* img2, uchar* result, int size){

            gIMUFFPatch(img1, img2, result, size);
        }

        ImagePtr ImageRegister::ProcessGPUPatch(ImagePtr img1, ImagePtr img2){
        	ImagePtr res(new Image(img1->getWidth(), img2->getHeight(), CV_8UC4));

        	uchar* patched = (uchar*) malloc(sizeof(uchar) * img1->getWidth() * img1->getHeight() * 4);
        	ProcessGPUPatch((uchar*)img1->getData(), (uchar*)img2->getData(), patched, img1->getWidth() * img1->getHeight());
        	res->setData(patched);

        	free(patched);

        	return res;
        }


        ImagePtr ImageRegister::ProcessCPUPatch(ImagePtr img1, ImagePtr img2){
        	ImagePtr res(new Image(img1->getWidth(), img2->getHeight(), CV_8UC4));

        	uchar* patched = (uchar*) malloc(sizeof(uchar) * img1->getWidth() * img1->getHeight() * 4);
        	ProcessCPUPatch((uchar*)img1->getData(), (uchar*)img2->getData(), patched, img1->getWidth() * img1->getHeight());
        	res->setData(patched);
        	free(patched);

        	return res;
        }



        void ImageRegister::ProcessCPUPatch(uchar* img1, uchar* img2, uchar* result, int size){

            for (int i = 0; i < size; i++){
                result[i*4] = img1[i*4] ^ img2[i*4];
                result[i*4+1] = img1[i*4+1] ^ img2[i*4+1];
                result[i*4+2] = img1[i*4+2] ^ img2[i*4+2];
                result[i*4+3] = 0;
            }
        }

        VIMUFF::ImagePtr ImageRegister::ProcessDIFF(ImagePtr _image1, ImagePtr _image2, Device device){
        	ImagePtr res(new Image(_image1->getWidth(), _image2->getHeight(), CV_8UC4));

        	uchar* diff = (uchar*) malloc(sizeof(uchar) * res->getWidth() * res->getHeight() * 4);

        	if (device == Device::D_CPU){
        		ProcessCPUDiffD((uchar*)_image1->getData(), (uchar*)_image2->getData(), diff, res->getWidth() * res->getHeight());
        	} else {
        		ProcessGPUDiffD((uchar*)_image1->getData(), (uchar*)_image2->getData(), diff, res->getWidth() * res->getHeight());
        	}

        	res->setData(diff);
        	free(diff);
        	return res;
        }
    }
}
