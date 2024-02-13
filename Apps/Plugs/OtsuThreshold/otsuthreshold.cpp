#include "otsuthreshold.h"
#include "ui_otsuthreshold.h"
#include <QImage>

OtsuThreshold::OtsuThreshold(QWidget* parent /*= nullptr*/, const QImage* image /*= nullptr*/) :
    QWidget(parent),
    ui(new Ui::OtsuThreshold)
{
    ui->setupUi(this);
	setImage(image);
}

OtsuThreshold::~OtsuThreshold()
{
    delete ui;
}

bool OtsuThreshold::setImage(const QImage* image)
{
	// do some tests for construction
	if (image != nullptr) {
		inputImage = image;
	}

	return true;
}