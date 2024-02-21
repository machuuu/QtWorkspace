#include <QApplication>
#include "Logger.h"
#include "mainwindow.h"

int main(int argc, char** argv)
{
	QApplication a(argc, argv);
	QString logFilePath = QCoreApplication::applicationDirPath() + "\\Application.log";
	Logger::Initialize(logFilePath);
	MainWindow mw;
	//mw.resize(1920, 1080);
	mw.show();
	Logger::Clean();
	return a.exec();
}