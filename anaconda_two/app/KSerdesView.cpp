/*
 * KSerdesView.cpp
 *
 *  Created on: Nov 22, 2008
 *      Author: koheik
 */

#include "KSerdesView.h"

#include <QtCore/QDebug>
#include <QtGui/QGridLayout>



KSerdesView::KSerdesView(QWidget *parent) : QGroupBox("SERDES", parent) {

  QGridLayout *grid = new QGridLayout(this);

  int row = 0;
  grid->addWidget(new QLabel("Logical Address:"), row, 0);
  grid->addWidget(m_laddr = new QLabel("-.-.-.-"), row, 1);

  grid->addWidget(new QLabel("Hardware Address:"), ++row, 0);
  grid->addWidget(m_haddr = new QLabel("---.--.-.-"), row, 1);

  grid->addWidget(new QLabel("Firmware:"), ++row, 0);
  grid->addWidget(m_firm = new QLabel("--"), row, 1);

  // grid->addWidget(new QLabel("Temperature:"), ++row, 0);
  // grid->addWidget(m_temp = new QLabel("--.--"), row, 1);

  grid->addWidget(new QLabel("Register:"), ++row, 0);
  grid->addWidget(m_ecsr = new QLabel("--"), row, 1);
}

KSerdesView::~KSerdesView() {
  // TODO Auto-generated destructor stub
}

void KSerdesView::currentRowChanged(const QModelIndex &current, const QModelIndex &parent)
{
	AnBoard *brd = static_cast<AnBoard*>(current.internalPointer());
//  tdig->sync(1);
	if(AnSerdes *srd = dynamic_cast<AnSerdes*>(brd)) {
		setTitle(srd->name());
		m_laddr->setText(srd->lAddress().toString());
		m_haddr->setText(srd->hAddress().toString());
		m_firm->setText(srd->firmwareString());
		m_ecsr->setText("0x" + QString::number(srd->ecsr(), 16));
		m_ecsr->setToolTip(srd->ecsrString());
	}
}
