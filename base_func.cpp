
#include "base_func.h"
#include "mainwindow.h"
#include "qcustomplot.h"
#include "qtextcodec.h"
#include <QDebug>

//均值去噪计算需要的 smnoise
//double calculateMeanValue(int xMinIndex, int xMaxIndex,
//                          int yMinIndex, int yMaxIndex,
//                          double radius,
//                          double xCoord, double yCoord,
//                        const std::vector<std::vector<float>>& outPutData,
//                        const QCPColorMapData*& data) {
//    double sum = 0.0;
//    int count = 0;

//    for (int xIndex = xMinIndex; xIndex <= xMaxIndex; ++xIndex) {
//        for (int yIndex = yMinIndex; yIndex <= yMaxIndex; ++yIndex) {
//            double x, y;
//            data->cellToCoord(xIndex, yIndex, &x, &y);
//            double distance = std::sqrt(std::pow(x - xCoord, 2) + std::pow(y - yCoord, 2));

//            if (distance <= radius) {
//                sum += outPutData[xIndex][yIndex];
//                count++;
//            }
//        }
//    }

//    return (count > 0) ? (sum / count) : 0.0;
//}


//阈值去噪
void theshorldDenoise(const std::vector<float>& values,
                      const std::vector<std::pair<int, int>>& indices,
                      const float& threshold,
                      std::vector<std::vector<float>>& m_outPutData
                       )
{
    std::vector<size_t> indicesSorted(values.size());
    std::iota(indicesSorted.begin(), indicesSorted.end(), 0);
    std::sort(indicesSorted.begin(), indicesSorted.end(), [&values](size_t a, size_t b) {
        return values[a] > values[b];
    });

    // 选择前 %以及后 %的数据点作为噪点
    size_t numNoisePoints = indicesSorted.size() * threshold;//0.1
    //qDebug() << " 阈值个数 numNoisePoints:" << numNoisePoints << " 总数 indicesSorted.size():"
    //         << indicesSorted.size() << " threshold:" << threshold;
    //int sum_test = 0;
    for (size_t k = 0; k < indicesSorted.size(); ++k) {
        if( k >= numNoisePoints && k <= indicesSorted.size() - numNoisePoints)
            continue;
        //++sum_test;

        int i = indices[indicesSorted[k]].first;
        int j = indices[indicesSorted[k]].second;
        m_outPutData[i][j] = 0.5;
    }
}

//彻底解决中文路径问题
// QString转 std::string ,  std转QString
std::string fromQString2stdString(const QString& str)
{
    QTextCodec *code = QTextCodec::codecForName("GB2312");//解决中文路径问题  UTF-8 GB2312
    return code->fromUnicode(str).data();
}
// std::string 转QString
QString fromStdString2QString(const std::string& str)
{
    QTextCodec *code = QTextCodec::codecForName("GB2312");//解决中文路径问题  UTF-8 GB2312
    QByteArray strArray = QByteArray(str.c_str(), str.length());
    return code->toUnicode(strArray);
}

//归一化数据
void Normalize2DVector(std::vector<std::vector<float>>& data) {
    if (data.empty() || data[0].empty()) {
        return; // Handle empty input
    }

    float min_value = std::numeric_limits<float>::max();
    float max_value = std::numeric_limits<float>::lowest();

    // Find the minimum and maximum values in the 2D vector
    for (const auto& row : data) {
        auto [min_it, max_it] = std::minmax_element(row.begin(), row.end());
        min_value = std::min(min_value, *min_it);
        max_value = std::max(max_value, *max_it);
    }

    if (min_value == max_value) {
        // All values are the same, normalization isn't meaningful
        std::fill(data.begin(), data.end(), std::vector<float>(data[0].size(), 0.0f));
        return;
    }

    // Normalize the data
    for (auto& row : data) {
        for (auto& elem : row) {
            elem = (elem - min_value) / (max_value - min_value);
        }
    }
}


