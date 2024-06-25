#include "filterconfigdialog.h"
//#include "ui_filterconfigdialog.h"

// FilterConfigDialog.cpp
#include "FilterConfigDialog.h"

FilterConfigDialog::FilterConfigDialog(QWidget *parent)
    : QDialog(parent),
    slidingWindowSizeSpinBox(new QSpinBox(this)),
    medianWindowSizeSpinBox(new QSpinBox(this)),
    gaussianKernelSizeSpinBox(new QSpinBox(this)),
    waveletLevelSpinBox(new QSpinBox(this)),
    savitzkyGolayWindowSizeSpinBox(new QSpinBox(this)),
    savitzkyGolayPolynomialOrderSpinBox(new QSpinBox(this)),
    madThresholdSpinBox(new QDoubleSpinBox(this) )
{

    QVBoxLayout *layout = new QVBoxLayout(this);

    // 设置初始值  不写在构造函数里...
//    slidingWindowSizeSpinBox->setValue(3);//滑动平均法滤波
//    medianWindowSizeSpinBox->setValue(5);//中值滤波
//    gaussianKernelSizeSpinBox->setValue(5);//高斯滤波
//    waveletLevelSpinBox->setValue(2);//小波 --
//    savitzkyGolayWindowSizeSpinBox->setValue(5);//Savitzky-Golay 滤波
//    savitzkyGolayPolynomialOrderSpinBox->setValue(2);
//    madThresholdSpinBox->setValue(3.0);//mad --

    layout->addWidget(new QLabel("Sliding Window Size:", this));
    layout->addWidget(slidingWindowSizeSpinBox);

    layout->addWidget(new QLabel("Median Window Size:", this));
    layout->addWidget(medianWindowSizeSpinBox);

    layout->addWidget(new QLabel("Gaussian Kernel Size:", this));
    layout->addWidget(gaussianKernelSizeSpinBox);

    layout->addWidget(new QLabel("Wavelet Level:", this));
    layout->addWidget(waveletLevelSpinBox);

    layout->addWidget(new QLabel("Savitzky-Golay Window Size:", this));
    layout->addWidget(savitzkyGolayWindowSizeSpinBox);

    layout->addWidget(new QLabel("Savitzky-Golay Polynomial Order:", this));
    layout->addWidget(savitzkyGolayPolynomialOrderSpinBox);

    layout->addWidget(new QLabel("MAD Threshold:", this));
    layout->addWidget(madThresholdSpinBox);

    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Cancel", this);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    layout->addWidget(okButton);
    layout->addWidget(cancelButton);

    setLayout(layout);
}

FilterConfig FilterConfigDialog::getConfig() const {
    FilterConfig config;
    config.slidingWindowSize = slidingWindowSizeSpinBox->value();
    config.medianWindowSize = medianWindowSizeSpinBox->value();
    config.gaussianKernelSize = gaussianKernelSizeSpinBox->value();
    config.waveletLevel = waveletLevelSpinBox->value();
    config.zeroPhaseCoefficients = { 0.25f, 0.5f, 0.25f };  // 根据实际情况修改
    config.savitzkyGolayWindowSize = savitzkyGolayWindowSizeSpinBox->value();
    config.savitzkyGolayPolynomialOrder = savitzkyGolayPolynomialOrderSpinBox->value();
    config.madThreshold = madThresholdSpinBox->value();
    return config;
}

void  FilterConfigDialog::setConfig(const FilterConfig& config)  {
    m_config = config;
    slidingWindowSizeSpinBox->setValue(m_config.slidingWindowSize);//滑动平均法滤波
    medianWindowSizeSpinBox->setValue(m_config.medianWindowSize);//中值滤波
    gaussianKernelSizeSpinBox->setValue(m_config.gaussianKernelSize);//高斯滤波
    waveletLevelSpinBox->setValue(m_config.waveletLevel);//小波 --
    savitzkyGolayWindowSizeSpinBox->setValue(m_config.savitzkyGolayWindowSize);//Savitzky-Golay 滤波
    savitzkyGolayPolynomialOrderSpinBox->setValue(m_config.savitzkyGolayPolynomialOrder);
    madThresholdSpinBox->setValue(m_config.madThreshold);//mad --

}





//FilterConfigDialog::FilterConfigDialog(QWidget *parent) :
//    QDialog(parent),
//    ui(new Ui::FilterConfigDialog)
//{
//    ui->setupUi(this);
//}

//FilterConfigDialog::~FilterConfigDialog()
//{
//    delete ui;
//}

//FilterConfig FilterConfigDialog::getConfig() const {
//    FilterConfig config;
////    config.slidingWindowSize = ui->slidingWindowSizeSpinBox->value();
////    config.medianWindowSize = ui->medianWindowSizeSpinBox->value();
////    config.gaussianKernelSize = ui->gaussianKernelSizeSpinBox->value();
////    config.waveletLevel = ui->waveletLevelSpinBox->value();
////    // 处理零相位滤波器系数
////    config.zeroPhaseCoefficients = { 0.25f, 0.5f, 0.25f };  // 根据实际情况修改
////    config.savitzkyGolayWindowSize = ui->savitzkyGolayWindowSizeSpinBox->value();
////    config.savitzkyGolayPolynomialOrder = ui->savitzkyGolayPolynomialOrderSpinBox->value();
////    config.madThreshold = ui->madThresholdSpinBox->value();
//    return config;
//}
