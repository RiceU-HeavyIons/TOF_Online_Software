/*
 * $Id$
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#include "KTcpuView.h"
#include <QtGui/QToolBox>
#include <QtGui/QHeaderView>

KTcpuView::KTcpuView(QWidget *parent) : QGroupBox("TCPU", parent)
{

	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->setContentsMargins(4, 4, 4, 4);

	QWidget *winfo = new QWidget(this);
//	winfo->setAutoFillBackground(true);
	QGridLayout *grid = new QGridLayout(winfo);
	grid->setSpacing(2);
	grid->setContentsMargins(2, 0, 2, 0);

	int row = -1;

	grid->addWidget(new QLabel("Tray SN:"), ++row, 0);
	grid->addWidget(m_tray = new QLabel(""),  row, 1);
	m_tray->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
	m_tray->setOpenExternalLinks(true);

// grid->addWidget(new QLabel("Logical Address:"), ++row, 0);
// grid->addWidget(m_laddr = new QLabel(""), row, 1);

	grid->addWidget(new QLabel("Hardware Address:"), ++row, 0);
	grid->addWidget(m_haddr = new QLabel(""), row, 1);


	grid->addWidget(new QLabel("Firmware:"), ++row, 0);
	grid->addWidget(m_firm = new QLabel(""), row, 1);

	grid->addWidget(new QLabel("Chip ID:"), ++row, 0);
	grid->addWidget(m_chip = new QLabel(""), row, 1);

	grid->addWidget(new QLabel("Temperature:"), ++row, 0);
	grid->addWidget(m_temp = new QLabel(""), row, 1);

	grid->addWidget(new QLabel("ECSR:"), ++row, 0);
	grid->addWidget(m_ecsr = new QLabel(""), row, 1);

	grid->addWidget(new QLabel("PLD Reg[02]:"), ++row, 0);
	grid->addWidget(m_pld02 = new QLabel("----"),  row, 1);

	grid->addWidget(new QLabel("PLD Reg[03]:"), ++row, 0);
	grid->addWidget(m_pld03 = new QLabel("----"),  row, 1);

	vbox->addWidget(winfo);
// TDIG
	m_view = new QTableView();
	m_model = new KTdigModel;
	m_view->setModel(m_model);
	m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_view->setAlternatingRowColors(true);

	for(int i = 0; i < 8; ++i) {
		m_view->setRowHeight(i, 20);
	}
	int h = 20*8 + m_view->horizontalHeader()->height() + 8;
	m_view->setMinimumHeight(h);
	m_view->setMaximumHeight(h);
	m_view->setColumnWidth(0, 30);
	m_view->setColumnWidth(1, 60);
	m_view->setColumnWidth(2, 60);
	m_view->setColumnWidth(3, 60);
	m_view->setMinimumWidth(30+60+60+60+8);

//	grid->addWidget(view, ++row, 0, 1, 2);
	vbox->addWidget(m_view);

	m_tdigView = new KTdigView(this);
//	grid->addWidget(m_tdigView, ++row, 0, 1, 2);
	vbox->addWidget(m_tdigView);

//	grid->addStretch(++row, 0, 1, 2);
//	grid->setSpacing(0);
	vbox->addStretch();
	m_tdigView->setSelectionModel(m_view->selectionModel());

}

KTcpuView::~KTcpuView()
{
// TODO Auto-generated destructor stub
}

void KTcpuView::currentRowChanged(const QModelIndex &current, const QModelIndex &parent)
{
  m_view->clearSelection();

  AnCanObject *cobj = static_cast<AnCanObject*>(current.internalPointer());
  AnTcpu *tcpu = dynamic_cast<AnTcpu*>(cobj);
  if (tcpu) {
    tcpu->sync(3);
    m_tcpu = tcpu;
    setTitle(tcpu->objectName());
    if (tcpu->traySn() != "") {
      QString ustr = QString("http://www.rhip.utexas.edu/~tofp/tray/view.php?sn=%1")
                            .arg(tcpu->traySn());
      m_tray->setText(QString("<a href=\"%1\">%2</a>").arg(ustr).arg(tcpu->traySn()));
      m_tray->setStatusTip(ustr);
    } else {
      m_tray->setText("");
      m_tray->setStatusTip("");
    }
//    m_tray->setEnabled(tcpu->traySn() != "");
    // m_laddr->setText(tcpu->lAddress().toString());
    m_haddr->setText(tcpu->hAddress().toString());
    m_firm->setText(tcpu->firmwareString());
	m_chip->setText(tcpu->chipIdString());
    m_temp->setText(tcpu->tempString());

    m_ecsr->setText(tcpu->ecsrString());
    m_ecsr->setToolTip(tcpu->ecsrToolTipString());

    m_pld02->setText(tcpu->pldReg02String(true));
    m_pld03->setText(tcpu->pldReg03String(true));

    m_model->setTcpu(tcpu);
  }
}
