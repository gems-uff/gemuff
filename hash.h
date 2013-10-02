#ifndef HASH_H
#define HASH_H

#define cimg_display 0
#define SQRT_TWO 1.4142135623730950488016887242097
#define ROUNDING_FACTOR(x) (((x) >= 0) ? 0.5 : -0.5)

#include <string>
#include <sstream>
#include <iostream>
#include "openssl/md5.h"
#include "CImg.h"
#include "GEMUFF.h"
#include "math.h"
#include <QDebug>
#include <boost/shared_ptr.hpp>

//#define HASH_GPU

#ifdef HASH_GPU
    #include <npp.h>
    #include <helper_cuda.h>
#endif

namespace GEMUFF {

    namespace Hash {



        /*! /brief Digest info
         */
        struct SDigest {
            char *id;                   //hash id
            uint8_t *coeffs;            //the head of the digest integer coefficient array
            int size;                   //the size of the coeff array
        };

        struct SProjections {
            cimg_library::CImg<uint8_t> *R;           //contains projections of image of angled lines through center
            int *nb_pix_perline;        //the head of int array denoting the number of pixels of each line
            int size;                   //the size of nb_pix_perline
        };

        /*! /brief feature vector info
         */
        struct SFeature {
            double *features;           //the head of the feature array of double's
            int size;                   //the size of the feature array
        };

        enum HashType {
            T_MD5,
            T_DCT,
            T_MH
        };

        /*
         * Base class for hash
         */
        class AbstractHash {
        private:
            HashType hashType;
            boost::shared_ptr<Hash::AbstractHash> data;

        private:
            bool operator==(const AbstractHash &other) const {
                return false;
            }

        public:
            AbstractHash (HashType _type) :  hashType(_type){}

            virtual ~AbstractHash(){ printf("AbstractHash::~AbstractHash()"); }

            HashType Type(){ return hashType; }

            virtual std::string toString() = 0;

            bool operator < (AbstractHash &h2){
                return this->toString() < h2.toString();
            }

            virtual float getSimilarity(boost::shared_ptr<Hash::AbstractHash> hash) = 0;

            bool isSimilar(boost::shared_ptr<Hash::AbstractHash> other, float threshold){

                if (other == NULL)
                    return false;

                if (other->Type() != Type())
                    return false;

                return getSimilarity(other) >= threshold;
            }

            void setData(boost::shared_ptr<Hash::AbstractHash> _data){ data = _data; }
            boost::shared_ptr<Hash::AbstractHash> getData(){ return data; }
        };


        /*
         * MD5 Hash
         */
        class MD5Hash : public AbstractHash {
        private:
            unsigned char key[16];

        private:
            MD5Hash() : AbstractHash(T_MD5) {
            }

        public:
            static MD5Hash* GenerateHash(const unsigned char *d, size_t n);

            std::string toString();

            float getSimilarity(boost::shared_ptr<Hash::AbstractHash> hash){

                if (toString() == hash->toString())
                    return 1.0f;

                return 0;
            }

        };

        /*
         *Discrete cosine transform based hash
         */
        class DCTHash : public AbstractHash{
        private:
            static double Sigma;
            static double N;
            static double Gamma;

        private:
            long64 hash;
            SDigest digest;

        private:
            DCTHash() : AbstractHash(T_DCT){
            }

            static cimg_library::CImg<float>* DctMatrix(const int N);

            static bool FeatureVector(const SProjections &projs, SFeature &fv);

            static bool Digest(cimg_library::CImg<uint8_t> &src, double sigma,
                                 double gamma, int N, SDigest& digest);

            static bool RandomProjections(const cimg_library::CImg<uint8_t> &img,
                                            int N, SProjections &projs);

            static bool Dct(const SFeature &fv,SDigest &digest);

        public:

            static DCTHash* GenerateHash(const unsigned char *d, int rows, int cols, int numChanels);

            float getSimilarity(boost::shared_ptr<Hash::AbstractHash> hash);

            std::string toString(){
                std::stringstream ss;
                ss << hash;
                return ss.str();
            }

            static void Parameters(double _sigma, double _gamma, double n);

        };


        class MarrHildretchHash : public AbstractHash {
        private:
            static int HashSize;
            static float Alpha;
            static float Level;
            uint8_t *hash;

        private:
            MarrHildretchHash() : AbstractHash(T_MH){
            }

            static cimg_library::CImg<float> *GetMHKernel(float _alpha, float _level);

            int static BitCount8(uint8_t val);

        public:
            static MarrHildretchHash *GenerateHash(const unsigned char *d, int rows, int cols, int numChannels);

            float getSimilarity(boost::shared_ptr<Hash::AbstractHash> hash);

            std::string toString();

            static void Parameters(int _n, float _alpha, float _level);

            ~MarrHildretchHash(){ free(hash); printf("MarrHildretchHash::~MarrHildretchHash()"); }
        };



        typedef boost::shared_ptr<Hash::AbstractHash> AbstractHashPtr;

    }
}

#endif // HASH_H
