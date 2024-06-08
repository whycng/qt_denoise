#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "FilterConfigDialog.h"
#include "qboxlayout.h"
#include "qcombobox.h"
#include "qcustomplot.h"
#include <QMainWindow>
#include <deque>

struct RecentGraphChangedValue{
    std::vector<std::pair<int, int>> indices;// i,j
    std::vector<float> values;//对应的值
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void onMousePress(QMouseEvent *event);
    void onMouseMove(QMouseEvent *event) ;
    void onMouseRelease(QMouseEvent *event);

    QHBoxLayout* hLayout = nullptr;
    QCustomPlot* m_customPlot_den = nullptr;
    QCPColorMap* m_colorMap = nullptr;
    //double g_threshold ;
    //double g_radius ;
    void plot_den(const std::vector<std::vector<float>>& outData,QHBoxLayout* hLayout
                  ,const float& vmax, const float& vmin
                  ,QCustomPlot* customPlot_den );


private:
    void updateRedCircle() ;
    void denoiseDataInCircle(QPoint mousePos);
    void denoiseDataSlideCircle(QPoint mousePos);
private slots:

    /*void onXAxisRangeChanged(double lower, double upper)
    {
        m_xoffset = m_xrangeLow - lower;
        // 输出当前 x 轴范围
        qDebug() << "X axis range changed: [" << lower << ", " << upper << "]"
                 << " m_xoffset:" << m_xoffset;

        // 更新当前范围值
        m_xrangeLow = lower;
    }

    void onYAxisRangeChanged(double lower, double upper)
    {
        m_yoffset = m_yrangeLow - lower;
        // 输出当前 y 轴范围
        qDebug() << "Y axis range changed: [" << lower << ", " << upper << "]"
                 << " m_yoffset:" << m_yoffset;

        // 更新当前范围值
        m_yrangeLow = lower;
    }*/

    void onXAxisRangeChanged(const QCPRange &newRange)
    {
        double xLower = newRange.lower;
        m_xoffset = m_xrangeLow - xLower;
        // 输出当前 x 轴范围
        //qDebug() << "X axis range changed: [" << xLower << ", " << xUpper << "]"
        //         << " m_xoffset:" << m_xoffset;
    }

    void onYAxisRangeChanged(const QCPRange &newRange)
    {
        double yLower = newRange.lower;
        m_yoffset = m_yrangeLow - yLower;
        // 输出当前 y 轴范围
        qDebug() << "Y axis range changed: [" << yLower << ", "   << "]"
                 << " m_yoffset:" << m_yoffset;
    }

    void on_pushButton_readData_clicked();

    void on_pushButton_denoisePrc_clicked();

    void on_pushButton_back_clicked();
    void on_pushButton_settings_clicked();

    void on_pushButton_saveData_clicked();

    void on_pushButton_savePic_clicked();

signals:
    //void xAxisChanged(double dx, double dy);
    //void yAxisChanged(double dx, double dy);
private:
    Ui::MainWindow *ui;
//    FilterConfigDialog *configDialog;
//    FilterConfig filterConfig;  // 存储滤波器配置

    FilterConfig m_config;
    bool m_selecting = false;
    QPoint m_startPoint;
    QPoint m_endPoint;
    std::vector<std::vector<float>> m_data;
    std::vector<std::vector<float>> m_outPutData;

    QCPItemEllipse *m_circle;
    QPoint m_mousePos;
    QSet<QPointF> m_selectedPoints;
    int m_radius;
    std::vector<RecentGraphChangedValue> g_recentVal;
    std::vector<float> g_slidValues;
    std::vector<std::pair<int, int>> g_slidIndices;
    std::vector<std::vector<RecentGraphChangedValue>> g_BackVal;
    //int m_nowStep;
    double m_xrange;
    double m_yrange;
    double m_xrangeLow;
    double m_yrangeLow;
    double m_xoffset;
    double m_yoffset;

};
#endif // MAINWINDOW_H
