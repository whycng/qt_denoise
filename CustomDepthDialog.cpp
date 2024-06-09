#include "CustomDepthDialog.h"
#include "qlabel.h"
CustomDepthDialog::CustomDepthDialog(QWidget *parent) :
    QDialog(parent)
{


    QVBoxLayout *layout = new QVBoxLayout(this);


//    QHBoxLayout* mdLayout = new QHBoxLayout();
//    QLabel* mdLabel = new QLabel("起始深度 (m):", this);
//    mdLayout->addWidget(mdLabel);

//    minDepthEdit = new QLineEdit(this);
//    minDepthEdit->setPlaceholderText("起始深度 (m)");
//    mdLayout->addWidget(minDepthEdit);

//    layout->addLayout(mdLayout);


//    QHBoxLayout* md2Layout = new QHBoxLayout();
//    QLabel* md2Label = new QLabel("终止深度 (m):", this);
//    md2Layout->addWidget(md2Label);

//    maxDepthEdit = new QLineEdit(this);
//    maxDepthEdit->setPlaceholderText("终止深度 (m)");
//    md2Layout->addWidget(maxDepthEdit);

//    layout->addLayout(md2Layout);



    QHBoxLayout* xLayout = new QHBoxLayout();
    QLabel* xLabel = new QLabel("保存图像宽度:", this);
    xLayout->addWidget(xLabel);

    xcale = new QLineEdit(this);
    xcale->setPlaceholderText("保存图像宽度");
    xLayout->addWidget(xcale);

    layout->addLayout(xLayout);


    QHBoxLayout* yLayout = new QHBoxLayout();
    QLabel* yLabel = new QLabel("保存图像高度:", this);
    yLayout->addWidget(yLabel);

    ycale = new QLineEdit(this);
    ycale->setPlaceholderText("保存图像高度");
    yLayout->addWidget(ycale);

    layout->addLayout(yLayout);



    formatComboBox = new QComboBox(this);
    formatComboBox->addItem("JPG");
    formatComboBox->addItem("PNG");
    formatComboBox->addItem("TIFF");
    formatComboBox->addItem("BMP");
    formatComboBox->addItem("Lossless");
    layout->addWidget(formatComboBox);

    QHBoxLayout *pathLayout = new QHBoxLayout();
    pathLineEdit = new QLineEdit(this);
    pathLineEdit->setReadOnly(true);
    pathLayout->addWidget(pathLineEdit);

    QPushButton *selectPathButton = new QPushButton("选择路径", this);
    connect(selectPathButton, &QPushButton::clicked, this, &CustomDepthDialog::onSelectPathClicked);
    pathLayout->addWidget(selectPathButton);

    layout->addLayout(pathLayout);

    QHBoxLayout *fileNameLayout = new QHBoxLayout();
    fileNameLineEdit = new QLineEdit(this);
    fileNameLineEdit->setPlaceholderText("图像名称");
    fileNameLayout->addWidget(fileNameLineEdit);

    layout->addLayout(fileNameLayout);

    QPushButton *saveButton = new QPushButton("保存", this);
    connect(saveButton, &QPushButton::clicked, this, &CustomDepthDialog::onSaveClicked);
    layout->addWidget(saveButton);

    setLayout(layout);
    setWindowTitle("LAS 保存图像");

    //给默认数值
    xcale->setText(QString::number(1000));
    ycale->setText(QString::number(500));
}


CustomDepthDialog::~CustomDepthDialog()
{
}

//void CustomDepthDialog::setMinDepth(double v)
//{
//    minDepthEdit->setText(QString::number(v));
//}

//void CustomDepthDialog::setMaxDepth(double v)
//{
//    maxDepthEdit->setText(QString::number(v));
//}

void CustomDepthDialog::setXscal(double v)
{
    xcale->setText(QString::number(v));
}

void CustomDepthDialog::setYscal(double v)
{
    ycale->setText(QString::number(v));
}

void CustomDepthDialog::setSaveFormat(QString v)
{
    formatComboBox->setCurrentText(v);
}

void CustomDepthDialog::setSavePath(QString v)
{
    pathLineEdit->setText(v);
}

void CustomDepthDialog::setFileName(QString v)
{
    fileNameLineEdit->setText(v);
}


double CustomDepthDialog::getXcale() const
{
    return  xcale->text().toDouble();
}

double CustomDepthDialog::getYcale() const
{
    return  ycale->text().toDouble();
}

//double CustomDepthDialog::getMinDepth() const
//{
//    return minDepthEdit->text().toDouble();
//}

//double CustomDepthDialog::getMaxDepth() const
//{
//    return maxDepthEdit->text().toDouble();
//}

QString CustomDepthDialog::getSaveFormat() const
{
    return formatComboBox->currentText();
}

QString CustomDepthDialog::getSavePath() const
{
    return pathLineEdit->text();
}

QString CustomDepthDialog::getFileName() const
{
    return fileNameLineEdit->text();
}

void CustomDepthDialog::onSaveClicked()
{
    // Validation
//    bool ok;
//    int minDepth = minDepthEdit->text().toDouble(&ok);
//    if (!ok) {
//        QMessageBox::warning(this, "Invalid Input", "Please enter a valid start depth.");
//        return;
//    }

//    int maxDepth = maxDepthEdit->text().toDouble(&ok);
//    if (!ok) {
//        QMessageBox::warning(this, "Invalid Input", "Please enter a valid end depth.");
//        return;
//    }

//    if (minDepth >= maxDepth) {
//        QMessageBox::warning(this, "Invalid Range", "End depth must be greater than start depth.");
//        return;
//    }

    accept(); // Close dialog with accept status
}

void CustomDepthDialog::onSelectPathClicked()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Select Directory"), QDir::homePath(), QFileDialog::ShowDirsOnly);
    if (!path.isEmpty())
        pathLineEdit->setText(path);
}
