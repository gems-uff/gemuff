#include "hash.h"

namespace GEMUFF {

    namespace Hash {

        std::string MD5Hash::toString()
        {
            char res[32] = {0};

            for (size_t k = 0; k < 16; k++)
                sprintf(&res[k*2], "%02x", key[k]);

            return std::string(res);
        }

        MD5Hash *MD5Hash::GenerateHash(const unsigned char *d, size_t n){
            MD5Hash *_hashKey = new MD5Hash();

            MD5(d, n, &_hashKey->key[0]);

            return _hashKey;
        }





        double DCTHash::Sigma = 3;
        double DCTHash::N =3;
        double DCTHash::Gamma = 10;
        DCTHash* DCTHash::GenerateHash(const unsigned char *d, int rows, int cols, int numChanels){
            cimg_library::CImg<unsigned char> src;
            cimg_library::CImg<float> img;

            DCTHash *dctHash = new DCTHash();

            src.assign(d, cols, rows, 1, numChanels);

            cimg_library::CImg<float> meanfilter(7, 7, 1, 1, 1);

            if (src.spectrum() == 3){
                img = src.RGBtoYCbCr().channel(0).get_convolve(meanfilter);
            } else if (src.spectrum() == 4){
                int width = img.width();
                int height = img.height();
                int depth = img.depth();
                img = src.crop(0,0,0,0,width-1,height-1,depth-1,2).RGBtoYCbCr().channel(0).get_convolve(meanfilter);
            } else {
                img = src.channel(0).get_convolve(meanfilter);
            }

            img.resize(32, 32);
            cimg_library::CImg<float> *C = DctMatrix(32);
            cimg_library::CImg<float> CTransp = C->get_transpose();
            cimg_library::CImg<float> dctImg = (*C)*img*CTransp;

            cimg_library::CImg<float> subSec = dctImg.crop(1, 1, 8, 8).unroll('x');

            float median = subSec.median();
            ulong64 one = 0x0000000000000001;
            dctHash->hash = 0x0000000000000000;

            for (int i=0;i< 64;i++){
            float current = subSec(i);
                if (current > median)
                dctHash->hash |= one;
            one = one << 1;
            }

            delete C;

            // Image digest
           // Digest(src, Sigma, Gamma, N, dctHash->digest);

            return dctHash;
        }

        float DCTHash::getSimilarity(boost::shared_ptr<Hash::AbstractHash> hash){

            return 0;

             /*DCTHash* _dctHash = (DCTHash*) hash;

            ulong64 x = this->hash^_dctHash->hash;
            const ulong64 m1  = 0x5555555555555555ULL;
            const ulong64 m2  = 0x3333333333333333ULL;
            const ulong64 h01 = 0x0101010101010101ULL;
            const ulong64 m4  = 0x0f0f0f0f0f0f0f0fULL;
            x -= (x >> 1) & m1;
            x = (x & m2) + ((x >> 2) & m2);
            x = (x + (x >> 4)) & m4;

             return 0;
            /*double pcc;

            if (hash->Type() != T_DCT)
                return false;

             SDigest _otherDigest = ((DCTHash*) hash)->digest;

            int N =  _otherDigest.size;

            uint8_t *x_coeffs = digest.coeffs;
            uint8_t *y_coeffs = _otherDigest.coeffs;

            double *r = new double[N];
            double sumx = 0.0;
            double sumy = 0.0;
            for (int i=0;i < N;i++){
            sumx += x_coeffs[i];
                sumy += y_coeffs[i];
            }
            double meanx = sumx/N;
            double meany = sumy/N;
            double max = 0;
            for (int d=0;d<N;d++){
                double num = 0.0;
                double denx = 0.0;
                double deny = 0.0;
            for (int i=0;i<N;i++){
                num  += (x_coeffs[i]-meanx)*(y_coeffs[(N+i-d)%N]-meany);
                    denx += pow((x_coeffs[i]-meanx),2);
                    deny += pow((y_coeffs[(N+i-d)%N]-meany),2);
            }
                r[d] = num/sqrt(denx*deny);
                if (r[d] > max)
                max = r[d];
            }
            delete[] r;
            pcc = max;

            if (pcc > threshold)
                return true;

            return false;*/
        }


