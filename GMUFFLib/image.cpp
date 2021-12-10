#include "image.h"


namespace GEMUFF{
    namespace VIMUFF {

        Image::Image(int width, int height, int type) : Image(){
        	image_data = cv::Mat(height, width, type);
        }

        Image::Image(int width, int height, int type, void* data) : Image(width, height, type){
        	setData(data);
        }


        void Image::setData(void *data){
        	int pixelSize = getPixelSize(getType());
        	buffer = malloc(getWidth() * getHeight() * 4);

            memcpy(buffer, data, pixelSize * getWidth() * getHeight());
            image_data = cv::Mat(getHeight(), getWidth(), getType(), buffer);

            static bool save = true;
            if (save){
            	cv::imwrite("/home/josericardo/test1.jpg", image_data);
            	save = false;
            }

            Util::System::informAllocated(pixelSize * getWidth() * getHeight());

            //resized->image_data.convertTo(gray_scale_thumbnail->image_data, CV_32F);

            //cv::imwrite("/home/josericardo/img_gray_s.jpg", resized->image_data);
        }

        Image* Image::loadFromFile(char *filename){
        	Image *result = new Image();
            result->image_data = cv::imread(filename, cv::IMREAD_UNCHANGED);

            //ImagePtr gray(this->toGray());
            //ImagePtr resized(gray->resize(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT));

            //gray_scale_thumbnail.reset(new Image());
            //resized->image_data.convertTo(gray_scale_thumbnail->image_data, CV_32FC1);

            return result;
        }


        Image* Image::toGray(){
            Image *result = new Image();

            cv::cvtColor (image_data, result->image_data, cv::COLOR_RGBA2GRAY);

            /*static bool kd = false;
            if (kd == false){
                kd = true;
                cv::imwrite("/home/josericardo/img_gray.jpg", result->image_data);
            }*/
            return result;
        }

        Image* Image::convertTo(int type){
        	Image *result = new Image();
        	image_data.convertTo(result->image_data, type);
        	return result;
        }


        Image* Image::resize(int width, int height){
            Image *result = new Image();

            cv::resize(image_data, result->image_data, cv::Size(width, height));

            return result;
        }

        Image* Image::blur(float sizeX, float sizeY){
            Image *result = new Image();

#ifdef USE_GPU
            cv::gpu::GpuMat gpuMat(image_data);
            cv::gpu::GpuMat gpuResult;
            cv::gpu::blur(gpuMat, gpuResult, cv::Size(sizeX, sizeY));
            gpuResult.download(result->image_data);
#else
            cv::blur(image_data, result->image_data, cv::Size(sizeX, sizeY) );
#endif


            return result;
        }

        Image* Image::equalize(){
            Image *result = new Image();

            cv::equalizeHist(image_data, result->image_data);

            return result;
        }

        Image* Image::dct(){
            Image *result = new Image();

            cv::dct(image_data, result->image_data);

           /* static bool kd = false;
            if (kd == false){
                kd = true;
                cv::imwrite("/Users/josericardo/img_dct.jpg", result->image_data);
            }*/

            return result;
        }

        Image* Image::subImage(int x, int y, int width, int height){
            Image *result = new Image();

            cv::Rect rect(x, y, width, height);

            result->image_data = image_data(rect).clone();

            return result;
        }

        void Image::save(std::string path){
        	cv::imwrite(path.c_str(), image_data);
        }

        Image* Image::correlateNormalized(int width, int height, float *kernel){
            Image *result = new Image();
            cv::Mat temp;
            //cv::Mat temp2;
            //image_data.convertTo(temp, CV_1F);
            cv::Mat mat(height, width, CV_32F, kernel);

            cv::matchTemplate(image_data, mat, temp, cv::TM_CCOEFF);
            cv::normalize(temp, result->image_data, 0, 1, cv::NORM_MINMAX);

           // temp.convertTo(result->image_data, CV_32FC1);

            /*static bool kd = false;
            if (kd == false){
                kd = true;
                cv::imwrite("/home/josericardo/img_kerneltemp.jpg", mat);
                cv::imwrite("/home/josericardo/img_template.jpg", temp);
                cv::imwrite("/home/josericardo/img_normalized.jpg", result->image_data);

                 //   for (int rindex=0;rindex < mat.rows;rindex++){
                   //     for (int cindex=0;cindex < mat.cols;cindex++){
                          // qDebug() << result->image_data.at<float>(rindex, cindex);
                     //   }
                    //}

            }*/



            return result;
        }


