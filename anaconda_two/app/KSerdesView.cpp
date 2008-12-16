/*
 * $Id$
 *
 *  Created on: Nov 22, 2008
 *      Author: koheik
 */

#include "KSerdesView.h"

#include <QtCore/QDebug>
#include <QtGui/QGridLayout>



KSerdesView::KSerdesView(QWidget *parent) : QGroupBox("SERDES", parent) {

	QGridLayout *grid = new QGridLayout(this);
	grid->setSpacing(2);
	grid->setContentsMargins(10, 2, 10, 2);

	int row = 0;
	grid->addWidget(new QLabel("Logical Address:"), row, 0);
	grid->addWidget(m_laddr = new QLabel("-.-.-.-"), row, 1);

	grid->addWidget(new QLabel("Hardware Address:"), ++row, 0);
	grid->addWidget(m_haddr = new QLabel("---.--.-.-"), row, 1);

	grid->addWidget(new QLabel("Firmware:"), ++row, 0);
	grid->addWidget(m_firm = new QLabel("--"), row, 1);

// grid->addWidget(new QLabel("Temperature:"), ++row, 0);
// grid->addWidget(m_temp = new QLabel("--.--"), row, 1);

	grid->addWidget(new QLabel("PLD Reg[9x]:"), ++row, 0);
	grid->addWidget(m_ecsr = new QLabel("--"), row, 1);

	setVisible(false);
}

KSerdesView::~KSerdesView() {
  // TODO Auto-generated destructor stub
}

void KSerdesView::setSelectionModel(QItemSelectionModel *md)
{
	m_selectionModel = md;

	QObject::connect(
		m_selectionModel,
		SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)),
		this,
		SLOT(currentRowChanged(const QModelIndex&, const QModelIndex&)));

	QObject::connect(
		m_selectionModel,
		SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this,
		SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
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
		m_ecsr->setText(srd->pld9xString());
		m_ecsr->setToolTip(srd->ecsrString());
		setVisible(true);
	}
}

void KSerdesView::selectionChanged(
	const QItemSelection& selected,
	const QItemSelection& deselected)
{
	if (m_selectionModel->selection().indexes().count() == 0) {
		setVisible(false);
	}
}