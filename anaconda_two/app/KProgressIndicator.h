/*
 * $Id$
 *
 *  Created on: Nov 28, 2008
 *      Author: koheik
 */
#ifndef KProgressIndicator_H_
#define KProgressIndicator_H_
#include <QtCore/QDebug>
#include <QtCore/QMutex>

#include <QtGui/QWidget>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QProgressBar>
#include <QtGui/QGridLayout>
#include <QtGui/QDialogButtonBox>

#include "AnRoot.h"

class KProgressIndicator : public QDialog
{
	Q_OBJECT
public:

	KProgressIndicator(AnRoot *root, QWidget *parent = 0);
	~KProgressIndicator();

public slots:
	void start();
	void setProgress(int i, int j);
	void cancel();

private:
	AnRoot         *m_root;
	int             m_size;
	int            *m_val;
	QProgressBar   *m_bar;
	QPushButton    *m_cancel;
	QPushButton    *m_ok;

	QMutex          m_mutex;

};
#endif
