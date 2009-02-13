/*
 * $Id$
 *
 *  Created on: Jan. 10, 2009
 *      Author: koheik
 */

#ifndef KSimpleWindow_H_
#define KSimpleWindow_H_

#include <QtCore/QString>
#include <QtCore/QTime>
#include <QtCore/QMutex>

#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>

class KMainWindow;
class AnRoot;

class KSimpleWindow : public QDialog
{
	Q_OBJECT

public:
	KSimpleWindow(KMainWindow *parent = 0);

	void selectMode(int idx) { m_combo->setCurrentIndex(idx); }

public slots:
	void agentFinished();

private:
	AnRoot      *m_root;
	KMainWindow *m_main;
	QPushButton *m_check;
	QComboBox   *m_combo;
	QLabel      *m_status;
	QLabel      *l_status;
};
#endif /* KSimpleWindow_H_ */
