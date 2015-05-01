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

        MD5Hash *MD5Hash::GenerateHash(VIMUFF::ImagePtr image){
            MD5Hash *_hashKey = new MD5Hash();

            MD5(image->getData(),
                image->getHeight()*image->getWidth()*image->getChannels(),
                &_hashKey->key[0]);

            return _hashKey;
        }

        void MD5Hash::writeKey(std::ofstream &_fstream){
            _fstream.write((char*)key, sizeof(unsigned char)*16);
        }

        void MD5Hash::readKey(std::ifstream &_ifstream){
            _ifstream.read((char*)key, sizeof(unsigned char)*16);
        }

        DCTHash* DCTHash::GenerateHash(VIMUFF::ImagePtr image){

            DCTHash *dctHash = new DCTHash();

            // 1 step: get the 32x32 gray scale image
            VIMUFF::ImagePtr grayScaledImage(image->getGrayResizedThumb());

            // 2 step: compute the dct
            VIMUFF::ImagePtr dctImage(grayScaledImage->dct());

            // 3 step: get only the 8x8 top right pixels (low frequency)
            // David Starkweather for the added information about pHash. He wrote:
            // "the dct hash is based on the low 2D DCT coefficients starting at the
            // second from lowest, leaving out the first DC term. This excludes
            //completely flat image information (i.e. solid colors) from being
            // included in the hash description."
            VIMUFF::ImagePtr dctCropped(dctImage->subImage(1, 1, 8, 8));

            // 4 step: calculate the mean
            float mean = (float)dctCropped->mean();
            const float* data = reinterpret_cast<const float*>(dctCropped->data());

            ulong64 one = 0x0000000000000001;
            dctHash->hash = 0x0000000000000000;

            for (int i=0;i< 64;i++){
                if (data[i] > mean)
                    dctHash->hash |= one;

                one = one << 1;
            }

            return dctHash;
        }

        double DCTHash::getSimilarity(boost::shared_ptr<Hash::AbstractHash> hash){

            boost::shared_ptr<Hash::DCTHash> mhHash =
                        boost::dynamic_pointer_cast<Hash::DCTHash>(hash);


            ulong64 x = this->hash^mhHash->hash;
            const ulong64 m1  = 0x5555555555555555ULL;
            const ulong64 m2  = 0x3333333333333333ULL;
            const ulong64 h01 = 0x0101010101010101ULL;
            const ulong64 m4  = 0x0f0f0f0f0f0f0f0fULL;
            x -= (x >> 1) & m1;
            x = (x & m2) + ((x >> 2) & m2);
            x = (x + (x >> 4)) & m4;

            //qDebug() << "h1: " << toString().c_str() << " h2: " << hash->toString().c_str() << " s: " << (((x * h01)>>56)/64.0);
            return 1.0f - (((x * h01)>>56)/64.0);
        }

        int DCTHash::BitCount8(long64 val){
            int num = 0;

            while (val){
                ++num;
                val &= val - 1;
            }

            return num;
        }



        /*double DCTHash::Sigma = 3;
        double DCTHash::N =3;
        double DCTHash::Gamma = 10;
        DCTHash* DCTHash::GenerateHash(VIMUFF::ImagePtr image){
            cimg_library::CImg<unsigned char> src;
            cimg_library::CImg<float> img;

            DCTHash *dctHash = new DCTHash();

            src.assign(image->getData(),
                       image->getWidth(),
                       image->getHeight(), 1,
                       image->getChannels());

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

            return false;
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
        }*/



        MarrHildretchHash *MarrHildretchHash::GenerateHash(VIMUFF::ImagePtr image){

            MarrHildretchHash *mhHash = new MarrHildretchHash();
            mhHash->hash = (unsigned char*)malloc(MH_HASH_SIZE*sizeof(uint8_t));



            VIMUFF::ImagePtr grayImg(image->toGray());
            VIMUFF::ImagePtr blurred (grayImg->blur(1.0f, 1.0f));
            VIMUFF::ImagePtr equalized (blurred->equalize());


            int sigma = (int)4*pow((float)MH_ALPHA,(float)MH_LEVEL);
            int width = (2*sigma+1);
            int height = (2*sigma+1);
            float kernel[width * height];

            for (int y = 0; y < height; y++){
                for (int x = 0; x < width; x++){
                    float xpos = pow(MH_ALPHA,-MH_LEVEL)*(x-sigma);
                    float ypos = pow(MH_ALPHA,-MH_LEVEL)*(y-sigma);
                    float A = xpos*xpos + ypos*ypos;

                    kernel[y*width + x] = (2-A)*exp(-A/2);
                }
            }

            VIMUFF::ImagePtr correlateNormalized(equalized->correlateNormalized(width, height, kernel));
            VIMUFF::ImagePtr final(correlateNormalized->resize(512, 512));

           VIMUFF::ImagePtr _blocks(final->subImageSum(31, 31, 16, 16, 16, 16));
            int hash_index;
            int nb_ones = 0, nb_zeros = 0;
            int bit_index = 0;
            unsigned char hashbyte = 0;
            for (int rindex=0;rindex < 31-2;rindex+=4){
                for (int cindex=0;cindex < 31-2;cindex+=4){
                    VIMUFF::ImagePtr subsec(_blocks->subImage(cindex, rindex, 3, 3));


                    float ave = subsec->mean();
                    float* data = (float*) subsec->getData();


                    for (int i = 0; i < subsec->getWidth()*subsec->getHeight(); i++){
                        hashbyte <<= 1;
                        if (data[i] > ave){
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

            return mhHash;
        }

        std::vector<AbstractHashPtr>
            MarrHildretchHash::GenerateHashBatch(std::vector<VIMUFF::ImagePtr> images){

            //VIMUFF::Image *_images =
              //      VIMUFF::Image::grayBlurredEqualizedBatch(images, 1.0f, 1.0f);

            std::vector<AbstractHashPtr> hashResults;

            for (int i = 0; i < images.size(); i++){
                //VIMUFF::ImagePtr equalized(&_images[i]);
                //AbstractHashPtr _hash(GenerateHash(equalized));
                AbstractHashPtr _hash(DCTHash::GenerateHash(images[i]));
                hashResults.push_back(_hash);
            }

            return hashResults;
        }


        double MarrHildretchHash::getSimilarity(boost::shared_ptr<Hash::AbstractHash> hash){

            boost::shared_ptr<Hash::MarrHildretchHash> mhHash =
                        boost::dynamic_pointer_cast<Hash::MarrHildretchHash>(hash);

            double dist = 0;
            uint8_t D = 0;
            for (int i=0;i<MH_HASH_SIZE;i++){
                D = this->hash[i]^mhHash->hash[i];
                dist = dist + (double)BitCount8(D);
            }

            double bits = (double)MH_HASH_SIZE*8;
            return dist/bits;
        }

        std::string MarrHildretchHash::toString(){

            char *res = new char[MH_HASH_SIZE*2];
            memset(res, 0, sizeof(char) * MH_HASH_SIZE * 2);

            for (size_t k = 0; k < MH_HASH_SIZE; k++)
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

        void MarrHildretchHash::writeKey(std::ofstream &_fstream){
            _fstream.write((char*)&(*hash), sizeof(uint8_t)*MH_HASH_SIZE);
        }

        void MarrHildretchHash::readKey(std::ifstream &_ifstream){
            hash = (unsigned char*)malloc(MH_HASH_SIZE*sizeof(uint8_t));
            _ifstream.read((char*)hash, sizeof(uint8_t)&MH_HASH_SIZE);
        }




        bool write(std::ofstream &_ofstream, AbstractHashPtr hash){
            bool isNull = (hash) ? false : true;

            _ofstream.write((char*)&isNull, sizeof(bool));

            if (!isNull){
                int type = 0;
                if (hash->Type() == T_MD5) type = 1;
                if (hash->Type() == T_DCT) type = 2;
                if (hash->Type() == T_MH) type = 3;

                _ofstream.write((char*)&type, sizeof(int));
                hash->writeKey(_ofstream);

                // Check for data
                AbstractHashPtr subData = hash->getData();

                bool subDataIsNull = (subData) ? false : true;
                _ofstream.write((char*) &subDataIsNull, sizeof(bool));

                if (!subDataIsNull){
                    int subType = 0;

                    if (subData->Type() == T_MD5) subType = 1;
                    if (subData->Type() == T_DCT) subType = 2;
                    if (subData->Type() == T_MH) subType = 3;

                    subData->writeKey(_ofstream);
                }
            }
        }

        AbstractHashPtr load(std::ifstream &_ifstream){

            AbstractHashPtr res;

            bool isNull;
            _ifstream.read((char*)&isNull, sizeof(bool));

            if (!isNull){
                int type;
                _ifstream.read((char*)&type, sizeof(int));

                if (type == 1) res.reset(new MD5Hash);
                if (type == 2) res.reset(new DCTHash);
                if (type == 3) res.reset(new MarrHildretchHash);

                res->readKey(_ifstream);

                bool subDataIsNull;
                _ifstream.read((char*)&subDataIsNull, sizeof(bool));

                if (!subDataIsNull){
                    int subType;
                    _ifstream.read((char*)&subType, sizeof(int));

                    AbstractHashPtr _subDataHash;

                    if (subType == 1) _subDataHash.reset(new MD5Hash);
                    if (subType == 2) _subDataHash.reset(new DCTHash);
                    if (subType == 3) _subDataHash.reset(new MarrHildretchHash);

                    _subDataHash->readKey(_ifstream);
                    res->setData(_subDataHash);
                }
            }

            return res;
        }

    }
}
