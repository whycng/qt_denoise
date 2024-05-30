#pragma once
#include <QString>
#include <fstream>
#include <vector> 
#include <sstream>
#include<algorithm>


//#define SAMPLE_RATE 1000.0f // sample rate in Hz   采样率
//#define DURATION 1.0f // duration of the signal in seconds  信号持续时间
//#define FREQ1 10.0f // frequency of the first component in Hz  三个频率成分
//#define FREQ2 20.0f // frequency of the second component in Hz
//#define FREQ3 30.0f // frequency of the third component in Hz
//#define F2 15.0f // lower bound of the passband in Hz 频率范围
//#define F3 25.0f // upper bound of the passband in Hz
//#define M_PI 3.14159265358979323846f


//std::vector<std::vector<float>> wienerFilter(const std::vector<std::vector<float>>& noisy_image, double snr);
//double filter2(
//    double* srcbuffer,
//    int width,
//    int height,
//    int kernelsize,
//    double* dstbuffer1,
//    double* dstbuffer2
//);
//void adpwiener2(
//    unsigned char* inbuffer,
//    int kernelsize,
//    int width,
//    int height,
//    unsigned char* outbuffer
//);
//std::vector<std::vector<float>> readLasFile(const std::string& filePath);
//void saveFile(const std::string& filePath, const std::vector<std::vector<float>>& data);
//void prcFFT(std::vector<std::vector<float>>& data);
//void saveSpecFile(const std::string& filePath, std::vector<std::vector<float>>& data);
//void gaussianFilter(float* input, float* output, int width, int height, const int& kernelSize);
//void gaussianFilter(const std::vector<std::vector<float>>& input, std::vector<std::vector<float>>& output,
//	const int& kernelSize);
//void medianFilter(float* input, float* output, int width, int height, int kernelSize);
//void medianFilter(const std::vector<std::vector<float>>& input, std::vector<std::vector<float>>& output, int kernelSize);
//void wienerFilter(float* input, float* output, int width, int height, float noiseVariance);
//void wienerFilter(const std::vector<std::vector<float>>& input, std::vector<std::vector<float>>& output,
//	float noiseVariance);





void thresholdFilter(std::vector<std::vector<float>>& data ,
                     float threshold1, float threshold2);

void normalize(std::vector<std::vector<float>>& data);
//小波3
void wavelet3(const std::vector<std::vector<float>>& data,
              std::vector<std::vector<float>>& outData);
// 将 const std::vector<std::vector<float>>& data 转换为 std::vector<std::vector<double>>
std::vector<std::vector<double>> convertData(const std::vector<std::vector<float>>& data);
// MAD-based outlier detection and replace with median
void madFilter(const std::vector<std::vector<float>>& input,
               std::vector<std::vector<float>>& output) ;
// Helper functions
float median(std::vector<float> v);
// Savitzky-Golay法a[0] -> 1/35 (-3 x[-2] + 12 x[-1] + 17 x[0] + 12 x[1] - 3 x[2])
void applySavitzkyGolay(const std::vector<std::vector<float>>& input,
                        std::vector<std::vector<float>>& output,
                        int windowSize, int polyOrder);
//Savitzky-Golay法滤波
std::vector<float> computeSGCoefficients(int windowSize, int polyOrder) ;
// 二维零相位滤波器
void zeroPhaseFilter2D(const std::vector<std::vector<float>>& input,
                       std::vector<std::vector<float>>& output,
                       const std::vector<float>& b);
// 实现零相位滤波功能
void zeroPhaseFilter(const std::vector<float>& x, std::vector<float>& y,
                     const std::vector<float>& b);
//零相位滤波功能
// 基本的一维滤波器函数（离散卷积）
void filter(const std::vector<float>& b, const std::vector<float>& a,
            const std::vector<float>& x, std::vector<float>& y);
// 函数接受一个 std::vector<float> 输入，窗口大小，并返回含有移动平均的 vector
std::vector<float> movmean(const std::vector<float>& x, int window);
void slidingAverageFilter(const std::vector<std::vector<float>>& input,
                          std::vector<std::vector<float>>& output, int n);
//滑动平均法滤波 --TODO:
void slidingAverageFilter(const std::vector<std::vector<float>>& input,
                          std::vector<std::vector<float>>& output) ;
//中值滤波
void medianFilter(const std::vector<std::vector<float>>& input,
                  std::vector<std::vector<float>>& output, int kernelSize);
//中值滤波
void medianFilter(float* input, float* output, int width, int height, int kernelSize);
void gaussianFilter(const std::vector<std::vector<float>>& input,
                    std::vector<std::vector<float>>& output,
                    const int& kernelSize) ;
/*高斯滤波器是一种线性滤波器，它使用高斯分布对相邻像素进行加权。 该滤波器将输入图像与高斯核进行卷积，
高斯核是遵循高斯分布的二维权重数组。 滤波器的输出是相邻像素的加权平均值。*/
void gaussianFilter(float* input, float* output, int width, int height,
                    const int& kernelSize) ;
//存储文件
void saveFile(const std::string& filePath, const std::vector<std::vector<float>>& data);
//读取las文件
std::vector<std::vector<float>> readLasFile(const std::string& filePath);
