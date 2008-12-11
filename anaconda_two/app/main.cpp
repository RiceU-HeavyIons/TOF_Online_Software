/*
 * $Id$
 *
 *  Created on: Nov 7, 2008
 *      Author: koheik
 */
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtGui/QFont>

#include "KMainWindow.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	// set application name from basename of binary image
	// i don't know whey i need to do this though...
	app.setApplicationName(QFileInfo(app.applicationFilePath()).baseName());
	app.setApplicationVersion("2.0.0");

	app.setFont(QFont("Lucida Grande", 10));

	qRegisterMetaType<AnRdMsg>("AnRdMsg");

	KMainWindow kmw;
	kmw.setWindowTitle(app.applicationName());
	kmw.show();

	return app.exec();
}
