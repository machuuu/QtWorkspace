#include <QApplication>
#include "Logger.h"
#include "AirwayRenderWidget.h"

int main(int argc, char** argv)
{
	QApplication a(argc, argv);
	QString logFilePath = QCoreApplication::applicationDirPath() + "\\Application.log";
	Logger::Initialize(logFilePath);
	AirwayRenderWidget w;
	w.resize(1920, 1080);
	w.show();
	Logger::Clean();
	return a.exec();
}