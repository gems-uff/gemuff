#ifndef HASH_H
#define HASH_H

#define cimg_display 0
#define SQRT_TWO 1.4142135623730950488016887242097
#define ROUNDING_FACTOR(x) (((x) >= 0) ? 0.5 : -0.5)

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include "openssl/md5.h"
//#include "CImg.h"
#include "GEMUFF.h"
#include "math.h"
#include <QDebug>
#include <boost/shared_ptr.hpp>
#include <image.h>

//#define HASH_GPU

#ifdef HASH_GPU
    #include <npp.h>
    #include <helper_cuda.h>
#endif

#define MH_HASH_SIZE  72
#define MH_ALPHA  2
#define MH_LEVEL  1

namespace GEMUFF {

    namespace Hash {


        class AbstractHash;
        typedef boost::shared_ptr<Hash::AbstractHash> AbstractHashPtr;

        /*! /brief Digest info
         */
        struct SDigest {
            char *id;                   //hash id
            uint8_t *coeffs;            //the head of the digest integer coefficient array
            int size;                   //the size of the coeff array
        };

        struct SProjections {
           // cimg_library::CImg<uint8_t> *R;           //contains projections of image of angled lines through center
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

        protected:

        public:
            AbstractHash (HashType _type) :  hashType(_type){}

            virtual ~AbstractHash(){ }

            HashType Type(){ return hashType; }

            virtual void writeKey(std::ofstream& _fstream) = 0;
            virtual void readKey(std::ifstream &_ifstream) = 0;

            virtual std::string toString() = 0;

            virtual bool operator < (AbstractHash &h2){
                printf("\nEqual: %s - %s", this->toString().c_str(), h2.toString().c_str());
                return this->toString() < h2.toString();
            }

            virtual double getSimilarity(boost::shared_ptr<Hash::AbstractHash> hash) = 0;

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

        public:
            MD5Hash() : AbstractHash(T_MD5) {
            }

        public:
            static MD5Hash* GenerateHash(VIMUFF::ImagePtr image);

            std::string toString();

             void writeKey(std::ofstream& _fstream);
             void readKey(std::ifstream &_ifstream);

            double getSimilarity(boost::shared_ptr<Hash::AbstractHash> hash){

                if (toString().compare(hash->toString()) == 0)
                    return 1.0f;

                return 0;
            }

        };

        /*
         *Discrete cosine transform based hash
         */
        /*class DCTHash : public AbstractHash{
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

            static DCTHash* GenerateHash(VIMUFF::ImagePtr image);

            float getSimilarity(boost::shared_ptr<Hash::AbstractHash> hash);

            std::string toString(){
                std::stringstream ss;
                ss << hash;
                return ss.str();
            }

            static void Parameters(double _sigma, double _gamma, double n);

        };*/

        class DCTHash : public AbstractHash{
        private:
            ulong64 hash;

        private:

            int BitCount8(long64 val);

            public:
            DCTHash() : AbstractHash(T_DCT){
            }

            static DCTHash* GenerateHash(VIMUFF::ImagePtr image);

            void writeKey(std::ofstream& _fstream){}
            void readKey(std::ifstream &_ifstream){}

            double getSimilarity(boost::shared_ptr<Hash::AbstractHash> hash);

            std::string toString(){
                std::stringstream ss;
                ss << hash;
                return ss.str();
            }
        };


        class MarrHildretchHash : public AbstractHash {
        private:
            uint8_t *hash;

        private:


            //static cimg_library::CImg<float> *GetMHKernel(float _alpha, float _level);

            int static BitCount8(uint8_t val);

        public:
            MarrHildretchHash() : AbstractHash(T_MH){
            }

            static MarrHildretchHash *GenerateHash(VIMUFF::ImagePtr image);

            static std::vector<AbstractHashPtr>
                GenerateHashBatch(std::vector<VIMUFF::ImagePtr> images);

            double getSimilarity(boost::shared_ptr<Hash::AbstractHash> hash);

            std::string toString();

            void writeKey(std::ofstream& _fstream);
            void readKey(std::ifstream &_ifstream);

            ~MarrHildretchHash(){ free(hash); }
        };




      bool write(std::ofstream &_ofstream, AbstractHashPtr hash);
      
      AbstractHashPtr load(std::ifstream &_ifstream);

    }
}

#endif // HASH_H
