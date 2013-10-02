#include <cuda.h>
#include <helper_cuda.h>         // helper functions for CUDA error check
#include "K_IMUFF.h"

__global__ void diff_k(uchar4* src1data, uchar4* src2data, uchar4* delta, int totalElements)
{
        int idx = blockDim.x * blockIdx.x + threadIdx.x;

        if (idx < totalElements)
        {
                uchar4 src1 = src1data[idx];
                uchar4 src2 = src2data[idx];

                //if ((src1.x == src2.x) && (src1.y == src2.y) && (src1.z == src2.z))
                //	data[idx] = make_uchar4(0, 0, 0, 0);
                //else
                //	data[idx] = make_uchar4(255, 255, 255, 255);

                //data[idx] = make_int4(src1.x - src2.x, src1.y - src2.y,
                //	src1.z - src2.z, src1.w - src2.w);

                delta[idx] = make_uchar4(src1.x xor src2.x, src1.y xor src2.y, src1.z xor src2.z, src1.w xor src2.w);

        }
}

__global__ void fromRGBToYCbCr(uchar4* src1data, uchar4* out, int totalElements){

}


/*
 * Funcao global em GPU para aplicacao da diferenca em uma imagem
 * imagem = imagem_base + delta
 */
__global__ void patch_k(uchar4* src1data, uchar4* delta, uchar4* outimage, int totalElements)
{
        int idx = blockDim.x * blockIdx.x + threadIdx.x;

        if (idx < totalElements)
        {
                uchar4 src1 = src1data[idx];
                uchar4 src2 = delta[idx];

                //if ((src1.x == src2.x) && (src1.y == src2.y) && (src1.z == src2.z))
                //	data[idx] = make_uchar4(0, 0, 0, 0);
                //else
                //	data[idx] = make_uchar4(255, 255, 255, 255);

                //data[idx] = make_int4(src1.x - src2.x, src1.y - src2.y,
                //	src1.z - src2.z, src1.w - src2.w);

                outimage[idx] = make_uchar4(src1.x xor src2.x, src1.y xor src2.y, src1.z xor src2.z, src1.w xor src2.w);

        }
}


extern "C"
    void gIMUFFDiff(const uchar* src1data, const uchar* src2data, void* delta, int totalElements)
    {
        int numElements = totalElements;

        uchar4 *d_data1, *d_data2, *d_delta;
        checkCudaErrors(cudaMalloc((void**)&d_data1, sizeof(uchar4) * numElements));
        checkCudaErrors(cudaMalloc((void**)&d_data2, sizeof(uchar4) * numElements));
        checkCudaErrors(cudaMalloc((void**)&d_delta, sizeof(uchar4) * numElements));

        checkCudaErrors(cudaMemcpy((void*)d_data1, src1data,
                        sizeof(uchar4) * numElements, cudaMemcpyHostToDevice));
        checkCudaErrors(cudaMemcpy((void*)d_data2, src2data,
                        sizeof(uchar4) * numElements, cudaMemcpyHostToDevice));

        // Processamento
        dim3 dimBlock(256, 1, 1);
        dim3 dimGrid((numElements/dimBlock.x) + 1, 1);
        diff_k<<<dimGrid, dimBlock>>>(d_data1, d_data2, d_delta, numElements);

        checkCudaErrors(cudaMemcpy((void*)delta, d_delta,
                        sizeof(uchar4) * numElements, cudaMemcpyDeviceToHost));

        checkCudaErrors(cudaFree(d_data1));
        checkCudaErrors(cudaFree(d_data2));
        checkCudaErrors(cudaFree(d_delta));
    }


extern "C"
    void gIMUFFPatch(const uchar* src1data, const uchar* delta, void* imgfinal, int totalElements)
    {

        int numElements = totalElements;

        uchar4 *d_data1,*d_delta, *d_data2;
        checkCudaErrors(cudaMalloc((void**)&d_data1, sizeof(uchar4) * numElements));
        checkCudaErrors(cudaMalloc((void**)&d_data2, sizeof(uchar4) * numElements));
        checkCudaErrors(cudaMalloc((void**)&d_delta, sizeof(uchar4) * numElements));

        checkCudaErrors(cudaMemcpy((void*)d_data1, src1data,
                        sizeof(uchar4) * numElements, cudaMemcpyHostToDevice));
        checkCudaErrors(cudaMemcpy((void*)d_delta, delta,
                        sizeof(uchar4) * numElements, cudaMemcpyHostToDevice));

        // Processamento
        dim3 dimBlock(256, 1, 1);
        dim3 dimGrid((numElements/dimBlock.x) + 1, 1);
        patch_k<<<dimGrid, dimBlock>>>(d_data1, d_delta, d_data2, numElements);

        checkCudaErrors(cudaMemcpy((void*)imgfinal, d_data2,
                        sizeof(uchar4) * numElements, cudaMemcpyDeviceToHost));

        checkCudaErrors(cudaFree(d_data1));
        checkCudaErrors(cudaFree(d_data2));
        checkCudaErrors(cudaFree(d_delta));
    }


extern "C"
    void gIMUFFMerge(const uchar* imgbase, const uchar* imgA, const uchar* imgB, void* outimage, int totalElements)
    {
        int numElements = totalElements;

        // Calcular delta da imagem base e B
        uchar4 *d_imgbase, *d_imgA, *d_delta;
        checkCudaErrors(cudaMalloc((void**)&d_imgbase, sizeof(uchar4) * numElements));
        checkCudaErrors(cudaMalloc((void**)&d_imgA, sizeof(uchar4) * numElements));
        checkCudaErrors(cudaMalloc((void**)&d_delta, sizeof(uchar4) * numElements));

        checkCudaErrors(cudaMemcpy((void*)d_imgbase, imgbase,
            sizeof(uchar4) * numElements, cudaMemcpyHostToDevice));
        checkCudaErrors(cudaMemcpy((void*)d_imgA, imgA,
            sizeof(uchar4) * numElements, cudaMemcpyHostToDevice));

        // Processamento do delta
        dim3 dimBlock(256, 1, 1);
        dim3 dimGrid((numElements/dimBlock.x) + 1, 1);
        diff_k<<<dimGrid, dimBlock>>>(d_imgbase, d_imgA, d_delta, numElements);

        // Aplicar patch do delta na imagem B
        uchar4 *d_imgB, *d_outimage;
        checkCudaErrors(cudaMalloc((void**)&d_imgB, sizeof(uchar4) * numElements));
        checkCudaErrors(cudaMalloc((void**)&d_outimage, sizeof(uchar4) * numElements));
        checkCudaErrors(cudaMemcpy((void*)d_imgB, imgB,
            sizeof(uchar4) * numElements, cudaMemcpyHostToDevice));

        patch_k<<<dimGrid, dimBlock>>>(d_imgB, d_delta, d_outimage, numElements);

        checkCudaErrors(cudaMemcpy((void*)outimage, d_outimage,
            sizeof(uchar4) * numElements, cudaMemcpyDeviceToHost));

         checkCudaErrors(cudaFree(d_imgbase));
         checkCudaErrors(cudaFree(d_imgA));
         checkCudaErrors(cudaFree(d_imgB));
         checkCudaErrors(cudaFree(d_delta));
         checkCudaErrors(cudaFree(d_outimage));
    }
