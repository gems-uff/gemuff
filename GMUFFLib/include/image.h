#ifndef IMAGE_H
#define IMAGE_H

#include "GEMUFFLib_global.h"
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudafilters.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/cudaimgproc.hpp>
//#include<opencv/cv.h>
#include <boost/shared_ptr.hpp>
//#include <opencv/highgui.h>
#include <iostream>
#include <iomanip>
#include <System.h>

#define THUMBNAIL_WIDTH  32
#define THUMBNAIL_HEIGHT 32
//#define USE_GPU



namespace GEMUFF {
    namespace VIMUFF {

        class GEMUFFLIB_EXPORT Image
        {
        private:
        	Image(){ buffer = NULL; }

        public:
            cv::Mat image_data;
            void* buffer;

        public:
            Image(int width, int height, int type);
            Image(int width, int height, int type, void* data);
            ~Image(){
                if (buffer != NULL){
                	free(buffer);
                	Util::System::informAllocated(getType() * getWidth() * getHeight());
                }
            }



            static Image* loadFromFile(char* filename);
            void save(std::string path);
            cv::Mat& getOpenCVMat(){ return image_data; }
            int getWidth(){ return image_data.cols; }
            int getHeight(){ return image_data.rows; }
            int getChannels(){ return image_data.channels(); }
            int getStride(){ return image_data.step; }
            int getType(){ return image_data.type(); }
            void* getData(){ if (buffer != NULL) return buffer; return image_data.data; }
            float sum(){ return cv::sum(image_data)[0]; }


            static Image* grayBlurredEqualizedBatch(std::vector< boost::shared_ptr<Image> > img,
                float sizeX, float sizeY);

            Image* toGray();

            Image* resize(int width, int height);

            Image* blur(float sizeX, float sizeY);

            Image* equalize();

            Image* dct();

            Image* correlateNormalized(int width, int height, float *kernel);

            Image* subImage(int x, int y, int width, int height);

            Image* subImageSum(int w, int h, int strideX, int strideY,
                               int blockSizeX, int blockSizeY);

            Image* grayBlurredEqualized(float sizeX, float sizeY);

            Image* convertTo(int type);

            double mean();

            Image* getGrayResizedThumb(){

            	Image* gray = this->toGray();
            	Image* thumb = gray->resize(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);
            	Image* grayScaleThumb = thumb->convertTo(CV_32F);

            	delete gray;
            	delete thumb;

                return grayScaleThumb;
            }

            void setData(void* data);

            static int getPixelSize(int type);

        };


        typedef boost::shared_ptr<Image> ImagePtr;
    }
}


#endif // IMAGE_H
