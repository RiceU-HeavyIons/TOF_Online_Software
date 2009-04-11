/*
 * $Id$
 *
 *  Created on: Nov 23, 2008
 *      Author: koheik
 */
#include <QtGui/QHeaderView>
#include "KThubView.h"

KThubView::KThubView(QWidget *parent) : QGroupBox("THUB", parent)
{

	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->setContentsMargins(4, 4, 4, 4);

	QWidget *winfo = new QWidget();
	QGridLayout *grid = new QGridLayout(winfo);
	grid->setSpacing(2);
	grid->setContentsMargins(2, 0, 2, 0);

	int row = 0;
	grid->addWidget(new QLabel("Logical Address:"), row, 0);
	grid->addWidget(m_laddr = new QLabel(""), row, 1);

	grid->addWidget(new QLabel("Hardware Address:"), ++row, 0);
	grid->addWidget(m_haddr = new QLabel(""), row, 1);

	grid->addWidget(new QLabel("Firmware:"), ++row, 0);
	grid->addWidget(m_firm = new QLabel(""), row, 1);

	grid->addWidget(new QLabel("Temperature 1:"), ++row, 0);
	grid->addWidget(m_temp1 = new QLabel(""), row, 1);

	grid->addWidget(new QLabel("Temperature 2:"), ++row, 0);
	grid->addWidget(m_temp2 = new QLabel(""), row, 1);

	grid->addWidget(new QLabel("CRC:"), ++row, 0);
	grid->addWidget(m_ecsr = new QLabel(""), row, 1);
	
	vbox->addWidget(winfo);

// TDIG / SERDES Selector
	m_tblview = new QTableView();
	int fonth = QFontMetrics(m_tblview->font()).height();
	m_model = new KSerdesModel;
	m_tblview->setModel(m_model);
	m_tblview->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_tblview->setAlternatingRowColors(true);

	for(int i = 0; i < 8; ++i) {
		m_tblview->setRowHeight(i, 20);
	}
	int h = 20*8 + m_tblview->horizontalHeader()->height() + 8;
	m_tblview->setMinimumHeight(h);
	m_tblview->setMaximumHeight(h);
	m_tblview->setColumnWidth(0, 30);
	m_tblview->setColumnWidth(1, 80);
	m_tblview->setColumnWidth(2, 80);
	m_tblview->setMinimumWidth(30+80+80+8);

//	grid->addWidget(view, ++row, 0, 1, 2);
	vbox->addWidget(m_tblview);

	m_view = new KSerdesView(this);
	m_view->setSelectionModel(m_tblview->selectionModel());

//	grid->addWidget(m_view, ++row, 0, 1, 2);
	vbox->addWidget(m_view);

	vbox->addStretch();


}

KThubView::~KThubView()
{
// TODO Auto-generated destructor stub
}

void KThubView::currentRowChanged(const QModelIndex &current, const QModelIndex &parent)
{
  AnCanObject *cobj = static_cast<AnCanObject*>(current.internalPointer());
  if(AnThub *thub = dynamic_cast<AnThub*>(cobj)) {
    thub->sync(3);

    setTitle(thub->objectName());
    m_laddr->setText(thub->lAddress().toString());
    m_haddr->setText(thub->hAddress().toString());
    m_firm->setText(thub->firmwareString());
    m_temp1->setText(QString::number(thub->temp(0)));
    m_temp2->setText(QString::number(thub->temp(1)));
    m_ecsr->setText(thub->ecsrString(true));
//    m_ecsr->setToolTip(thub->ecsrString());

    m_model->setThub(thub);
  }
}
