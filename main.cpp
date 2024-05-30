#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <qDebug>

#include<iostream>
#include <random>
#include <cmath>
#include "filter_base2.h"
#include "base_func.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "denoise_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();

//    // Example usage
//    std::vector<std::vector<float>> data = {
//        {-1.0f, 2.0f, 3.0f},
//        {4.0f, 5.0f, 6.0f},
//        {-7.0f, 8.0f, 9.0f}
//    };

//    Normalize2DVector(data);

//    // Print normalized data
//    for (const auto& row : data) {
//        for (const auto& elem : row) {
//            std::cout << elem << " ";
//        }
//        std::cout << std::endl;
//    }



    /*数据说明
第1列：深度
第2~第257列：数据体，共256列
*/

//    const std::string fileDir = R"(E:\XueXiao\比赛-中国石油集团测井第三届校园创意大赛\波形数据-群\Image Data (2)\)";
//    const std::string fileNameLas = R"(Image Data .las)";
//    const std::string filePath = fileDir + fileNameLas;
//    std::vector<std::vector<float>> outPutData;
    // 读取文件
    //std::vector<std::vector<float>> data = readLasFile(filePath);
    // 归一化data
    //normalize(data);
    //saveFile(fileDir + "oriData.txt", data);

    //高斯滤波
    //gaussianFilter(data, outPutData, 11);
    ////。。。
    //saveFile(fileDir + "outputGaussianFilter.txt", outPutData);

    //中值滤波
    //medianFilter(data, outPutData, 5);
    //thresholdFilter(outPutData, 0.53,0.35);
    //saveFile(fileDir + "outputMedianFilter.txt", outPutData);

    //维纳滤波--测试
    //testVienwerFilter(fileDir, data);

    //滑动平均法滤波
    //slidingAverageFilter(data, outPutData);
    //saveFile(fileDir + "outputSlidAvg.txt", outPutData);

    //零相位滤波
    // 定义滤波器系数
    //std::vector<float> b = { 0.25, 0.5, 0.25 };
    //zeroPhaseFilter2D(data, outPutData, b);
    //saveFile(fileDir + "outputZeroPhase.txt", outPutData);

    //Savitzky-Golay 滤波
    //applySavitzkyGolay(data, outPutData, 5, 2);
    //saveFile(fileDir + "outputSavitzkyGolay.txt", outPutData);

    //mad滤波
    //madFilter(data, outPutData);
    //saveFile(fileDir + "outputMad.txt", outPutData);

    //小波去噪
    //wavelet3(data);
    /*medianFilter(data, outPutData, 5);
    saveFile(fileDir + "outputMedianFilter.txt", outPutData);*/

    qDebug() << " OVER " ;

    return a.exec();
}
