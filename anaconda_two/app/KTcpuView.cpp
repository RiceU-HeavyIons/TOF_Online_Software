/*
 * KTcpuView.cpp
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#include "KTcpuView.h"

KTcpuView::KTcpuView(QWidget *parent) : QGroupBox("Txxx", parent) {

  QGridLayout *grid = new QGridLayout(this);

  int row = 0;
  grid->addWidget(new QLabel("Logical Address:"), row, 0);
  grid->addWidget(m_laddr = new QLabel(""), row, 1);

  grid->addWidget(new QLabel("Hardware Address:"), ++row, 0);
  grid->addWidget(m_haddr = new QLabel(""), row, 1);

  grid->addWidget(new QLabel("Firmware:"), ++row, 0);
  grid->addWidget(m_firm = new QLabel(""), row, 1);

  grid->addWidget(new QLabel("Temperature:"), ++row, 0);
  grid->addWidget(m_temp = new QLabel(""), row, 1);

  grid->addWidget(new QLabel("ECSR:"), ++row, 0);
  grid->addWidget(m_ecsr = new QLabel(""), row, 1);

// TDIG / SERDES Selector
  QTableView *view = new QTableView();
  m_model = new TdigTableModel;
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

    setTitle(tcpu->name());
    m_laddr->setText(tcpu->lAddress().toString());
    m_haddr->setText(tcpu->hAddress().toString());
    m_firm->setText(tcpu->firmwareString());
    m_temp->setText(QString::number(tcpu->temp()));

    m_ecsr->setText(QString::number(tcpu->ecsr()));
    m_ecsr->setToolTip(tcpu->ecsrString());

    m_model->setTcpu(tcpu);
  }
}
