/*
 * $Id$
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#include <QtGui/QApplication>
#include "KTdigView.h"

KTdigView::KTdigView(QWidget *parent) : QGroupBox("TDIG", parent) {

  QGridLayout *grid = new QGridLayout(this);
  grid->setSpacing(2);
  grid->setContentsMargins(10, 2, 10, 2);

  int row = -1;
  // grid->addWidget(l_laddr = new QLabel("Logical Address:"), ++row, 0);
  // grid->addWidget(m_laddr = new QLabel("-.---.-.-"),          row, 1);

  grid->addWidget(l_haddr = new QLabel("Hardware Address:"), ++row, 0);
  grid->addWidget(m_haddr = new QLabel("---.--.--.-"),         row, 1);

  grid->addWidget(l_firm = new QLabel("Firmware:"), ++row, 0);
  grid->addWidget(m_firm = new QLabel("--/--"),       row, 1);

  grid->addWidget(l_chip = new QLabel("Chip ID:"),  ++row, 0);
  grid->addWidget(m_chip = new QLabel("--/--"),       row, 1);

  grid->addWidget(l_temp = new QLabel("Temperature:"), ++row, 0);
  grid->addWidget(m_temp = new QLabel("--.--"),        row, 1);

  grid->addWidget(l_ecsr = new QLabel("ECSR:"), ++row, 0);
  grid->addWidget(m_ecsr = new QLabel("--"),    row, 1);

  grid->addWidget(l_pld03 = new QLabel("PLD Reg[03]:"), ++row, 0);
  grid->addWidget(m_pld03 = new QLabel("--"),    row, 1);

  grid->addWidget(l_thrs = new QLabel("Threhosld:"), ++row, 0);
  grid->addWidget(m_thrs = new QLabel("--"),    row, 1);

  grid->addWidget(l_eeprom = new QLabel("EEPROM:"), ++row, 0);
  grid->addWidget(m_eeprom = new QLabel("----"),  row, 1);

	for (int i = 0; i < 3; ++i) {
		grid->addWidget(l_status[i] = new QLabel(QString("TDC %1 Status:").arg(i+1)), ++row, 0);
		grid->addWidget(m_status[i] = new QLabel("--"),                                 row, 1);
	}

	QFont ft(QApplication::font());
//	l_laddr->setFont(ft); m_laddr->setFont(ft);
	l_haddr->setFont(ft); m_haddr->setFont(ft);
	l_firm->setFont(ft);  m_firm->setFont(ft);
	l_chip->setFont(ft);  m_chip->setFont(ft);
	l_temp->setFont(ft);  m_temp->setFont(ft);
	l_ecsr->setFont(ft);  m_ecsr->setFont(ft);
	l_pld03->setFont(ft); m_pld03->setFont(ft);	
	l_thrs->setFont(ft);  m_thrs->setFont(ft);
	l_eeprom->setFont(ft); m_eeprom->setFont(ft);
	for (int i = 0; i < 3; ++i) {
		l_status[i]->setFont(ft);
		m_status[i]->setFont(ft);
	}
	setVisible(false);
}

KTdigView::~KTdigView() {
  // TODO Auto-generated destructor stub
}

void KTdigView::setSelectionModel(QItemSelectionModel *md)
{
	m_selectionModel = md;

	QObject::connect(
		m_selectionModel,
		SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
		this,
		SLOT(currentRowChanged(const QModelIndex &, const QModelIndex &)));

	QObject::connect(
		m_selectionModel,
		SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
		this,
		SLOT(selectionChanged(const QItemSelection&, const QItemSelection&)));
}

void KTdigView::currentRowChanged(const QModelIndex &current, const QModelIndex &parent)
{
	AnBoard *brd = static_cast<AnBoard*>(current.internalPointer());
	if (AnTdig *tdig = dynamic_cast<AnTdig*>(brd)) {
		tdig->sync(3);
		setTitle(tdig->name());
		// m_laddr->setText(tdig->lAddress().toString());
		m_haddr->setText(tdig->hAddress().toString());
		m_firm->setText(tdig->firmwareString());
		m_chip->setText(tdig->chipIdString());
		m_temp->setText(tdig->tempString());
		m_thrs->setText(tdig->thresholdString());
		m_ecsr->setText(tdig->ecsrString(true));
		m_ecsr->setToolTip(tdig->ecsrToolTipString());
		m_pld03->setText(tdig->pldReg03String(true));
		m_pld03->setToolTip(tdig->pldReg03ToolTipString());
		m_eeprom->setText(tdig->eepromString());

		for(int i = 0; i < 3; i++) {
			m_status[i]->setText("0x" + QString::number(tdig->tdc(i+1)->status(), 16));
			m_status[i]->setToolTip(tdig->tdc(i+1)->statusTipString());
		}
		setVisible(true);
	}
}

void KTdigView::selectionChanged(
	const QItemSelection& selected,
	const QItemSelection& deselected)
{
	if (m_selectionModel->selection().indexes().count() == 0) {
		setVisible(false);
	}
}
