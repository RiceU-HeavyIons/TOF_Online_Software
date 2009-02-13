/*
 * $Id$
 *
 *  Created on: Jan. 10, 2009
 *      Author: koheik
 */

#include <QtGui/QApplication>
#include <QtGui/QGridLayout>
#include "KMainWindow.h"
#include "KSimpleWindow.h"


//-----------------------------------------------------------------------------
KSimpleWindow::KSimpleWindow(KMainWindow *parent)
  : QDialog(parent)
{
	m_main = parent; // Main Window

	m_root = parent->root();
	QObject::connect(m_root, SIGNAL(finished()),  this, SLOT(agentFinished()));
	
	setWindowTitle(QApplication::applicationName());
	setModal(false);
//	resize(300, 200);

	QGridLayout *gl = new QGridLayout(this);

	l_status = new QLabel("Status:");
	m_status = new QLabel("unknown");
	gl->addWidget(l_status, 0, 0, 1, 1);
	gl->addWidget(m_status, 0, 1, 1, 1);

	QLabel *label = new QLabel(tr("Mode:"));
	//  label->setAlignment(Qt::AlignLeft);
	m_combo = new QComboBox(this);
	m_combo->addItems(m_root->modeList());
	QObject::connect(m_combo, SIGNAL(activated(int)), m_main, SLOT(setMode(int)));
	gl->addWidget(label,   1, 0, 1, 1);
	gl->addWidget(m_combo, 1, 1, 1, 1);

	m_check = new QPushButton("check");
	QObject::connect(m_check, SIGNAL(clicked()), m_main, SLOT(doSync()));
	gl->addWidget(m_check, 2, 0, 1, 2);

}

void KSimpleWindow::agentFinished()
{
	if (m_root->status() == 0)
		m_status->setText("OK");
	else
		m_status->setText("Error");
}

