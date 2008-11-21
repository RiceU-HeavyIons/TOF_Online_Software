/*
 * KTdigView.cpp
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#include "KTdigView.h"

KTdigView::KTdigView(QWidget *parent) : QGroupBox("Txxx", parent) {

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
}

KTdigView::~KTdigView() {
  // TODO Auto-generated destructor stub
}

void KTdigView::currentRowChanged(const QModelIndex &current, const QModelIndex &parent)
{
  AnTdig *tdig = static_cast<AnTdig*>(current.internalPointer());
//  tdig->sync(1);

  setTitle(tdig->name());
  m_laddr->setText(tdig->lAddress().toString());
  m_haddr->setText(tdig->hAddress().toString());
  m_firm->setText(tdig->firmwareString());
  m_temp->setText(QString::number(tdig->temp()));

  m_ecsr->setText(QString::number(tdig->ecsr()));
  m_ecsr->setToolTip(tdig->ecsrString());

}
