/*
 * $Id$
 *
 *  Created on: Nov 23, 2008
 *      Author: koheik
 */

#include "KThubView.h"

KThubView::KThubView(QWidget *parent) : QGroupBox("THUB", parent) {

  QGridLayout *grid = new QGridLayout(this);

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

// TDIG / SERDES Selector
  QTableView *view = new QTableView();
  int fonth = QFontMetrics(view->font()).height();
  m_model = new KSerdesModel;
  view->setModel(m_model);
  view->setSelectionBehavior(QAbstractItemView::SelectRows);
  for(int i = 0; i < 8; ++i) {
    view->setRowHeight(i, 20);
  }
  view->setMinimumHeight(9*fonth);
  view->setMaximumHeight(9*1.2*fonth);
  view->setColumnWidth(0, 30);
  view->setColumnWidth(1, 80);
  view->setColumnWidth(2, 80);

  grid->addWidget(view, ++row, 0, 1, 2);

  m_view = new KSerdesView(this);
  grid->addWidget(m_view, ++row, 0, 1, 2);

  QObject::connect(
        view->selectionModel(),
        SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
        m_view,
        SLOT(currentRowChanged(const QModelIndex &, const QModelIndex &)));

}

KThubView::~KThubView() {
  // TODO Auto-generated destructor stub
}

void KThubView::currentRowChanged(const QModelIndex &current, const QModelIndex &parent)
{
  AnCanObject *cobj = static_cast<AnCanObject*>(current.internalPointer());
  if(AnThub *thub = dynamic_cast<AnThub*>(cobj)) {
    thub->sync(1);

    setTitle(thub->name());
    m_laddr->setText(thub->lAddress().toString());
    m_haddr->setText(thub->hAddress().toString());
    m_firm->setText(thub->firmwareString());
    m_temp1->setText(QString::number(thub->temp(0)));
    m_temp2->setText(QString::number(thub->temp(1)));

    m_ecsr->setText("0x" + QString::number(thub->ecsr(), 16));
    m_ecsr->setToolTip(thub->ecsrString());

    m_model->setThub(thub);
  }
}
