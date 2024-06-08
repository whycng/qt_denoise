#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "filter_base2.h"
#include "base_func.h"
#include "qcustomplot.h"
#include "CustomDepthDialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化 comboBox_denoiseMode 下拉框
    // 假设 ui 是已经初始化的 UI 指针
    initComboBoxDenoiseMode(ui->comboBox_denoiseMode);
    initComboBoxInteractionMode(ui->comboBox_InteractionMode);
    // 初始默认值
    ui->lineEdit_vmax->setText("0.8");
    ui->lineEdit_vmin->setText("0.2");
    ui->lineEdit_threshold->setText("0.1");
    ui->lineEdit_radius->setText("0.5");
    ui->checkBox_rightClick->setChecked(Qt::Unchecked);
    ui->checkBox_rightSlider->setChecked(Qt::Unchecked);
    ui->checkBox_whichone->setChecked(Qt::Checked);
    ui->checkBox_rect->blockSignals(true);  // 禁用信号
    ui->checkBox_rect->setChecked(Qt::Unchecked);
    ui->checkBox_rect->blockSignals(false);  // 启用信号
    ui->comboBox_calDenoiseVal->addItem("均值去噪");
    ui->comboBox_calDenoiseVal->addItem("中值去噪");
    ui->comboBox_calDenoiseVal->addItem("高斯滤波去噪");
    m_radius = 10;

    m_config.slidingWindowSize = 3;//滑动滤波，窗口大小
    m_config.medianWindowSize = 5;//中值滤波，窗口
    m_config.gaussianKernelSize = 5;//高斯滤波，窗口
    m_config.waveletLevel = 4;//小波，窗口
    m_config.zeroPhaseCoefficients = { 0.25f, 0.5f, 0.25f };//零相位滤波
    m_config.savitzkyGolayWindowSize = 5;//savitzkyGolay
    m_config.savitzkyGolayPolynomialOrder = 2 ;//savitzkyGolay

    //qcustomplot框
    hLayout = new QHBoxLayout(ui->widget_pic);
    m_customPlot_den = new QCustomPlot();


    //限定输入为0-1之内的浮点数
    QDoubleValidator *validator1 = new QDoubleValidator(0.0, 1.0, 6, this);
    validator1->setNotation(QDoubleValidator::StandardNotation);
    ui->lineEdit_vmax->setValidator(validator1);
    ui->lineEdit_vmin->setValidator(validator1);
    ui->lineEdit_threshold->setValidator(validator1);

    // 创建一个 QDoubleValidator 对象
    QDoubleValidator *validator2 = new QDoubleValidator(0.0, std::numeric_limits<double>::max(), 2, this);
    // 设置验证器的属性
    validator2->setNotation(QDoubleValidator::StandardNotation);  // 标准表示法
    validator2->setDecimals(2);  // 设置小数点后的位数
    // 将验证器应用到 QLineEdit
    ui->lineEdit_radius->setValidator(validator2);

    connect(this,&MainWindow::progressUpdate,this,&MainWindow::onProgressUpdate);


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

void MainWindow::onProgressUpdate(int val)
{

    qDebug() << " onProgressUpdate:" << val;
    if( 1 == val)
    {
        // 创建进度条
        if( nullptr != progressBar )
        {
            delete progressBar;
            progressBar = nullptr;
        }
        progressBar = new QProgressDialog();
        progressBar->setWindowTitle("数据加载中...");
        progressBar->setRange(0, 100);
        progressBar->setValue(0);
        progressBar->setMinimumSize(300, 50);

        progressBar->show();
    }
    else if( val < 10 )
    {
        progressBar->setValue(val * 10);
        QCoreApplication::processEvents();  // 确保事件循环处理事件
    }
    else
    {
        progressBar->setValue(val * 10);
        progressBar->close();
    }

};


//绘制图像
void MainWindow::plot_den(const std::vector<std::vector<float>>& outData,QHBoxLayout* hLayout
              ,const float& vmax, const float& vmin
              ,QCustomPlot* customPlot_den
              ,const bool& rectMode)
{
    if( true == outData.empty())
    {
        qDebug() << " EMPTY oudata";
        return;
    }


    const int row = outData.size();
    const int col = outData.at(0).size();
    QCPColorMap* colorMap = new QCPColorMap(customPlot_den->xAxis, customPlot_den->yAxis);
    m_colorMap = colorMap;

    colorMap->data()->setSize(row, col);
    for (int i = 0  ; i < row; ++i )
    {
        for (int j = 0; j < col; ++j)
        {
            colorMap->data()->setCell(i, j, outData.at(i).at(j));
        }
    }
    // 设置颜色映射
    QCPColorGradient colorGradient;
    colorGradient.loadPreset(QCPColorGradient::gpJet);
    colorMap->setGradient(colorGradient);
    colorMap->setInterpolate(false);
    colorMap->setTightBoundary(true);
    // 设置颜色映射的上下限
    colorMap->setDataRange(QCPRange(vmin, vmax));

    //customPlot_den->xAxis->setLabel(" "); // 设置横坐标标签
    //customPlot_den->yAxis->setLabel(" "); // 设置纵坐标标签
    //customPlot_den->yAxis->setRangeReversed(true);// 设置纵坐标反向
    customPlot_den->axisRect()->setupFullAxesBox();//四边安装轴并显示
    //customPlot_priW_den->legend->setVisible(true); // 显示图例
    //customPlot_den->rescaleAxes();
    customPlot_den->replot();

    //交互方式
    if( true == rectMode)
    {
        customPlot_den->setInteractions(QCP::iRangeZoom);
    }
    else
    {
        customPlot_den->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    }
}



