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

#include "version.h" // svn version number

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	// set application name from basename of binary image
	// i don't know whey i need to do this though...
	app.setApplicationName(QFileInfo(app.applicationFilePath()).baseName());
	app.setApplicationVersion(SVN_VERSION);
//	app.setFont(QFont("Lucida Grande", 9));

	qRegisterMetaType<AnRdMsg>("AnRdMsg");

#ifdef WITH_GUI
	KMainWindow kmw;
	//kmw.show();
	return app.exec();
#else
	AnRoot *m_root = new AnRoot(0);
	sleep(1);
	m_root->setMode(3);
	sleep(1);
	m_root->startAutoSync();
	int ret = app.exec();
	delete m_root;
	return ret;
#endif
}
