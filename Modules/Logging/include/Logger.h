#pragma once 

// Adapted from https://github.com/VelazcoJD/QtLogging/blob/master

#include "qlogging.h"
#include "QDebug"
#include "QFile"
#include "QHash"

class Logger {
private:
	/// @brief The file object where logs are written to.
	static QFile* logFile;

	/// @brief Whether the logger has being initialized.
	static bool isInit;

	/// @brief The different type of contexts.
	static QHash<QtMsgType, QString> contextNames;

	/// @brief The function which handles the logging of text.
	static void MessageOutput(QtMsgType type, const QMessageLogContext& context,
		const QString& msg);

public:
	/// @brief Initializes the logger.
	static void Initialize(const QString fileName);

	static const bool IsInit();

	/// @brief Cleans up the logger.
	static void Clean();
};