//交互去噪 --点值 点击去噪
void MainWindow::denoiseDataInCircle(QPoint mousePos) {

    qDebug() << " <denoiseDataInCircle> 执行";

//    const float vmax = ui->lineEdit_vmax->text().toFloat();
//    const float vmin = ui->lineEdit_vmin->text().toFloat();
    const float threshold = ui->lineEdit_threshold->text().toFloat();

    // 将鼠标像素坐标转换为轴坐标
    double xCoord = m_customPlot_den->xAxis->pixelToCoord(mousePos.x());
    double yCoord = m_customPlot_den->yAxis->pixelToCoord(mousePos.y());

    qDebug() << " xCoord:" << xCoord << " yCoord:" << yCoord;

    // 收集选中数据点的值及其索引
    std::vector<std::pair<int, int>> indices;
    std::vector<float> values;

    const int row = m_outPutData.size();
    const int col = m_outPutData.at(0).size();

    // 获取ColorMap的实际尺寸范围
//    const double xMin = m_customPlot_den->xAxis->range().lower;
//    const double xMax = m_customPlot_den->xAxis->range().upper;
//    const double yMin = m_customPlot_den->yAxis->range().lower;
//    const double yMax = m_customPlot_den->yAxis->range().upper;
    const double xrange = m_customPlot_den->xAxis->range().size();

//    const double xStep = (xMax - xMin) / (row - 1);
//    const double yStep = (yMax - yMin) / (col - 1);

    const double scaleX = xrange/m_xrange;
    const float radius = ui->lineEdit_radius->text().toFloat() * scaleX;

    //qDebug() << " xMin:" << xMin << " xMax:" << xMax << " xStep:" << xStep;

    if (!m_colorMap) {
        qDebug() << "colorMap is not initialized.";
        return;
    }
    QCPColorMapData* data = m_colorMap->data();

    int keySize = data->keySize();
    int valueSize = data->valueSize();

    int xMin, xMax, yMin, yMax;
    data->coordToCell(xCoord - radius, yCoord - radius, &xMin, &yMin);
    data->coordToCell(xCoord + radius, yCoord + radius, &xMax, &yMax);

    // 确保索引在有效范围内
    int xMinIndex = std::max(0, xMin);
    int xMaxIndex = std::min(keySize - 1, xMax);
    int yMinIndex = std::max(0, yMin);
    int yMaxIndex = std::min(valueSize - 1, yMax);

    //获取权重模式
    WeightMode wmode = static_cast<WeightMode>(ui->comboBox_InteractionMode->currentIndex());

    //获取somenoise
    // 计算去噪值
    double someDenoisedValue = 0.0;
    const bool inverseWeight = ui->checkBox_whichone->isChecked();
    const int calmode = ui->comboBox_calDenoiseVal->currentIndex();
    qDebug() << "calmode:" << calmode;
    if (calmode == 0) {
        // 均值去噪
        double sum = 0.0;
        int count = 0;
        for (int xi = xMinIndex; xi <= xMaxIndex; ++xi) {
            for (int yi = yMinIndex; yi <= yMaxIndex; ++yi) {
                double x_inner, y_inner;
                data->cellToCoord(xi, yi, &x_inner, &y_inner);
                double dist_inner = std::sqrt(std::pow(x_inner - xCoord, 2) + std::pow(y_inner - yCoord, 2));
                if (dist_inner <= radius) {
                    sum += m_outPutData[xi][yi];
                    count++;
                }
            }
        }
        someDenoisedValue = (count > 0) ? (sum / count) : 0.0;
    } else if (calmode == 1) {
        // 中值去噪
        std::vector<double> neighborhoodValues;
        for (int xi = xMinIndex; xi <= xMaxIndex; ++xi) {
            for (int yi = yMinIndex; yi <= yMaxIndex; ++yi) {
                double x_inner, y_inner;
                data->cellToCoord(xi, yi, &x_inner, &y_inner);
                double dist_inner = std::sqrt(std::pow(x_inner - xCoord, 2) + std::pow(y_inner - yCoord, 2));
                if (dist_inner <= radius) {
                    neighborhoodValues.push_back(m_outPutData[xi][yi]);
                }
            }
        }
        if (!neighborhoodValues.empty()) {
            std::sort(neighborhoodValues.begin(), neighborhoodValues.end());
            someDenoisedValue = neighborhoodValues[neighborhoodValues.size() / 2];
        }
    } else if (calmode == 2) {
        // 高斯滤波去噪
        double sum = 0.0;
        double weightSum = 0.0;
        for (int xi = xMinIndex; xi <= xMaxIndex; ++xi) {
            for (int yi = yMinIndex; yi <= yMaxIndex; ++yi) {
                double x_inner, y_inner;
                data->cellToCoord(xi, yi, &x_inner, &y_inner);
                double dist_inner = std::sqrt(std::pow(x_inner - xCoord, 2) + std::pow(y_inner - yCoord, 2));
                if (dist_inner <= radius) {
                    double weight = std::exp(-std::pow(dist_inner / radius, 2));
                    sum += m_outPutData[xi][yi] * weight;
                    weightSum += weight;
                }
            }
        }
        someDenoisedValue = (weightSum > 0) ? (sum / weightSum) : 0.0;
    }
    qDebug() << "someDenoisedValue:" << someDenoisedValue;

    for (int xIndex = xMinIndex; xIndex <= xMaxIndex; ++xIndex) {
        for (int yIndex = yMinIndex; yIndex <= yMaxIndex; ++yIndex) {
            double x, y;
            data->cellToCoord(xIndex, yIndex, &x, &y);
            double distance = std::sqrt(std::pow(x - xCoord, 2) + std::pow(y - yCoord, 2));

            if (distance <= radius) {
                indices.push_back(std::make_pair(xIndex, yIndex));
                values.push_back(m_outPutData[xIndex][yIndex]);

                double weight = 1;

                switch (wmode) {
                case WeightMode::NoWeight:
                    weight = 1;  // NoWeight 模式下，权重为1
                    break;
                case WeightMode::LinearWeight:
                     weight = inverseWeight ? (distance / radius) : (1.0 - (distance / radius));  // 线性权重
                    break;
                case WeightMode::ExponentialWeight:
                    weight = inverseWeight ? (1 - std::exp(-distance / radius)) : std::exp(-distance / radius); // 指数衰减权重
                    break;
                case WeightMode::GaussianWeight:
                    weight = inverseWeight ? (1 - std::exp(-std::pow(distance / radius, 2))) : std::exp(-std::pow(distance / radius, 2)); // 高斯权重
                    break;
                default:
                    weight = 1;  // 默认权重为1，确保程序不会崩溃
                    break;
                }

                m_outPutData[xIndex][yIndex] = m_outPutData[xIndex][yIndex] * weight + someDenoisedValue * (1 - weight);

            }
        }
    }

    //------------------------------------------------------------------------

    //这种方法可以，但遍历整个网格效率低下
//    for (int xIndex = 0; xIndex < data->keySize(); ++xIndex) {
//        for (int yIndex = 0; yIndex < data->valueSize(); ++yIndex) {
//            double x, y;
//            data->cellToCoord(xIndex, yIndex, &x, &y);
//            //double value = data->cell(xIndex, yIndex);
//            // 计算距离
//            double distance = std::sqrt(std::pow(x - xCoord, 2) + std::pow(y - yCoord, 2));

//            if(distance <= radius) {
//                indices.push_back(std::make_pair(xIndex, yIndex));
//                values.push_back(m_outPutData[xIndex][yIndex]);
//            }
//            //qDebug() << "Point (" << x << "," << y << ") has value:" << value;
//        }
//    }



    //-----------------------------------------------------------------------------

    //这种方式不能解决移动和缩放后进行交互去噪的问题，位置变化了
    // 遍历m_outPutData，找到在指定范围内的数据点
//    for(int i = 0; i < row; ++i) {
//        for(int j = 0; j < col; ++j) {
//            double dataX = xMin + i * xStep + m_xoffset;
//            double dataY = yMin + j * yStep + m_yoffset;
//            //double dataX = m_customPlot_den->xAxis->coordToPixel(xMin + i * xStep);
//            //double dataY = m_customPlot_den->yAxis->coordToPixel(yMin + j * yStep);

//            //qDebug() << " dataX:" << dataX << " dataX2:" << dataX2;

//            // 计算距离
//            double distance = std::sqrt(std::pow(dataX - xCoord, 2) + std::pow(dataY - yCoord, 2));

//            if(distance <= radius) {
//                //qDebug() << "dataX:" << dataX <<" xCoord:" << xCoord
//                //         << " dataY:" << dataY << " yCoord:" << yCoord;
//                indices.push_back(std::make_pair(i, j));
//                values.push_back(m_outPutData[i][j]);
//            }
//        }
//    }

    // 添加新的元素到容器尾部
    g_recentVal.push_back({indices,values});

    //阈值去噪
    if( WeightMode::NoWeight == wmode)
    {
        theshorldDenoise(values,indices, threshold, m_outPutData );
    }


}

