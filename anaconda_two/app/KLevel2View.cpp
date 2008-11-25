/*
 * KLevel2View.cpp
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#include "KLevel2View.h"

KLevel2View::KLevel2View(const QString &title, QWidget *parent) : QDockWidget(title, parent) {

	m_tcpuView = new KTcpuView(this);
	m_thubView = new KThubView(this);
	
	QWidget *widget = new QWidget(this);
	QHBoxLayout *hbox = new QHBoxLayout(widget);
	hbox->addWidget(m_tcpuView);
	hbox->addWidget(m_thubView);

	m_tcpuView->setVisible(false);

	setWidget(widget);

}

KLevel2View::~KLevel2View() {
  // TODO Auto-generated destructor stub
}

void KLevel2View::currentRowChanged(const QModelIndex &current, const QModelIndex &parent)
{
	AnCanObject *cobj = static_cast<AnCanObject*>(current.internalPointer());

	if (AnTcpu *tcpu  = dynamic_cast<AnTcpu*>(cobj)) {
		tcpu->sync(1);
		m_tcpuView->currentRowChanged(current, parent);
		m_thubView->setVisible(false);
		m_tcpuView->setVisible(true);

	} else if (AnThub *thub  = dynamic_cast<AnThub*>(cobj)) {
		thub->sync(1);
		m_thubView->currentRowChanged(current, parent);
		m_tcpuView->setVisible(false);
		m_thubView->setVisible(true);
	}
	
}
