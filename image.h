#ifndef IMAGE_H
#define IMAGE_H

#include <opencv2/core/core.hpp>
#include <opencv2/gpu/gpu.hpp>
#include<opencv/cv.h>
#include <boost/shared_ptr.hpp>
#include <QImage>
#include <opencv/highgui.h>
#include <QDebug>

#define THUMBNAIL_WIDTH  32
#define THUMBNAIL_HEIGHT 32
//#define USE_GPU

namespace GEMUFF {
    namespace VIMUFF {

        class Image
        {
        public:
            cv::Mat image_data;
            boost::shared_ptr<Image> gray_scale_thumbnail;
            uchar* buffer;

        public:
            Image();
            ~Image(){
                if (buffer != NULL) free(buffer);
            }

            void loadFromFile(char* filename);

            int getWidth(){ return image_data.cols; }
            int getHeight(){ return image_data.rows; }
            int getChannels(){ return image_data.channels(); }
            int getStride(){ return image_data.step; }
            unsigned char* getData(){ return (unsigned char*) image_data.data; }


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


            double mean();

            const uchar* data() const {
                return image_data.ptr();
            }

            boost::shared_ptr<Image> getGrayResizedThumb(){
                return gray_scale_thumbnail;
            }

            QImage toQImage();

            void setData(unsigned char* data, int width, int height, int bpp);

        };


        typedef boost::shared_ptr<Image> ImagePtr;
    }
}


#endif // IMAGE_H