//交互去噪 --滑动去噪 先记录坐标点
void MainWindow::denoiseDataSlideCircle(QPoint mousePos) {

    // 将鼠标像素坐标转换为轴坐标
    double xCoord = m_customPlot_den->xAxis->pixelToCoord(mousePos.x());
    double yCoord = m_customPlot_den->yAxis->pixelToCoord(mousePos.y());

    //qDebug() << " xCoord:" << xCoord << " yCoord:" << yCoord;

    // 收集选中数据点的值及其索引
    std::vector<std::pair<int, int>> indices;
    std::vector<float> values;

    const double xrange = m_customPlot_den->xAxis->range().size();

    const double scaleX = xrange/m_xrange;
    const float radius = ui->lineEdit_radius->text().toFloat() * scaleX;

    if (!m_colorMap) {
        qDebug() << "colorMap is not initialized.";
        return;
    }
    QCPColorMapData* data = m_colorMap->data();

    int keySize = data->keySize();
    int valueSize = data->valueSize();

    int xMin, xMax, yMin, yMax;
    data->coordToCell(xCoord - radius, yCoord - radius, &xMin, &yMin);
    data->coordToCell(xCoord + radius, yCoord + radius, &xMax, &yMax);

    // 确保索引在有效范围内
    int xMinIndex = std::max(0, xMin);
    int xMaxIndex = std::min(keySize - 1, xMax);
    int yMinIndex = std::max(0, yMin);
    int yMaxIndex = std::min(valueSize - 1, yMax);

    for (int xIndex = xMinIndex; xIndex <= xMaxIndex; ++xIndex) {
        for (int yIndex = yMinIndex; yIndex <= yMaxIndex; ++yIndex) {
            double x, y;
            data->cellToCoord(xIndex, yIndex, &x, &y);
            double distance = std::sqrt(std::pow(x - xCoord, 2) + std::pow(y - yCoord, 2));

            if (distance <= radius) {
                indices.push_back(std::make_pair(xIndex, yIndex));
                values.push_back(m_outPutData[xIndex][yIndex]);
                // 添加新元素，用来鼠标释放的时候排序计算处理
                g_slidIndices.push_back(std::make_pair(xIndex, yIndex));
                g_slidValues.push_back(m_outPutData[xIndex][yIndex]);
            }
        }
    }
    // 添加新的元素到容器尾部 用来恢复上一步
    g_recentVal.push_back({indices,values});

}

