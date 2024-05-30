#ifndef BASE_FUNC_H
#define BASE_FUNC_H

#include "qboxlayout.h"
#include <QString>
#include <QComboBox>

// 定义去噪模式的枚举类型
enum DenoiseMode {
    SlidingAverageFilter, //滑动平均法滤波
    MedianFilter,//中值滤波
    GaussianFilter,//高斯滤波
    WaveletDenoise,//小波
    ZeroPhaseFilter2D,//零相位滤波
    SavitzkyGolayFilter,//Savitzky-Golay 滤波
    MadFilter,//mad滤波
    DenoiseModeCount // 用于获取枚举的数量
};
std::string fromQString2stdString(const QString& str);
QString fromStdString2QString(const std::string& str);


void Normalize2DVector(std::vector<std::vector<float>>& data);
void plot_den(const std::vector<std::vector<float>>& outData,QHBoxLayout* hLayout
              ,const float& vmax, const float& vmin);
QString denoiseModeToString(DenoiseMode mode);
void initComboBoxDenoiseMode(QComboBox *comboBox);


#endif // BASE_FUNC_H
