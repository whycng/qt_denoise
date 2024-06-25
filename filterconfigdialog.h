#ifndef FILTERCONFIGDIALOG_H
#define FILTERCONFIGDIALOG_H

#include "qdebug.h"
#include <QDialog>
#include <QDialog>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <iostream>
#include <vector>

struct FilterConfig {
    int slidingWindowSize;//滑动滤波，窗口大小
    int medianWindowSize;//中值滤波，窗口
    int gaussianKernelSize;//高斯滤波，窗口
    int waveletLevel;//小波，窗口
    std::vector<float> zeroPhaseCoefficients;//零相位滤波
    int savitzkyGolayWindowSize;//savitzkyGolay
    int savitzkyGolayPolynomialOrder;//savitzkyGolay
    float madThreshold;//mad滤波  倍数

};


class FilterConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit FilterConfigDialog(QWidget *parent = nullptr);
    FilterConfig getConfig() const;
    void setConfig(const FilterConfig& config);

private:
    FilterConfig m_config;
    QSpinBox *slidingWindowSizeSpinBox;
    QSpinBox *medianWindowSizeSpinBox;
    QSpinBox *gaussianKernelSizeSpinBox;
    QSpinBox *waveletLevelSpinBox;
    QSpinBox *savitzkyGolayWindowSizeSpinBox;
    QSpinBox *savitzkyGolayPolynomialOrderSpinBox;
    QDoubleSpinBox *madThresholdSpinBox;
};

//ui的方式

//namespace Ui {
//class FilterConfigDialog;
//}

//class FilterConfigDialog : public QDialog
//{
//    Q_OBJECT

//public:
//    explicit FilterConfigDialog(QWidget *parent = nullptr);
//    ~FilterConfigDialog();

//    FilterConfig getConfig() const;


//private:
//    Ui::FilterConfigDialog *ui;
//};

#endif // FILTERCONFIGDIALOG_H