void MainWindow::applyFilterToSelection(const double& xCoordLB, const double& yCoordLB,
                                        const double& xCoordRT, const double& yCoordRT)
{
    // 中心点
    const double xCoord = (xCoordLB + xCoordRT) / 2;
    const double yCoord = (yCoordLB + yCoordRT) / 2;

    const float threshold = ui->lineEdit_threshold->text().toFloat();

    // 收集选中数据点的值及其索引
    std::vector<std::pair<int, int>> indices;
    std::vector<float> values;

    if (!m_colorMap) {
        qDebug() << "colorMap is not initialized.";
        return;
    }

    QCPColorMapData* data = m_colorMap->data();

    int keySize = data->keySize();
    int valueSize = data->valueSize();

    int xMin, xMax, yMin, yMax;
    data->coordToCell(xCoordLB, yCoordLB, &xMin, &yMin);
    data->coordToCell(xCoordRT, yCoordRT, &xMax, &yMax);

    // 确保索引在有效范围内
    int xMinIndex = std::max(0, xMin);
    int xMaxIndex = std::min(keySize - 1, xMax);
    int yMinIndex = std::max(0, yMin);
    int yMaxIndex = std::min(valueSize - 1, yMax);

    // 获取权重模式
    WeightMode wmode = static_cast<WeightMode>(ui->comboBox_InteractionMode->currentIndex());

    // 获取 someDenoisedValue
    double someDenoisedValue = 0.0;
    const bool inverseWeight = ui->checkBox_whichone->isChecked();
    const int calmode = ui->comboBox_calDenoiseVal->currentIndex();
    qDebug() << "calmode:" << calmode;

    if (calmode == 0) {
        // 均值去噪
        double sum = 0.0;
        int count = 0;
        for (int xi = xMinIndex; xi <= xMaxIndex; ++xi) {
            for (int yi = yMinIndex; yi <= yMaxIndex; ++yi) {
                sum += m_outPutData[xi][yi];
                count++;
            }
        }
        someDenoisedValue = (count > 0) ? (sum / count) : 0.0;
    } else if (calmode == 1) {
        // 中值去噪
        std::vector<double> neighborhoodValues;
        for (int xi = xMinIndex; xi <= xMaxIndex; ++xi) {
            for (int yi = yMinIndex; yi <= yMaxIndex; ++yi) {
                neighborhoodValues.push_back(m_outPutData[xi][yi]);
            }
        }
        if (!neighborhoodValues.empty()) {
            std::sort(neighborhoodValues.begin(), neighborhoodValues.end());
            someDenoisedValue = neighborhoodValues[neighborhoodValues.size() / 2];
        }
    } else if (calmode == 2) {
        // 高斯滤波去噪
        double sum = 0.0;
        double weightSum = 0.0;
        for (int xi = xMinIndex; xi <= xMaxIndex; ++xi) {
            for (int yi = yMinIndex; yi <= yMaxIndex; ++yi) {
                double x_inner, y_inner;
                data->cellToCoord(xi, yi, &x_inner, &y_inner);
                double dist_inner = std::sqrt(std::pow(x_inner - xCoord, 2) + std::pow(y_inner - yCoord, 2));
                double weight = std::exp(-std::pow(dist_inner, 2));
                sum += m_outPutData[xi][yi] * weight;
                weightSum += weight;
            }
        }
        someDenoisedValue = (weightSum > 0) ? (sum / weightSum) : 0.0;
    }

    qDebug() << "someDenoisedValue:" << someDenoisedValue;

    for (int xIndex = xMinIndex; xIndex <= xMaxIndex; ++xIndex) {
        for (int yIndex = yMinIndex; yIndex <= yMaxIndex; ++yIndex) {
            double x, y;
            data->cellToCoord(xIndex, yIndex, &x, &y);

            indices.push_back(std::make_pair(xIndex, yIndex));
            values.push_back(m_outPutData[xIndex][yIndex]);

            double weight = 1;

            switch (wmode) {
            case WeightMode::NoWeight:
                weight = 1;  // NoWeight 模式下，权重为1
                break;
            case WeightMode::LinearWeight:
                weight = inverseWeight ? (std::sqrt(std::pow(x - xCoord, 2) + std::pow(y - yCoord, 2))) : (1.0 - std::sqrt(std::pow(x - xCoord, 2) + std::pow(y - yCoord, 2)));  // 线性权重
                break;
            case WeightMode::ExponentialWeight:
                weight = inverseWeight ? (1 - std::exp(-std::sqrt(std::pow(x - xCoord, 2) + std::pow(y - yCoord, 2)))) : std::exp(-std::sqrt(std::pow(x - xCoord, 2) + std::pow(y - yCoord, 2))); // 指数衰减权重
                break;
            case WeightMode::GaussianWeight:
                weight = inverseWeight ? (1 - std::exp(-std::pow(std::sqrt(std::pow(x - xCoord, 2) + std::pow(y - yCoord, 2)), 2))) : std::exp(-std::pow(std::sqrt(std::pow(x - xCoord, 2) + std::pow(y - yCoord, 2)), 2)); // 高斯权重
                break;
            default:
                weight = 1;  // 默认权重为1，确保程序不会崩溃
                break;
            }

            m_outPutData[xIndex][yIndex] = m_outPutData[xIndex][yIndex] * weight + someDenoisedValue * (1 - weight);
        }
    }

    // 添加新的元素到容器尾部
    g_recentVal.push_back({indices, values});

    // 阈值去噪
    if (WeightMode::NoWeight == wmode)
    {
        theshorldDenoise(values, indices, threshold, m_outPutData);
    }
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
    const float vmax = ui->lineEdit_vmax->text().toFloat();
    const float vmin = ui->lineEdit_vmin->text().toFloat();
    if (event->button() == Qt::RightButton) { //鼠标右键
        //如果勾选了鼠标右键单击去噪模式，则执行
        if( Qt::Checked == ui->checkBox_rightClick->checkState())
        {
            denoiseDataInCircle(event->pos());
            //压入上一步，清空g_recentVal
            qDebug() << "onMouseRelease g_recentVal size:" << g_recentVal.size();
            g_BackVal.push_back(g_recentVal);
            if (g_BackVal.size() == 50)
            {
                g_BackVal.pop_back();
            }
            g_recentVal.clear();
            qDebug() << "PUSH_BACK g_BackVal size:" << g_BackVal.size();
            //绘制
            plot_den(m_outPutData,hLayout,vmax,vmin,m_customPlot_den,g_rectMode);
        }
    }

    if (event->button() == Qt::RightButton) { //鼠标右键
        //m_customPlot_den->setCurrentLayer("TST");
        if( Qt::Checked == ui->checkBox_rightSlider->checkState())
        {//如果选中了滑动交互
            m_selecting = true;
        }

        m_startPoint = event->pos();
        m_endPoint = m_startPoint;

    }
    if (event->button() == Qt::LeftButton && g_rectMode) // 鼠标左键
    {

        dragStartPoint = event->pos();
        isDragging = true;
        // 初始化 selectionRect
        if (selectionRect != nullptr) {
            m_customPlot_den->removeItem(selectionRect); // 从 QCustomPlot 中移除
            //--不知道为啥导致虚函数析构报错，也许是因为qcus内部管理
            //delete selectionRect; // 删除对象
            selectionRect = nullptr; // 避免悬空指针
        }

        selectionRect = new QCPItemRect(m_customPlot_den); // 创建新的 QCPItemRect

        selectionRect->setPen(QPen(Qt::red));
        selectionRect->setBrush(Qt::NoBrush);
        selectionRect->setVisible(false);
        // 初始化 selectionRect
        selectionRect->topLeft->setCoords(m_customPlot_den->xAxis->pixelToCoord(dragStartPoint.x()),
                                          m_customPlot_den->yAxis->pixelToCoord(dragStartPoint.y()));
        selectionRect->bottomRight->setCoords(m_customPlot_den->xAxis->pixelToCoord(dragStartPoint.x()),
                                              m_customPlot_den->yAxis->pixelToCoord(dragStartPoint.y()));
        selectionRect->setVisible(true);
        m_customPlot_den->replot();
    }
}