//绘图函数
//void plot_den(const std::vector<std::vector<float>>& outData,QHBoxLayout* hLayout
//                          ,const float& vmax, const float& vmin
//                          ,QCustomPlot* customPlot_den )
//{
//    if( true == outData.empty())
//    {
//        qDebug() << " EMPTY oudata";
//        return;
//    }


//    const int row = outData.size();
//    const int col = outData.at(0).size();

//    QCPColorMap* colorMap = new QCPColorMap(customPlot_den->xAxis, customPlot_den->yAxis);

//    colorMap->data()->setSize(row, col);
//    for (int i = 0  ; i < row; ++i )
//    {
//        for (int j = 0; j < col; ++j)
//        {
//            colorMap->data()->setCell(i, j, outData.at(i).at(j));
//        }
//    }
//    // 设置颜色映射
//    QCPColorGradient colorGradient;
//    colorGradient.loadPreset(QCPColorGradient::gpJet);
//    colorMap->setGradient(colorGradient);
//    colorMap->setInterpolate(false);
//    colorMap->setTightBoundary(true);
//    // 设置颜色映射的上下限
//    colorMap->setDataRange(QCPRange(vmin, vmax));

//    //customPlot_den->xAxis->setLabel(" "); // 设置横坐标标签
//    //customPlot_den->yAxis->setLabel(" "); // 设置纵坐标标签
//    //customPlot_den->yAxis->setRangeReversed(true);// 设置纵坐标反向
//    customPlot_den->axisRect()->setupFullAxesBox();//四边安装轴并显示
//    //customPlot_priW_den->legend->setVisible(true); // 显示图例
//    //customPlot_den->rescaleAxes();
//    customPlot_den->replot();

//    //交互方式
//    customPlot_den->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);


//}



// 辅助函数：将枚举值转换为相应的字符串
QString denoiseModeToString(DenoiseMode mode) {
    switch (mode) {
    case SlidingAverageFilter: return ("滑动平均法滤波"); //QString乱码； QObject::tr --国际化支持 ？
    case MedianFilter: return ("中值滤波");
    case GaussianFilter: return ("高斯滤波");
    case WaveletDenoise: return ("小波去噪");
    case ZeroPhaseFilter2D: return ("零相位滤波");
    case SavitzkyGolayFilter: return ("Savitzky-Golay滤波");
    case MadFilter: return ("mad滤波");
//    case SlidingAverageFilter: return fromStdString2QString("滑动平均法滤波"); //QString乱码； QObject::tr --国际化支持 ？
//    case MedianFilter: return fromStdString2QString("中值滤波");
//    case GaussianFilter: return fromStdString2QString("高斯滤波");
//    case WaveletDenoise: return fromStdString2QString("小波去噪");
//    case ZeroPhaseFilter2D: return fromStdString2QString("零相位滤波");
//    case SavitzkyGolayFilter: return fromStdString2QString("Savitzky-Golay滤波");
//    case MadFilter: return fromStdString2QString("mad滤波");
    default: return QString();
    }
}

// 初始化 comboBox_denoiseMode 下拉框
void initComboBoxDenoiseMode(QComboBox *comboBox) {
    for (int i = 0; i < DenoiseModeCount; ++i) {
        comboBox->addItem(denoiseModeToString(static_cast<DenoiseMode>(i)));
    }
}

// 初始化 comboBox_InteractionMode 下拉框
void initComboBoxInteractionMode(QComboBox *comboBox) {
    // 创建一个 map 来存储权重模式和描述
    std::map<WeightMode, std::string> weightModes = {
                                                 {NoWeight, "无权重"},
                                                 {LinearWeight, "线性权重"},
                                                 {ExponentialWeight, "指数衰减权重"},
                                                 {GaussianWeight, "高斯权重"}
    };

    // 遍历 map 并添加到 comboBox
    for (const auto &mode : weightModes) {
        comboBox->addItem(QString::fromStdString(mode.second), mode.first);
    }
}
