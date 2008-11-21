/*
 * DetailView.cpp
 *
 *  Created on: Nov 7, 2008
 *      Author: koheik
 */

#include <QtCore/QStringList>

#include <QtGui/QSizePolicy>

#include <QtGui/QTableView>
#include <QtGui/QStringListModel>

#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>

#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QIcon>
#include <QtGui/QLabel>

#include <QtGui/QGroupBox>

#include "DetailView.h"
#include "KLevel3View.h"
#include "KLevel2View.h"

DetailView::DetailView(QWidget *parent) : QFrame(parent) {

  QVBoxLayout *vb = new QVBoxLayout(this);

  KLevel2View *l2view = new KLevel2View();

  QStringList list;
  QStandardItemModel *model = new QStandardItemModel(7, 1);
  QTableView *view = new QTableView();
  list  << "TCPU"
        << "TDIG 8" << "TDIG 7" << "TDIG 6" << "TDIG 5"
        << "TDIG 4" << "TDIG 3" << "TDIG 2" << "TDIG 1";
  for(int i = 0; i < list.size(); ++i) {
    QString str = list.at(i);
    if(i==0)
      model->setItem(i, 0, new QStandardItem(QIcon("./icons/blue.PNG"), str));
    else
      model->setItem(i, 0, new QStandardItem(QIcon("./icons/green.PNG"), str));
  }
  view->setModel(model);
  for(int i = 0; i < 9; i++) {
    view->setRowHeight(i, 20);
  }
  view->setMinimumHeight(250);

  QSizePolicy policy = view->sizePolicy();
  policy.setVerticalStretch(2);
  view->setSizePolicy(policy);
  view->selectRow(0);
  view->clearSelection();

  KLevel3View *l3view = new KLevel3View();
  l3view->setModel(model);
  l3view->setSelectionModel(view->selectionModel());

  vb->addWidget(new QPushButton("Reset"));
  vb->addWidget(view);
  vb->addWidget(l3view);
  vb->addStretch(1);
}