//鼠标移动事件处理
void MainWindow::onMouseMove(QMouseEvent *event) {

    //m_selecting决定了是否选中了滑动交互
    if (m_selecting && event->buttons() & Qt::RightButton) {
        m_endPoint = event->pos();
        //m_customPlot_den->replot();
        qDebug() << "<onMouseMove> m_endPoint:" << m_endPoint;

        // Update mouse position and redraw the red circle
        m_mousePos = event->pos();
        //updateRedCircle(); --bug
        //如果勾选了鼠标右键点击并滑动去噪模式，则执行
        if( Qt::Checked == ui->checkBox_rightSlider->checkState())
        {
            denoiseDataSlideCircle(event->pos());
        }
    }
    if (event->buttons() & Qt::LeftButton && g_rectMode ) // 鼠标左键
    {
        dragEndPoint = event->pos();
        // 更新 selectionRect
        selectionRect->bottomRight->setCoords(m_customPlot_den->xAxis->pixelToCoord(dragEndPoint.x()),
                                              m_customPlot_den->yAxis->pixelToCoord(dragEndPoint.y()));
        m_customPlot_den->replot();
    }
}


//鼠标释放事件处理
void MainWindow::onMouseRelease(QMouseEvent *event) {

    const float vmax = ui->lineEdit_vmax->text().toFloat();
    const float vmin = ui->lineEdit_vmin->text().toFloat();

    //如果右键 且 滑动交互
    if (m_selecting && event->button() == Qt::RightButton) {
        m_selecting = false;
        m_endPoint = event->pos();

        //压入上一步，清空g_recentVal
        qDebug() << "onMouseRelease g_recentVal size:" << g_recentVal.size();
        g_BackVal.push_back(g_recentVal);
        if (g_BackVal.size() == 50)
        {
            g_BackVal.pop_back();
        }
        g_recentVal.clear();
        qDebug() << "PUSH_BACK g_BackVal size:" << g_BackVal.size();

        //处理滑动去噪那部分的值
        const float threshold = ui->lineEdit_threshold->text().toFloat();
        theshorldDenoise(g_slidValues,g_slidIndices, threshold, m_outPutData);
        //清空
        g_slidIndices.clear();
        g_slidValues.clear();

        //问题来了，放入plot_den？--tmp
        //先生成一个新的m_customPlot_den，再填入hLayout
        /*
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
*/

        qDebug() << "<onMouseRelease> shi fang wan bi m_endPoint:" << m_endPoint;
        m_customPlot_den->setCurrentLayer("main");

        //绘制
        plot_den(m_outPutData,hLayout,vmax,vmin,m_customPlot_den,g_rectMode);
    }
    if (event->button() == Qt::LeftButton && g_rectMode ) // 鼠标左键
    {
        isDragging = false;
        dragEndPoint = event->pos();

        // 更新 selectionRect
        selectionRect->bottomRight->setCoords(m_customPlot_den->xAxis->pixelToCoord(dragEndPoint.x()),
                                              m_customPlot_den->yAxis->pixelToCoord(dragEndPoint.y()));
        m_customPlot_den->replot();

        // 进行去噪处理
        // 这里你需要将拖动选区转换为矩阵的索引范围
        // 获取拖动的起始和结束点的像素坐标
        double x1 = m_customPlot_den->xAxis->pixelToCoord(dragStartPoint.x());
        double y1 = m_customPlot_den->yAxis->pixelToCoord(dragStartPoint.y());
        double x2 = m_customPlot_den->xAxis->pixelToCoord(dragEndPoint.x());
        double y2 = m_customPlot_den->yAxis->pixelToCoord(dragEndPoint.y());

        // 计算左下角 (xLB, yLB) 和右上角 (xRB, yRB) 的坐标
        double xLB = qMin(x1, x2);
        double yLB = qMin(y1, y2);
        double xRT = qMax(x1, x2);
        double yRT = qMax(y1, y2);


        // 假设你已经有一个函数 applyMadFilterToSelection
        applyFilterToSelection(xLB, yLB, xRT, yRT);

        // 清除选区
        selectionRect->setVisible(false);
        //m_customPlot_den->replot();

        //压入上一步，清空g_recentVal
        qDebug() << "onMouseRelease g_recentVal size:" << g_recentVal.size();
        g_BackVal.push_back(g_recentVal);
        if (g_BackVal.size() == 50)
        {
            g_BackVal.pop_back();
        }
        g_recentVal.clear();
        qDebug() << "PUSH_BACK g_BackVal size:" << g_BackVal.size();

        //绘制
        plot_den(m_outPutData,hLayout,vmax,vmin,m_customPlot_den,g_rectMode);
    }


}




