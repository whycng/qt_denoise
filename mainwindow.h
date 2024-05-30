#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qboxlayout.h"
#include "qcombobox.h"
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

    QHBoxLayout* hLayout = nullptr;

private:

private slots:
    void on_pushButton_readData_clicked();

    void on_pushButton_denoisePrc_clicked();

private:
    Ui::MainWindow *ui;

    std::vector<std::vector<float>> m_data;
};
#endif // MAINWINDOW_H
