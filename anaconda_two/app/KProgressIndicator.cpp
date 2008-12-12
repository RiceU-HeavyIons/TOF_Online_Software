/*
 * $Id$
 *
 *  Created on: Nov 28, 2008
 *      Author: koheik
 */
#include <QtCore/QMutexLocker>
#include "KProgressIndicator.h"

KProgressIndicator::KProgressIndicator(AnRoot *root, QWidget *parent)
 : QDialog(parent), m_root(root)
{

	m_size = m_root->nAgents();

    setWindowTitle("In Progress...");
	resize(400, 200);

	m_val = new int[m_size + 1];
	m_bar = new QProgressBar[m_size + 1];
    QGridLayout *gl = new QGridLayout(this);

    QDialogButtonBox *box = new QDialogButtonBox(Qt::Horizontal);
	//     box->addButton(QDialogButtonBox::Cancel);
	m_cancel = new QPushButton("Cancel");
	m_ok     = new QPushButton("OK");
	m_ok->setEnabled(false);
	box->addButton(m_cancel, QDialogButtonBox::RejectRole);
	box->addButton(m_ok,     QDialogButtonBox::AcceptRole);
	connect(box, SIGNAL(rejected()), this, SLOT(cancel()));
	connect(box, SIGNAL(accepted()), this, SLOT(close()));

	m_bar[0].setRange(0, m_size*100);
	gl->addWidget(new QLabel("Total"),  0, 0, 1, 1);
	gl->addWidget(&m_bar[0],            0 ,1, 1, 1);

	for(int i = 1; i <= m_size; ++i) {
    	m_bar[i].setRange(0, 100);
    	gl->addWidget(new QLabel(QString("CanBus %1").arg(i)), i, 0, 1, 1);
    	gl->addWidget(&m_bar[i],                               i, 1, 1, 1);
	}
    gl->addWidget(box,    m_size + 1 , 0, 1, 2);


	for (int i = 0; i < m_size; ++i) {
	 	connect(m_root->agentById(i+1), SIGNAL(progress(int, int)),
			this, SLOT(setProgress(int, int)));
		connect(m_root->agentById(i+1), SIGNAL(finished()),
			parent, SLOT(agentFinished()));
	}
}

KProgressIndicator::~KProgressIndicator()
{
	delete[] m_bar;
	delete[] m_val;
}
// public slots
void KProgressIndicator::start()
{
	m_ok->setEnabled(false);
	m_cancel->setEnabled(true);
	for(int x = 0; x <= m_size; ++x) {
		m_val[x] = 0;
		m_bar[x].setValue(0);
	}
	exec();
}

void KProgressIndicator::setProgress(int id, int j)
{
	QMutexLocker locker(&m_mutex);

	m_bar[id].setValue(m_val[id] = j);
	m_val[0] = 0;
	for(int x = 1; x <= m_size; ++x) m_val[0] += m_val[x];
	m_bar[0].setValue(m_val[0]);
	update();

	if (m_val[0] == 100*m_size) {
		m_ok->setEnabled(true);
		m_cancel->setEnabled(false);
//		setVisible(false);
	}
}

void KProgressIndicator::cancel()
{
	m_root->stop();
	m_ok->setEnabled(true);
	m_cancel->setEnabled(false);
}
