#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qboxlayout.h"
#include "qcombobox.h"
#include "qcustomplot.h"
#include <QMainWindow>




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

private:
    void updateRedCircle() ;
    void denoiseDataInCircle();
private slots:
    void on_pushButton_readData_clicked();

    void on_pushButton_denoisePrc_clicked();

private:
    Ui::MainWindow *ui;

    bool m_selecting = false;
    QPoint m_startPoint;
    QPoint m_endPoint;
    std::vector<std::vector<float>> m_data;

    QCPItemEllipse *m_circle;
    QPoint m_mousePos;
    QSet<QPointF> m_selectedPoints;
    int m_radius;
};
#endif // MAINWINDOW_H
