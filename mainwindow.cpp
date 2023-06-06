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

    // Find the rightContainer widget
    QWidget* rightContainer = findChild<QWidget*>("rightContainer");
    if (rightContainer != nullptr)
    {
        // Create a QScrollArea to enable scrolling
        QScrollArea* scrollArea = new QScrollArea(this);
        scrollArea->setWidgetResizable(true);

        // Create a special box widget with defined background (QLabel)
        QLabel* textContainer = new QLabel(scrollArea);
        textContainer->setObjectName("textContainer");
        textContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        textContainer->setAlignment(Qt::AlignCenter);
        textContainer->setStyleSheet("QLabel#textContainer { background-color: #f0f0f0; border: 2px solid #333333; color: black; }");
        textContainer->setWordWrap(true); // Enable word wrapping

        // Set the QLabel as the widget for the QScrollArea
        scrollArea->setWidget(textContainer);

        // Create a layout for the rightContainer
        QVBoxLayout* layout = new QVBoxLayout(rightContainer);
        layout->addWidget(scrollArea);

        // Set the layout on the rightContainer widget
        rightContainer->setLayout(layout);
    }
    else
    {
        qDebug() << "Error: Unable to find the rightContainer widget.";
    }

    // Find the middleContainer widget
    QWidget* middleContainer = findChild<QWidget*>("middleContainer");
    if (middleContainer != nullptr)
    {
        // Set the background color of the middleContainer
        middleContainer->setStyleSheet("background-color: #808080;");

        // Create a QLabel to display the uploaded image
        QLabel* imageLabel = new QLabel(middleContainer);
        imageLabel->setObjectName("imageLabel");
        imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        imageLabel->setScaledContents(true); // Scale the image to fit the label

        // Set the QLabel as the widget for the middleContainer
        QVBoxLayout* layout = new QVBoxLayout(middleContainer);
        layout->addWidget(imageLabel);

        // Set the layout on the middleContainer widget
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

        // Find the textContainer widget
        QLabel* textContainer = findChild<QLabel*>("textContainer");
        if (textContainer != nullptr)
        {
            // Display the OCR result inside the textContainer
            textContainer->setText(QString::fromStdString(ocrResult));
        }

        // Find the imageLabel widget
        QLabel* imageLabel = findChild<QLabel*>("imageLabel");
        if (imageLabel != nullptr)
        {
            // Display the uploaded image
            QImage qImage(image.data, image.cols, image.rows, static_cast<int>(image.step), QImage::Format_RGB888);
            imageLabel->setPixmap(QPixmap::fromImage(qImage));
        }
    }
}
