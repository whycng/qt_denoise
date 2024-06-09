#ifndef CUSTOMDEPTHDIALOG_H
#define CUSTOMDEPTHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>

class CustomDepthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomDepthDialog(QWidget *parent = nullptr);
    ~CustomDepthDialog();

//    void setMinDepth(double v) ;
//    void setMaxDepth(double v) ;
    void setXscal(double v) ;
    void setYscal(double v) ;

    void setSaveFormat(QString v) ;
    void setSavePath(QString v) ;
    void setFileName(QString v) ;

//    double getMinDepth() const;
//    double getMaxDepth() const;
    double getXcale() const;
    double getYcale() const;
    QString getSaveFormat() const;
    QString getSavePath() const;
    QString getFileName() const;

private:
    //QLineEdit *minDepthEdit;
    //QLineEdit *maxDepthEdit;
    QLineEdit* xcale;
    QLineEdit* ycale;
    QComboBox *formatComboBox;
    QLineEdit *pathLineEdit;
    QLineEdit *fileNameLineEdit;

private slots:
    void onSaveClicked();
    void onSelectPathClicked();
};

#endif // CUSTOMDEPTHDIALOG_H