        cimg_library::CImg<float> *DCTHash::DctMatrix(const int N){

            cimg_library::CImg<float> *ptr_matrix =
                    new cimg_library::CImg<float>(N,N,1,1,1/sqrt((float)N));

            const float c1 = sqrt(2.0/N);

            for (int x=0;x<N;x++){
                for (int y=1;y<N;y++){
                    *ptr_matrix->data(x,y) = c1*cos((cimg_library::cimg::PI/2/N)*y*(2*x+1));
                }
            }

            return ptr_matrix;
        }

        bool DCTHash::Digest(cimg_library::CImg<uint8_t> &src, double sigma,
                             double gamma, int N, SDigest& digest){
            cimg_library::CImg<uint8_t> graysc;

            if (src.spectrum() >= 3){
                graysc = src.get_RGBtoYCbCr().channel(0);
            }
            else if (src.spectrum() == 1){
                graysc = src;
            }
            else {
                return false;
            }

            graysc.blur((float)sigma);

            (graysc/graysc.max()).pow(gamma);

            SProjections projs;
            if (RandomProjections(graysc,N,projs) < 0){
                free(projs.nb_pix_perline);
                delete projs.R;
                return false;
            }

            SFeature features;
            if (FeatureVector(projs,features) < 0){
                free(projs.nb_pix_perline);
                free(features.features);
                delete projs.R;
                return false;
            }

            if (Dct(features,digest) < 0){
                free(projs.nb_pix_perline);
                free(features.features);
                delete projs.R;
                return false;
            }

            free(projs.nb_pix_perline);
            free(features.features);
            delete projs.R;
            return true;
        }


        bool DCTHash::RandomProjections(const cimg_library::CImg<uint8_t> &img,
                                        int N, SProjections &projs){

            int width = img.width();
            int height = img.height();
            int D = (width > height)?width:height;
            float x_center = (float)width/2;
            float y_center = (float)height/2;
            int x_off = (int)std::floor(x_center + ROUNDING_FACTOR(x_center));
            int y_off = (int)std::floor(y_center + ROUNDING_FACTOR(y_center));

            projs.R = new cimg_library::CImg<uint8_t>(N,D,1,1,0);
            projs.nb_pix_perline = (int*)calloc(N,sizeof(int));

            if (!projs.R || !projs.nb_pix_perline)
                return false;

            projs.size = N;

            cimg_library::CImg<uint8_t> *ptr_radon_map = projs.R;
            int *nb_per_line = projs.nb_pix_perline;

            for (int k=0;k<N/4+1;k++){
                double theta = k*cimg_library::cimg::PI/N;
                double alpha = std::tan(theta);

                for (int x=0;x < D;x++){
                    double y = alpha*(x-x_off);
                    int yd = (int)std::floor(y + ROUNDING_FACTOR(y));

                    if ((yd + y_off >= 0)&&(yd + y_off < height) && (x < width)){
                        *ptr_radon_map->data(k,x) = img(x,yd + y_off);
                        nb_per_line[k] += 1;
                    }

                    if ((yd + x_off >= 0) && (yd + x_off < width) && (k != N/4) && (x < height)){
                        *ptr_radon_map->data(N/2-k,x) = img(yd + x_off,x);
                        nb_per_line[N/2-k] += 1;
                    }
                }
            }

            int j= 0;
            for (int k=3*N/4;k<N;k++){

                double theta = k*cimg_library::cimg::PI/N;
                double alpha = std::tan(theta);

                for (int x=0;x < D;x++){

                    double y = alpha*(x-x_off);
                    int yd = (int)std::floor(y + ROUNDING_FACTOR(y));

                    if ((yd + y_off >= 0)&&(yd + y_off < height) && (x < width)){
                        *ptr_radon_map->data(k,x) = img(x,yd + y_off);
                        nb_per_line[k] += 1;
                    }

                    if ((y_off - yd >= 0)&&
                        (y_off - yd<width)&&(2*y_off-x>=0)&&
                        (2*y_off-x<height)&&(k!=3*N/4)){

                        *ptr_radon_map->data(k-j,x) = img(-yd+y_off,-(x-y_off)+y_off);
                        nb_per_line[k-j] += 1;
                    }

                }
                j += 2;
            }

            return true;
        }


