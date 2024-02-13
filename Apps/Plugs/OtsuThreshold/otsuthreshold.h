#ifndef OTSUTHRESHOLD_H
#define OTSUTHRESHOLD_H

#include <QWidget>
#include "OtsuThresholdExport.h"

namespace Ui {
class OtsuThreshold;
}

class OTSU_THRESHOLD_EXPORT OtsuThreshold : public QWidget
{
    Q_OBJECT

public:
    explicit OtsuThreshold(QWidget* parent = nullptr, const QImage* image = nullptr);
    ~OtsuThreshold();

private:
    Ui::OtsuThreshold *ui;
    const QImage* inputImage;

    bool setImage(const QImage* image);
};

#endif // OTSUTHRESHOLD_H
