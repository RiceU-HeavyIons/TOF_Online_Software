/*
 * $Id$
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#include "KTcpuView.h"
#include <QtGui/QToolBox>

KTcpuView::KTcpuView(QWidget *parent) : QGroupBox("TCPU", parent) {

  QGridLayout *grid = new QGridLayout(this);

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
  grid->addWidget(m_pld = new QLabel("----"),  row, 1);

// QToolBox *tab = new QToolBox();
// tab->addItem(new QLabel("they"), QIcon(":icons/blue.png"),  "TDIG 1");
// tab->addItem(new QLabel("they"), QIcon(":icons/black.png"), "TDIG 2");
// tab->addItem(new QLabel("they"), QIcon(":icons/black.png"), "TDIG 3");
// tab->addItem(new QLabel("they"), QIcon(":icons/black.png"), "TDIG 4");
// tab->addItem(new QLabel("they"), QIcon(":icons/black.png"), "TDIG 5");
// tab->addItem(new QLabel("they"), QIcon(":icons/black.png"), "TDIG 6");
// tab->addItem(new QLabel("they"), QIcon(":icons/black.png"), "TDIG 7");
// tab->addItem(new QLabel("they"), QIcon(":icons/black.png"), "TDIG 8");
// grid->addWidget(tab, ++row, 0, 1, 2);

// TDIG
  QTableView *view = new QTableView();
  m_model = new KTdigModel;
  view->setModel(m_model);
  view->setSelectionBehavior(QAbstractItemView::SelectRows);
  for(int i = 0; i < 8; ++i) {
    view->setRowHeight(i, 20);
  }
  view->setMinimumHeight(20*8);
  view->setColumnWidth(0, 100);
  view->setColumnWidth(1, 60);
  view->setColumnWidth(2, 60);

  grid->addWidget(view, ++row, 0, 1, 2);

  m_tdigView = new KTdigView(this);
  grid->addWidget(m_tdigView, ++row, 0, 1, 2);

  grid->spacing();

  QObject::connect(
        view->selectionModel(),
        SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
        m_tdigView,
        SLOT(currentRowChanged(const QModelIndex &, const QModelIndex &)));

}

KTcpuView::~KTcpuView() {
  // TODO Auto-generated destructor stub
}

void KTcpuView::currentRowChanged(const QModelIndex &current, const QModelIndex &parent)
{
  AnCanObject *cobj = static_cast<AnCanObject*>(current.internalPointer());
  AnTcpu *tcpu = dynamic_cast<AnTcpu*>(cobj);
  if(tcpu) {
    tcpu->sync(1);
    m_tcpu = tcpu;
    setTitle(tcpu->name());
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

    m_ecsr->setText("0x" + QString::number(tcpu->ecsr(), 16));
    m_ecsr->setToolTip(tcpu->ecsrString());

    m_pld->setText(tcpu->pldRegString());

    m_model->setTcpu(tcpu);
  }
}