        bool DCTHash::FeatureVector(const SProjections &projs, SFeature &fv)
        {
            cimg_library::CImg<uint8_t> *ptr_map = projs.R;
            cimg_library::CImg<uint8_t> projection_map = *ptr_map;
            int *nb_perline = projs.nb_pix_perline;
            int N = projs.size;
            int D = projection_map.height();

            fv.features = (double*)malloc(N*sizeof(double));
            fv.size = N;

            if (!fv.features)
                return false;

            double *feat_v = fv.features;
            double sum = 0.0;
            double sum_sqd = 0.0;

            for (int k=0; k < N; k++){
                double line_sum = 0.0;
                double line_sum_sqd = 0.0;
                int nb_pixels = nb_perline[k];

                for (int i=0;i<D;i++){
                    line_sum += projection_map(k,i);
                        line_sum_sqd += projection_map(k,i)*projection_map(k,i);
                }

                feat_v[k] = (line_sum_sqd/nb_pixels) - (line_sum*line_sum)/(nb_pixels*nb_pixels);
                sum += feat_v[k];
                sum_sqd += feat_v[k]*feat_v[k];
            }

            double mean = sum/N;
            double var  = sqrt((sum_sqd/N) - (sum*sum)/(N*N));

            for (int i=0;i<N;i++){
                feat_v[i] = (feat_v[i] - mean)/var;
            }

            return true;
        }

        bool DCTHash::Dct(const SFeature &fv,SDigest &digest)
        {
            int N = fv.size;
            const int nb_coeffs = 40;

            digest.coeffs = (uint8_t*)malloc(nb_coeffs*sizeof(uint8_t));
            if (!digest.coeffs)
            return EXIT_FAILURE;

            digest.size = nb_coeffs;

            double *R = fv.features;

            uint8_t *D = digest.coeffs;

            double D_temp[nb_coeffs];
            double max = 0.0;
            double min = 0.0;
            for (int k = 0;k<nb_coeffs;k++){
            double sum = 0.0;
                for (int n=0;n<N;n++){
                double temp = R[n]*cos((cimg_library::cimg::PI*(2*n+1)*k)/(2*N));
                    sum += temp;
            }
                if (k == 0)
                D_temp[k] = sum/sqrt((double)N);
                else
                    D_temp[k] = sum*SQRT_TWO/sqrt((double)N);
                if (D_temp[k] > max)
                    max = D_temp[k];
                if (D_temp[k] < min)
                    min = D_temp[k];
            }

            for (int i=0;i<nb_coeffs;i++){

            D[i] = (uint8_t)(UCHAR_MAX*(D_temp[i] - min)/(max - min));

            }

            return EXIT_SUCCESS;
        }



