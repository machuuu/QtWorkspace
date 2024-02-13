#include <QApplication>
#include "VulkanWidget.h"
#include "Logger.h"
//#include "vcMainWindow.h"

int main(int argc, char **argv)
{
	QApplication a(argc, argv);
	QString logFilePath = QCoreApplication::applicationDirPath() + "\\Application.log";
	Logger::Initialize(logFilePath);
	VulkanWidget w;
	w.InitializeVulkan();
	w.resize(1920, 1080);
	w.show();
	w.CleanupVulkan();
	Logger::Clean();
	return a.exec();
}