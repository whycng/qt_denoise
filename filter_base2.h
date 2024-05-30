#pragma once
#include <QString>
#include <fstream>
#include <vector> 
#include <sstream>
#include<algorithm>


//#define SAMPLE_RATE 1000.0f // sample rate in Hz   ������
//#define DURATION 1.0f // duration of the signal in seconds  �źų���ʱ��
//#define FREQ1 10.0f // frequency of the first component in Hz  ����Ƶ�ʳɷ�
//#define FREQ2 20.0f // frequency of the second component in Hz
//#define FREQ3 30.0f // frequency of the third component in Hz
//#define F2 15.0f // lower bound of the passband in Hz Ƶ�ʷ�Χ
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
//С��3
void wavelet3(const std::vector<std::vector<float>>& data,
              std::vector<std::vector<float>>& outData);
// �� const std::vector<std::vector<float>>& data ת��Ϊ std::vector<std::vector<double>>
std::vector<std::vector<double>> convertData(const std::vector<std::vector<float>>& data);
// MAD-based outlier detection and replace with median
void madFilter(const std::vector<std::vector<float>>& input,
               std::vector<std::vector<float>>& output) ;
// Helper functions
float median(std::vector<float> v);
// Savitzky-Golay��a[0] -> 1/35 (-3 x[-2] + 12 x[-1] + 17 x[0] + 12 x[1] - 3 x[2])
void applySavitzkyGolay(const std::vector<std::vector<float>>& input,
                        std::vector<std::vector<float>>& output,
                        int windowSize, int polyOrder);
//Savitzky-Golay���˲�
std::vector<float> computeSGCoefficients(int windowSize, int polyOrder) ;
// ��ά����λ�˲���
void zeroPhaseFilter2D(const std::vector<std::vector<float>>& input,
                       std::vector<std::vector<float>>& output,
                       const std::vector<float>& b);
// ʵ������λ�˲�����
void zeroPhaseFilter(const std::vector<float>& x, std::vector<float>& y,
                     const std::vector<float>& b);
//����λ�˲�����
// ������һά�˲�����������ɢ�����
void filter(const std::vector<float>& b, const std::vector<float>& a,
            const std::vector<float>& x, std::vector<float>& y);
// ��������һ�� std::vector<float> ���룬���ڴ�С�������غ����ƶ�ƽ���� vector
std::vector<float> movmean(const std::vector<float>& x, int window);
void slidingAverageFilter(const std::vector<std::vector<float>>& input,
                          std::vector<std::vector<float>>& output, int n);
//����ƽ�����˲� --TODO:
void slidingAverageFilter(const std::vector<std::vector<float>>& input,
                          std::vector<std::vector<float>>& output) ;
//��ֵ�˲�
void medianFilter(const std::vector<std::vector<float>>& input,
                  std::vector<std::vector<float>>& output, int kernelSize);
//��ֵ�˲�
void medianFilter(float* input, float* output, int width, int height, int kernelSize);
void gaussianFilter(const std::vector<std::vector<float>>& input,
                    std::vector<std::vector<float>>& output,
                    const int& kernelSize) ;
/*��˹�˲�����һ�������˲�������ʹ�ø�˹�ֲ����������ؽ��м�Ȩ�� ���˲���������ͼ�����˹�˽��о����
��˹������ѭ��˹�ֲ��Ķ�άȨ�����顣 �˲�����������������صļ�Ȩƽ��ֵ��*/
void gaussianFilter(float* input, float* output, int width, int height,
                    const int& kernelSize) ;
//�洢�ļ�
void saveFile(const std::string& filePath, const std::vector<std::vector<float>>& data);
//��ȡlas�ļ�
std::vector<std::vector<float>> readLasFile(const std::string& filePath);