//读取las文件
std::vector<std::vector<float>> MainWindow::readLasFile(const std::string& filePath)
{
    std::vector<std::vector<float>> data;
    std::string line;

    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error opening  file!" << std::endl;
        return data;
    }

    std::ifstream fileLine(filePath);
    //总行数
    const int lienCount = std::count(std::istreambuf_iterator<char>(fileLine),// 文件开始的字符迭代器
                                     std::istreambuf_iterator<char>(),// 文件结束的字符迭代器
                                     '\n');// 要计数的字符：换行符
    const int modVal = lienCount / 10;//取模值
    int sendVal = 1;//发送的进度条值
    int addLine = 1;//记录行数

    std::getline(file, line);//跳过首行
    while (std::getline(file, line)) {

        if( addLine % modVal == 0)
        {
            //qDebug() << " addLine:" << addLine << " modVal:" << modVal
            //         << " sendVal:" << sendVal;
            emit progressUpdate(sendVal);
            ++sendVal;
        }

        std::istringstream iss(line);
        std::vector<float> row;
        float value;

        // read depth (first column)
        iss >> value;
        //row.push_back(value); //深度值，暂时不需要

        // read data body (remaining columns)
        while (iss >> value) {
            row.push_back(value);
        }

        data.push_back(row);
        ++addLine;
    }

    file.close();
    return data;
}