        int MarrHildretchHash::HashSize = 72;
        float MarrHildretchHash::Alpha = 2;
        float MarrHildretchHash::Level = 1;
        MarrHildretchHash *MarrHildretchHash::GenerateHash(const unsigned char *d, int rows, int cols, int numChannels){

            MarrHildretchHash *mhHash = new MarrHildretchHash();

            const unsigned char* _processedImg = NULL;

#ifdef HASH_GPU
            NppiSize nppSize; nppSize.width = cols; nppSize.height = rows;

            int stride = sizeof(unsigned char) * cols;
            Npp8u* imgOriginal = nppiMalloc_8u_C3(cols, rows, &stride);
            Npp8u* imgTmp = nppiMalloc_8u_C3(cols, rows, &stride);

            checkCudaErrors(cudaMemcpy2D(imgOriginal, 0, d, 0, cols, rows, cudaMemcpyHostToDevice));
            nppiRGBToYCbCr_8u_C3R(imgOriginal, 0, imgTmp, 0, nppSize);

            _processedImg = (const unsigned char*) malloc(sizeof(unsigned char) * cols * rows * numChannels);
            checkCudaErrors(cudaMemcpy2D((void*)_processedImg, 0, imgTmp, 0, cols, rows, cudaMemcpyDeviceToHost));

            nppiFree(imgOriginal);
            nppiFree(imgTmp);
#else
            _processedImg = d;
#endif

            cimg_library::CImg<unsigned char> src;
            cimg_library::CImg<uint8_t> img;


            src.assign(_processedImg, cols, rows, 1, numChannels);

            mhHash->hash = (unsigned char*)malloc(HashSize*sizeof(uint8_t));


            if (src.spectrum() == 3){
#ifdef HASH_GPU
                img = src.channel(0).blur(1.0).resize(512,512,1,1,5).get_equalize(256);
#else
                img = src.get_RGBtoYCbCr().channel(0).blur(1.0).resize(512,512,1,1,5).get_equalize(256);
#endif
            } else{ 
                img = src.channel(0).get_blur(1.0).resize(512,512,1,1,5).get_equalize(256);
            }

            src.clear();

            cimg_library::CImg<float> *pkernel = GetMHKernel(Alpha,Level);
            cimg_library::CImg<float> fresp =  img.get_correlate(*pkernel);
            img.clear();
            fresp.normalize(0,1.0);

            cimg_library::CImg<float> blocks(31,31,1,1,0);

            for (int rindex=0;rindex < 31;rindex++){
                for (int cindex=0;cindex < 31;cindex++){
                    blocks(rindex,cindex) = fresp.get_crop(rindex*16,cindex*16,rindex*16+16-1,cindex*16+16-1).sum();
                }
            }

            int hash_index;
            int nb_ones = 0, nb_zeros = 0;
            int bit_index = 0;
            unsigned char hashbyte = 0;
            for (int rindex=0;rindex < 31-2;rindex+=4){
                cimg_library::CImg<float> subsec;
                for (int cindex=0;cindex < 31-2;cindex+=4){
                    subsec = blocks.get_crop(cindex,rindex, cindex+2, rindex+2).unroll('x');
                    float ave = subsec.mean();

                    cimg_forX(subsec, I){
                        hashbyte <<= 1;

                        if (subsec(I) > ave){
                            hashbyte |= 0x01;
                            nb_ones++;
                        } else {
                            nb_zeros++;
                        }

                        bit_index++;
                        if ((bit_index%8) == 0){
                            hash_index = (int)(bit_index/8) - 1;
                            mhHash->hash[hash_index] = hashbyte;
                            hashbyte = 0x00;
                        }
                    }
                }
            }

#ifdef HASH_GPU
            free((void*)_processedImg);
#endif
            return mhHash;
        }


        cimg_library::CImg<float> *MarrHildretchHash::GetMHKernel(float _alpha, float _level){

            int sigma = (int)4*pow((float)_alpha,(float)_level);
            static cimg_library::CImg<float> *pkernel = NULL;
            float xpos, ypos, A;

            if (!pkernel){
                pkernel = new cimg_library::CImg<float>(2*sigma+1,2*sigma+1,1,1,0);
                cimg_forXY(*pkernel,X,Y){
                    xpos = pow(_alpha,-_level)*(X-sigma);
                    ypos = pow(_alpha,-_level)*(Y-sigma);
                    A = xpos*xpos + ypos*ypos;
                    pkernel->atXY(X,Y) = (2-A)*exp(-A/2);
                }
            }
            return pkernel;
        }

        float MarrHildretchHash::getSimilarity(boost::shared_ptr<Hash::AbstractHash> hash){

            boost::shared_ptr<Hash::MarrHildretchHash> mhHash =
                        boost::dynamic_pointer_cast<Hash::MarrHildretchHash>(hash);

            double dist = 0;
            uint8_t D = 0;
            for (int i=0;i<HashSize;i++){
                D = this->hash[i]^mhHash->hash[i];
                dist = dist + (double)BitCount8(D);
            }

            double bits = (double)HashSize*8;
            return dist/bits;
        }

        void MarrHildretchHash::Parameters(int _n, float _alpha, float _level){
            HashSize = _n;
            Alpha = _alpha;
            Level = _level;
        }

        std::string MarrHildretchHash::toString(){

            char *res = new char[HashSize*2];
            memset(res, 0, sizeof(char) * HashSize * 2);

            for (size_t k = 0; k < HashSize; k++)
                sprintf(&res[k*2], "%02x", hash[k]);

            delete res;

            return std::string(res);

        }

        int MarrHildretchHash::BitCount8(uint8_t val){
            int num = 0;

            while (val){
                ++num;
                val &= val - 1;
            }

            return num;
        }
    }
}
