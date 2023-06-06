#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QScrollArea>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <tesseract/baseapi.h>
#include <iostream>

void performOCR(const cv::Mat& image, tesseract::TessBaseAPI& tess, std::string& result) {
    tess.SetImage(image.data, image.cols, image.rows, 3, image.step);

    if (tess.Recognize(0) != 0) {
        std::cerr << "Error: Failed to recognize text in the image." << std::endl;
        return;
    }

    result = tess.GetUTF8Text();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;

    QWidget* rightContainer = findChild<QWidget*>("rightContainer");
    if (rightContainer != nullptr)
    {
        QScrollArea* scrollArea = new QScrollArea(this);
        scrollArea->setWidgetResizable(true);

        QLabel* textContainer = new QLabel(scrollArea);
        textContainer->setObjectName("textContainer");
        textContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        textContainer->setAlignment(Qt::AlignCenter);
        textContainer->setStyleSheet("QLabel#textContainer { background-color: #FFFFFF; border: 2px solid #333333; color: black; }");
        textContainer->setWordWrap(true);

        scrollArea->setWidget(textContainer);

        QVBoxLayout* layout = new QVBoxLayout(rightContainer);
        layout->addWidget(scrollArea);

        rightContainer->setLayout(layout);
    }
    else
    {
        qDebug() << "Error: Unable to find the rightContainer widget.";
    }

    QWidget* middleContainer = findChild<QWidget*>("middleContainer");
    if (middleContainer != nullptr)
    {
        middleContainer->setStyleSheet("background-color: #FFFFFF;");

        QLabel* imageLabel = new QLabel(middleContainer);
        imageLabel->setObjectName("imageLabel");
        imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        imageLabel->setScaledContents(true); // Scale the image to fit the label
        QVBoxLayout* layout = new QVBoxLayout(middleContainer);
        layout->addWidget(imageLabel);

        middleContainer->setLayout(layout);
    }
    else
    {
        qDebug() << "Error: Unable to find the middleContainer widget.";
    }

    connect(ui->uploadImageButton, &QPushButton::clicked, this, &MainWindow::onUploadImageButtonClicked);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onUploadImageButtonClicked()
{
    QFileDialog dialog(this, tr("Open Image"), QString(), tr("Image Files (*.png *.jpg *.bmp)"));
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setOption(QFileDialog::DontUseNativeDialog, true);
    dialog.exec();

    QStringList fileNames = dialog.selectedFiles();
    if (!fileNames.isEmpty())
    {
        QString fileName = fileNames.first();
        QString imagePath = fileName;

        cv::Mat image = cv::imread(imagePath.toStdString());

        tesseract::TessBaseAPI tess;
        if (tess.Init(nullptr, "eng", tesseract::OEM_DEFAULT) != 0) {
            std::cerr << "Failed to initialize Tesseract OCR!" << std::endl;
            return;
        }

        std::string ocrResult;
        performOCR(image, tess, ocrResult);

        QLabel* textContainer = findChild<QLabel*>("textContainer");
        if (textContainer != nullptr)
        {
            textContainer->setText(QString::fromStdString(ocrResult));
        }

        QLabel* imageLabel = findChild<QLabel*>("imageLabel");
        if (imageLabel != nullptr)
        {
            QImage qImage(image.data, image.cols, image.rows, static_cast<int>(image.step), QImage::Format_RGB888);
            imageLabel->setPixmap(QPixmap::fromImage(qImage));
        }
    }
}
