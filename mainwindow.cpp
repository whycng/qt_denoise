#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "filter_base2.h"
#include "base_func.h"
#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化 comboBox_denoiseMode 下拉框
    // 假设 ui 是已经初始化的 UI 指针
    initComboBoxDenoiseMode(ui->comboBox_denoiseMode);
    // 初始默认值
    ui->lineEdit_vmax->setText("0.8");
    ui->lineEdit_vmin->setText("0.2");

    //qcustomplot框
    hLayout = new QHBoxLayout(ui->widget_pic);

    //限定输入为0-1之内的浮点数
    QDoubleValidator *validator = new QDoubleValidator(0.0, 1.0, 6, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEdit_vmax->setValidator(validator);
    ui->lineEdit_vmin->setValidator(validator);
}

MainWindow::~MainWindow()
{
    delete ui;
}



//读取数据
void MainWindow::on_pushButton_readData_clicked()
{

    QString fileFilter = tr("LAS Files (*.las);;All Files (*.*)");
    QString fileNameLas = QFileDialog::getOpenFileName(this,
        tr("打开LAS文件"), "", fileFilter);
    if (fileNameLas.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("未选择文件"));
        return;
    }

    std::string filePath = fromQString2stdString(fileNameLas);//fileNameLas.toStdString();
    std::vector<std::vector<float>> outPutData;

    // 读取文件
    m_data = readLasFile(filePath);

    // 在这里处理读取到的数据
    if( m_data.empty() == true)
    {
        qDebug() << "[T] EMPTY data";
    }
    else
    {
        qDebug() << "[T] data.size:" << m_data.size() << " " << m_data.at(0).size();
    }

    //数据归一化
    Normalize2DVector(m_data);
}



//执行去噪
void MainWindow::on_pushButton_denoisePrc_clicked()
{
    std::vector<std::vector<float>> outPutData;
    const int mode = ui->comboBox_denoiseMode->currentIndex();
    switch(mode){
        case SlidingAverageFilter: {//滑动平均法滤波
            slidingAverageFilter(m_data, outPutData);
            break;
        }
        case MedianFilter: {//中值滤波
            medianFilter(m_data, outPutData, 5);
            break;
        }
        case GaussianFilter: {//高斯滤波
            gaussianFilter(m_data, outPutData, 11);
            break;
        }
        case WaveletDenoise: {//小波
            wavelet3(m_data,outPutData);
            break;
        }
        case ZeroPhaseFilter2D: {//零相位滤波
            std::vector<float> b = { 0.25, 0.5, 0.25 };
            zeroPhaseFilter2D(m_data, outPutData, b);
            break;
        }
        case SavitzkyGolayFilter: {//Savitzky-Golay 滤波
            applySavitzkyGolay(m_data, outPutData, 5, 2);
            break;
        }
        case MadFilter: {//mad滤波
            madFilter(m_data, outPutData);
            break;
        }
        default: {
            qWarning("未定义的去噪模式");
            return;
        }
    }
    qDebug() << "去噪完成 denoist over" << outPutData.size() << " "
        << outPutData.at(0).size();

    const float vmax = ui->lineEdit_vmax->text().toFloat();
    const float vmin = ui->lineEdit_vmin->text().toFloat();

    plot_den(outPutData,hLayout,vmax,vmin);
}


