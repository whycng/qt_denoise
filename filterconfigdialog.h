#ifndef FILTERCONFIGDIALOG_H
#define FILTERCONFIGDIALOG_H

#include <QDialog>
#include <QDialog>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <iostream>
#include <vector>

struct FilterConfig {
    int slidingWindowSize;
    int medianWindowSize;
    int gaussianKernelSize;
    int waveletLevel;
    std::vector<float> zeroPhaseCoefficients;
    int savitzkyGolayWindowSize;
    int savitzkyGolayPolynomialOrder;
    float madThreshold;

    void print() const {
        std::cout << "Sliding Window Size: " << slidingWindowSize << "\n"
                  << "Median Window Size: " << medianWindowSize << "\n"
                  << "Gaussian Kernel Size: " << gaussianKernelSize << "\n"
                  << "Wavelet Level: " << waveletLevel << "\n"
                  << "Zero Phase Coefficients: ";
        for (float coeff : zeroPhaseCoefficients) {
            std::cout << coeff << " ";
        }
        std::cout << "\n"
                  << "Savitzky-Golay Window Size: " << savitzkyGolayWindowSize << "\n"
                  << "Savitzky-Golay Polynomial Order: " << savitzkyGolayPolynomialOrder << "\n"
                  << "MAD Threshold: " << madThreshold << "\n";
    }
};


class FilterConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit FilterConfigDialog(QWidget *parent = nullptr);
    FilterConfig getConfig() const;

private:
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