//读取数据
void MainWindow::on_pushButton_readData_clicked()
{

    QString fileFilter = tr("LAS Files (*.las);;All Files (*.*)");
    QString fileNameLas = QFileDialog::getOpenFileName(this,
        tr("打开LAS文件"), "", fileFilter);
    if (fileNameLas.isEmpty()) {
        const std::string str_warn = "警告";
        const std::string str_mesg = "未选择文件";
        qDebug() << "<ERROR> fileNameLas.isEmpty()";
        //QMessageBox::warning(this, QString::fromStdString(str_warn)
        //                     ,QString::fromStdString(str_mesg));
        //QMessageBox::warning(this, fromStdString2QString(str_warn)
        //                     ,fromStdString2QString(str_mesg));
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
            slidingAverageFilter(m_data, outPutData, m_config.slidingWindowSize);//3
            break;
        }
        case MedianFilter: {//中值滤波
            medianFilter(m_data, outPutData, m_config.medianWindowSize);//5
            break;
        }
        case GaussianFilter: {//高斯滤波
            gaussianFilter(m_data, outPutData, m_config.gaussianKernelSize);// 5
            break;
        }
        case WaveletDenoise: {//小波
            wavelet3(m_data,outPutData,m_config.waveletLevel);
            break;
        }
        case ZeroPhaseFilter2D: {//零相位滤波
            std::vector<float> b = { 0.25, 0.5, 0.25 };
            zeroPhaseFilter2D(m_data, outPutData, b);
            break;
        }
        case SavitzkyGolayFilter: {//Savitzky-Golay 滤波
            applySavitzkyGolay(m_data, outPutData,
                               m_config.savitzkyGolayWindowSize,
                               m_config.savitzkyGolayPolynomialOrder);//5 2
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

    g_recentVal.clear();
    g_BackVal.clear();
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
    plot_den(outPutData,hLayout,vmax,vmin,m_customPlot_den,g_rectMode);
    m_outPutData = outPutData;
    //记录当前尺寸
    // 获取ColorMap的实际尺寸范围
    m_xrange = m_customPlot_den->xAxis->range().size();
    m_yrange = m_customPlot_den->yAxis->range().size();
    qDebug() << " m_xrange:" << m_xrange << " m_yrange:" << m_yrange;
    //获取当前坐标
//    m_xrangeLow = m_customPlot_den->xAxis->range().lower;
//    m_yrangeLow = m_customPlot_den->yAxis->range().lower;

    QObject::connect(m_customPlot_den, &QCustomPlot::mousePress, this, &MainWindow::onMousePress);
    QObject::connect(m_customPlot_den, &QCustomPlot::mouseMove, this, &MainWindow::onMouseMove);
    QObject::connect(m_customPlot_den, &QCustomPlot::mouseRelease, this, &MainWindow::onMouseRelease);

    // 连接范围变化信号到槽函数
    //QObject::connect(m_customPlot_den->xAxis, SIGNAL(rangeChanged(double, double)), this, SLOT(onXAxisRangeChanged(double, double)));
    //QObject::connect(m_customPlot_den->yAxis, SIGNAL(rangeChanged(double, double)), this, SLOT(onYAxisRangeChanged(double, double)));

    // 连接范围变化信号到槽函数
    //QObject::connect(m_customPlot_den->xAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(onXAxisRangeChanged(const QCPRange&)));
    //QObject::connect(m_customPlot_den->yAxis, SIGNAL(rangeChanged(const QCPRange&)), this, SLOT(onYAxisRangeChanged(const QCPRange&)));

    //QObject::connect(m_customPlot_den, &QCustomPlot::xAxisChanged, this, &MainWindow::updateTranslationOffset);
    //QObject::connect(m_customPlot_den, &QCustomPlot::yAxisChanged, this, &MainWindow::updateTranslationOffset);


}


//上一步
void MainWindow::on_pushButton_back_clicked()
{
    if( m_customPlot_den == nullptr)
        return;
    if (g_BackVal.size() < 1) {
        qDebug() << "Not enough elements to get the previous value";
        return;
        //throw std::out_of_range("Not enough elements to get the previous value");
    }
    qDebug() << "eee0 g_BackVal.size():" << g_BackVal.size();

    // 返回新的最后一个元素
    std::vector<RecentGraphChangedValue> rval = g_BackVal.back();
    //此处需要遵循顺序依次还原
    //for( int j = 0 ; j < rval.size(); ++j)
    for( int j = rval.size()-1; j >= 0; --j)
    {
        //此处遍历每个点，顺序无所谓
        for( int i = 0 ; i < rval.at(j).values.size(); ++i)
        //for( int i = rval.at(j).values.size()-1 ; i >= 0; --i)
        {
            const int index_i = rval.at(j).indices.at(i).first;
            const int index_j = rval.at(j).indices.at(i).second;
            m_outPutData[index_i][index_j] = rval.at(j).values.at(i);
        }
    }

    const float vmax = ui->lineEdit_vmax->text().toFloat();
    const float vmin = ui->lineEdit_vmin->text().toFloat();

    //问题来了，放入plot_den？--tmp
    //先生成一个新的m_customPlot_den，再填入hLayout
//    if( nullptr != m_customPlot_den)
//    {
//        delete m_customPlot_den;
//        m_customPlot_den = nullptr;
//    }
//    m_customPlot_den = new QCustomPlot();
//    //布局设置
//    // 检查g_custLayout_priWell中是否存在小部件
//    QLayoutItem* existingItem = hLayout->takeAt(0);
//    if (existingItem) {
//        QWidget* existingWidget = existingItem->widget();
//        if (existingWidget) {
//            // 删除已存在的小部件
//            delete existingWidget;
//        }
//        delete existingItem;
//    }
//    hLayout->addWidget(m_customPlot_den);


    plot_den(m_outPutData,hLayout,vmax,vmin,m_customPlot_den,g_rectMode);

    // 移除最后一个元素
    g_BackVal.pop_back();

}

//配置参数
void MainWindow::on_pushButton_settings_clicked()
{
    FilterConfigDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m_config = dialog.getConfig();
        //config.print();  // 打印 config 内容以便调试
        //qDebug() << config.gaussianKernelSize << " " << config.medianWindowSize;
        qDebug() << " print over";
        // 处理 config
    }


    //这种实现存在一些问题
    // 显示配置对话框
//    if (configDialog->exec() == QDialog::Accepted) {
//        // 获取配置
//        //filterConfig = configDialog->getConfig();
//        // 在这里可以添加应用配置的代码，比如更新滤波器参数
//        // applyFilter(filterConfig);
//    }
}

