#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    //double g_threshold ;
    //double g_radius ;

private:
    void updateRedCircle() ;
    void denoiseDataInCircle(QPoint mousePos);
private slots:
    void on_pushButton_readData_clicked();

    void on_pushButton_denoisePrc_clicked();

    void on_pushButton_back_clicked();

private:
    Ui::MainWindow *ui;

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
    std::vector<std::vector<RecentGraphChangedValue>> g_BackVal;
    //int m_nowStep;

};
#endif // MAINWINDOW_H
