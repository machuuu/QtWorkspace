#include "otsuthreshold.h"
#include "QApplication"
#include "QMainWindow"
#include "QMenuBar"
#include "QFileDialog"
#include "QImageReader"
#include "QMessageBox"

QImage* inputImage = nullptr;
OtsuThreshold* ot = nullptr;

static void onImageLoad(QMainWindow* win)
{
	QString filePath;

	QFileDialog dialog(nullptr);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(QMainWindow::tr("Images (*.png *.xpm *.jpg *.bmp)"));
	if (dialog.exec()) {
		filePath = dialog.selectedFiles().at(0);
	}

	QImageReader reader(filePath);
	const QImage newImage = reader.read();
	if (newImage.isNull())
	{
		QMessageBox::information(nullptr, QGuiApplication::applicationDisplayName(),
			QMainWindow::tr("Cannot load %1: %2")
			.arg(QDir::toNativeSeparators(filePath), reader.errorString()));

		return;
	}

	inputImage = new QImage(newImage);
	ot = new OtsuThreshold(win, inputImage);

	win->setCentralWidget(ot);
}

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	QMainWindow win;
	
	QAction* openImageAction = win.menuBar()->addMenu("File")->addAction("Open");
	QObject::connect(openImageAction, &QAction::triggered, [&win]() {
		onImageLoad(&win);
	});

	win.show();
	return a.exec();
}
