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
    ui->lineEdit_threshold->setText("0.1");
    ui->lineEdit_radius->setText("0.5");
    m_radius = 10;

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

//交互去噪
void MainWindow::denoiseDataInCircle(QPoint mousePos) {

    const float vmax = ui->lineEdit_vmax->text().toFloat();
    const float vmin = ui->lineEdit_vmin->text().toFloat();
    const float radius = ui->lineEdit_radius->text().toFloat();
    const float threshold = ui->lineEdit_threshold->text().toFloat();

    // 将鼠标像素坐标转换为轴坐标
    double xCoord = m_customPlot_den->xAxis->pixelToCoord(mousePos.x());
    double yCoord = m_customPlot_den->yAxis->pixelToCoord(mousePos.y());

    // 收集选中数据点的值及其索引
    std::vector<std::pair<int, int>> indices;
    std::vector<float> values;

    const int row = m_outPutData.size();
    const int col = m_outPutData.at(0).size();

    // 获取ColorMap的实际尺寸范围
    double xMin = m_customPlot_den->xAxis->range().lower;
    double xMax = m_customPlot_den->xAxis->range().upper;
    double yMin = m_customPlot_den->yAxis->range().lower;
    double yMax = m_customPlot_den->yAxis->range().upper;

    double xStep = (xMax - xMin) / (row - 1);
    double yStep = (yMax - yMin) / (col - 1);

    // 遍历m_outPutData，找到在指定范围内的数据点
    for(int i = 0; i < row; ++i) {
        for(int j = 0; j < col; ++j) {
            double dataX = xMin + i * xStep;
            double dataY = yMin + j * yStep;

            // 计算距离
            double distance = std::sqrt(std::pow(dataX - xCoord, 2) + std::pow(dataY - yCoord, 2));

            if(distance <= radius) {
                indices.push_back(std::make_pair(i, j));
                values.push_back(m_outPutData[i][j]);
            }
        }
    }


    // 添加新的元素到容器尾部
    g_recentVal.push_back({indices,values});


    // 按值降序排列
    std::vector<size_t> indicesSorted(values.size());
    std::iota(indicesSorted.begin(), indicesSorted.end(), 0);
    std::sort(indicesSorted.begin(), indicesSorted.end(), [&values](size_t a, size_t b) {
        return values[a] > values[b];
    });

    // 选择前10%以及后10%的数据点作为噪点
    size_t numNoisePoints = indicesSorted.size() * threshold;//0.1
    qDebug() << " 阈值个数 numNoisePoints:" << numNoisePoints << " 总数 indicesSorted.size():"
             << indicesSorted.size();
    int sum_test = 0;
    //for (size_t k = numNoisePoints; k < indicesSorted.size(); ++k) {
    for (size_t k = 0; k < indicesSorted.size(); ++k) {
        if( k >= numNoisePoints && k <= indicesSorted.size() - numNoisePoints)
            continue;
        ++sum_test;

        int i = indices[indicesSorted[k]].first;
        int j = indices[indicesSorted[k]].second;
        m_outPutData[i][j] = 0.5;
    }
    qDebug() << "去除噪点 sum_test:" << sum_test;

    // 刷新QCustomPlot以显示更新后的数据
    //plot_den(m_outPutData,hLayout,vmax,vmin,m_customPlot_den);

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
        //denoiseDataInCircle(event->pos());
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
        //updateRedCircle(); --bug
        denoiseDataInCircle(event->pos());
        //m_customPlot_den->replot(); // Trigger replot to update the plot
    }
}


//鼠标释放事件处理
void MainWindow::onMouseRelease(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        m_selecting = false;
        m_endPoint = event->pos();
        //performDenoising();
        //m_customPlot_den->replot();
        const float vmax = ui->lineEdit_vmax->text().toFloat();
        const float vmin = ui->lineEdit_vmin->text().toFloat();


        qDebug() << " g_recentVal size:" << g_recentVal.size();
        g_BackVal.push_back(g_recentVal);
        if (g_BackVal.size() == 20)
        {
            g_BackVal.pop_back();
        }
        g_recentVal.clear();
        qDebug() << "PUSH_BACK g_BackVal size:" << g_BackVal.size();




        plot_den(m_outPutData,hLayout,vmax,vmin,m_customPlot_den);
        //qDebug() << "<onMouseRelease> m_endPoint:" << m_endPoint;
        m_customPlot_den->setCurrentLayer("main");
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
    m_outPutData = outPutData;

    QObject::connect(m_customPlot_den, &QCustomPlot::mousePress, this, &MainWindow::onMousePress);
    QObject::connect(m_customPlot_den, &QCustomPlot::mouseMove, this, &MainWindow::onMouseMove);
    QObject::connect(m_customPlot_den, &QCustomPlot::mouseRelease, this, &MainWindow::onMouseRelease);
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

    plot_den(m_outPutData,hLayout,vmax,vmin,m_customPlot_den);

    // 移除最后一个元素
    g_BackVal.pop_back();

}