//导出数据
void MainWindow::on_pushButton_saveData_clicked()
{
    // 打开文件保存对话框
    QString fileNameLas = QFileDialog::getSaveFileName(this,
                                                       "save file", "",
                                                       tr("txt file (*.txt);;all file (*)"));

    // 检查用户是否取消了文件选择
    if (fileNameLas.isEmpty()) {
        QMessageBox::warning(this, tr("warn"), tr("no choose file"));
        return;
    }

    // 将 QString 转换为 std::string
    const std::string filePath = fromQString2stdString(fileNameLas);

    // 保存文件
    saveFile(filePath, m_outPutData);
}

//导出图像
void MainWindow::on_pushButton_savePic_clicked()
{
    if( nullptr == m_customPlot_den)
    {
        const std::string str_warn = "警告";
        const std::string str_mesg = "图像还绘制";
        QMessageBox::warning(this, QString::fromStdString(str_warn)
                             ,QString::fromStdString(str_mesg));
    }


    CustomDepthDialog dialog(this);
    //dialog.setMinDepth(uiData->getDepthBeginSet3());
    //dialog.setMaxDepth(uiData->getDepthEndSet3());
    dialog.setSaveFormat("png");
    dialog.setSavePath(".");
    dialog.setFileName("name1");

    if (dialog.exec() == QDialog::Accepted  ) {
        // 获取对话框中的参数
        //const double minDepth = dialog.getMinDepth();
        //const double maxDepth = dialog.getMaxDepth();
        const double xcale = dialog.getXcale();
        const double ycale = dialog.getYcale();
        const QString saveFormat = dialog.getSaveFormat();
        const QString savePath = dialog.getSavePath();
        const QString saveFileName = dialog.getFileName();

        //qDebug() << "minDepth:" << minDepth << " maxDepth:" << maxDepth
        //         << " saveFormat:" << saveFormat << " savePath:" << savePath
        //         << " saveFileName:" << saveFileName;

        // 设置 QCustomPlot 的 y 轴范围
        //m_customPlot_den->yAxis->setRange(minDepth, maxDepth);

        // 设置目标图像大小
        QSize desiredSize(xcale, ycale);

        // 渲染 QCustomPlot 到 QPixmap
        QPixmap originalPixmap = m_customPlot_den->toPixmap(desiredSize.width(), desiredSize.height());

        // 构建保存路径
        const QString fullPath = savePath + "/" + saveFileName + "." + saveFormat.toLower();

        // 保存图像
        if (originalPixmap.save(fullPath))
        {
            qDebug() << "Image saved successfully at" << fullPath;
        }
        else
        {
            qDebug() << "Failed to save image at" << fullPath;
        }
    }
}

//矩形框交互
void MainWindow::on_checkBox_rect_stateChanged(int arg1)
{
    if( nullptr == m_customPlot_den ) return;
    if( Qt::Checked == arg1)
    {
        g_rectMode = true;
        m_customPlot_den->setInteractions(QCP::iRangeZoom);
    }
    else
    {
        g_rectMode = false;
        m_customPlot_den->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    }
}

//恢复初始大小
void MainWindow::on_pushButton_oriScale_clicked()
{
    if( nullptr == m_customPlot_den) return;
    m_customPlot_den->rescaleAxes();
    m_customPlot_den->replot();
}

