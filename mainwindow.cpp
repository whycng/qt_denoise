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
    m_radius = 10;

    //qcustomplot框
    hLayout = new QHBoxLayout(ui->widget_pic);
    m_customPlot_den = new QCustomPlot();

    //限定输入为0-1之内的浮点数
    QDoubleValidator *validator = new QDoubleValidator(0.0, 1.0, 6, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEdit_vmax->setValidator(validator);
    ui->lineEdit_vmin->setValidator(validator);

//    // Enable mouse tracking to capture move events even when no button is pressed
//    m_customPlot_den->setMouseTracking(true);

//    //qDebug() << "Default layer name:" << m_customPlot_den->layer(6)->name(); // 通常会输出 "main"
//    // Add custom layer for drawing the red circle
//    m_customPlot_den->addLayer("TST", m_customPlot_den->layer("main"),
//                               QCustomPlot::limAbove);
//    //m_customPlot_den->setCurrentLayer("TST");//设置当前图层
//    m_circle = new QCPItemEllipse(m_customPlot_den);
//    m_circle->setPen(QPen(Qt::red));
//    m_circle->setLayer("TST");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::denoiseDataInCircle() {
    QVector<double> x = m_customPlot_den->graph(0)->data()->keys().toVector();
    QVector<double> y = m_customPlot_den->graph(0)->data()->values().toVector();
    QPointF center = m_customPlot_den->plotArea().topLeft() + QPointF(m_mousePos.x(), m_mousePos.y());
    double centerX = m_customPlot_den->xAxis->pixelToCoord(center.x());
    double centerY = m_customPlot_den->yAxis->pixelToCoord(center.y());

    // 参数化的平滑范围
    int smoothingRange = 2;  // 可调整的平滑范围
    int range = 5; // 可调整的范围大小

    // 用于存储平滑后的数据
    QVector<double> smoothedY(y.size());

    // 使用滑动窗口算法进行平滑处理
    double sum = 0;
    int windowSize = 2 * smoothingRange + 1;

    // 初始化滑动窗口的总和
    for (int i = 0; i < windowSize && i < y.size(); ++i) {
        sum += y[i];
    }

    // 遍历数据点进行平滑处理
    for (int i = 0; i < y.size(); ++i) {
        if (i > smoothingRange) {
            sum -= y[i - smoothingRange - 1];
        }
        if (i + smoothingRange < y.size()) {
            sum += y[i + smoothingRange];
        }

        smoothedY[i] = sum / std::min(windowSize, i + smoothingRange + 1) - std::max(0, i - smoothingRange);
    }

    // 更新图表数据
    m_customPlot_den->graph(0)->setData(x, smoothedY);
    m_customPlot_den->replot();
}
void MainWindow::updateRedCircle() {
    //    if (!m_customPlot_den->viewport().contains(m_mousePos)) {
    //        m_circle->setVisible(false);
    //        return;
    //    }

    m_circle->setVisible(true);// 显示圆圈
    QPointF customPlotPos = m_customPlot_den->mapFromParent(m_mousePos);
    double x = m_customPlot_den->xAxis->pixelToCoord(customPlotPos.x());
    double y = m_customPlot_den->yAxis->pixelToCoord(customPlotPos.y());
    m_circle->topLeft->setCoords(x - m_radius, y + m_radius);
    m_circle->bottomRight->setCoords(x + m_radius, y - m_radius);
}

//鼠标按下事件处理
void MainWindow::onMousePress(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        //m_customPlot_den->setCurrentLayer("TST");
        m_selecting = true;
        m_startPoint = event->pos();
        m_endPoint = m_startPoint;
        //qDebug() << "<onMousePress> m_startPoint:" << m_startPoint;
    }
}

//鼠标移动事件处理
void MainWindow::onMouseMove(QMouseEvent *event) {
    if (m_selecting && event->buttons() & Qt::RightButton) {
        m_endPoint = event->pos();
        //m_customPlot_den->replot();
        //qDebug() << "<onMouseMove> m_endPoint:" << m_endPoint;

        // Update mouse position and redraw the red circle
        m_mousePos = event->pos();
        //updateRedCircle();
        m_customPlot_den->replot(); // Trigger replot to update the plot
    }
}


//鼠标释放事件处理
void MainWindow::onMouseRelease(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        m_selecting = false;
        m_endPoint = event->pos();
        //performDenoising();
        m_customPlot_den->replot();
        //qDebug() << "<onMouseRelease> m_endPoint:" << m_endPoint;
        //m_customPlot_den->setCurrentLayer("main");
    }
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

    //问题来了，放入plot_den？--tmp
    //先生成一个新的m_customPlot_den，再填入hLayout
    if( nullptr != m_customPlot_den)
    {
            delete m_customPlot_den;
            m_customPlot_den = nullptr;
    }
    m_customPlot_den = new QCustomPlot();
    //布局设置
    // 检查g_custLayout_priWell中是否存在小部件
    QLayoutItem* existingItem = hLayout->takeAt(0);
    if (existingItem) {
            QWidget* existingWidget = existingItem->widget();
            if (existingWidget) {
                // 删除已存在的小部件
                delete existingWidget;
            }
            delete existingItem;
    }
    hLayout->addWidget(m_customPlot_den);

    //用鼠标按下和释放去控制回到main层
    //m_customPlot_den->setCurrentLayer("main");
    //绘制
    plot_den(outPutData,hLayout,vmax,vmin,m_customPlot_den);

    QObject::connect(m_customPlot_den, &QCustomPlot::mousePress, this, &MainWindow::onMousePress);
    QObject::connect(m_customPlot_den, &QCustomPlot::mouseMove, this, &MainWindow::onMouseMove);
    QObject::connect(m_customPlot_den, &QCustomPlot::mouseRelease, this, &MainWindow::onMouseRelease);
}