        Image* Image::subImageSum(int w, int h, int strideX, int strideY,
                                  int blockSizeX, int blockSizeY){

            Image *result = new Image();
            result->image_data = cv::Mat(h, w, CV_32F);

            for (int rindex=0;rindex < h;rindex++){
                for (int cindex=0;cindex < w;cindex++){
                    cv::Rect r(rindex * strideX, cindex*strideY, blockSizeX, blockSizeY);
                    result->image_data.at<float>(rindex, cindex) =
                            (float)cv::sum(this->image_data(r)).val[0];
                    //qDebug() << "Val: " << (float)cv::sum(this->image_data(r))[0];
                }
            }

            return result;
        }

        Image* Image::grayBlurredEqualized(float sizeX, float sizeY){
            Image* result = new Image();

#ifdef USE_GPU
            cv::gpu::GpuMat gpuMat(image_data);
            cv::gpu::GpuMat gpuResult;
            cv::gpu::cvtColor(gpuMat, gpuResult, cv::COLOR_RGBA2GRAY);
            cv::gpu::blur(gpuResult, gpuMat, cv::Size(sizeX, sizeY));
            cv::gpu::equalizeHist(gpuMat, gpuResult);
            gpuResult.download(result->image_data);
#endif
            return result;
        }

        Image* Image::grayBlurredEqualizedBatch(std::vector< boost::shared_ptr<Image> > img,
                                                float sizeX, float sizeY) {

            cv::cuda::Stream s1;
            cv::cuda::Stream s2;

            Image* res = new Image[img.size()];
            cv::cuda::GpuMat *cudaMem = new cv::cuda::GpuMat[img.size()];
            cv::cuda::GpuMat *gpuMatSrc = new cv::cuda::GpuMat[img.size()];
            cv::cuda::GpuMat *gpuMatDst = new cv::cuda::GpuMat[img.size()];

            for (int i = 0; i < img.size(); i++){
                cudaMem[i] = cv::cuda::GpuMat(img[i]->image_data);
            }


            cv::Ptr<cv::cuda::Filter> blur = cv::cuda::createGaussianFilter(CV_32FC1, CV_32FC1, cv::Size(sizeX,sizeY), 1);

            for (int i = 0; i < img.size(); i += 1){
                gpuMatSrc[i].upload(img[i]->image_data);
                cv::cvtColor(gpuMatSrc[i], gpuMatDst[i], cv::COLOR_RGBA2GRAY);
                blur->apply(gpuMatSrc[i], gpuMatDst[i]);
                cv::cuda::equalizeHist(gpuMatSrc[i], gpuMatDst[i]);
                gpuMatDst[i].download(res[i].image_data);

                /*s1.enqueueUpload(cudaMem[i], gpuMatSrc[i]);
                //s2.enqueueUpload(cudaMem[i+1], gpuMatSrc[i+1]);

                s1.enqueueConvert(gpuMatSrc[i], gpuMatCvt[i], cv::COLOR_RGBA2GRAY);
                //s2.enqueueConvert(gpuMatSrc[i+1], gpuMatDst[i+1], cv::COLOR_RGBA2GRAY);

                cv::gpu::blur(gpuMatCvt[i], gpuMatBlurred[i], cv::Size(sizeX, sizeY), cv::Point(-1, -1), s1);
                //cv::gpu::blur(gpuMatDst[i+1], gpuMatSrc[i+1], cv::Size(sizeX, sizeY), cv::Point(-1, -1), s2);

                cv::gpu::equalizeHist(gpuMatBlurred[i], gpuMatEqualized[i], s1);
                //cv::gpu::equalizeHist(gpuMatSrc[i+1], gpuMatDst[i+1], s2);

                s1.enqueueDownload(gpuMatEqualized[i], res[i].image_data);
                //s2.enqueueDownload(gpuMatDst[i+1], res[i+1].image_data);*/
            }


            //s1.waitForCompletion();
            //s2.waitForCompletion();

            for (int i = 0; i < img.size(); i++){
                delete &cudaMem[i];
                delete &gpuMatSrc[i];
                delete &gpuMatDst[i];
            }

            return res;
        }

        double Image::mean(){
            return (double) cv::mean(image_data).val[0];
        }

        int Image::getPixelSize(int type){
        	if (type == CV_8UC4)
        		return sizeof(uchar) * 4;
        	else if (type == CV_8UC3)
        		return sizeof(uchar) * 3;
        	else if (type == CV_32F)
        		return sizeof(float);
        	else return -1;
        }
    }


}
