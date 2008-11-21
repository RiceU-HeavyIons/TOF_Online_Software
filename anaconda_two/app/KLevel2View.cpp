/*
 * KLevel3View.cpp
 *
 *  Created on: Nov 7, 2008
 *      Author: koheik
 */
#include <QtCore/QDebug>

#include "AnTcpu.h"
#include "KLevel2View.h"
#include "KLevel3View.h"

KLevel2View::KLevel2View(QWidget *parent) : QAbstractItemView(parent) {

  QVBoxLayout *vb = new QVBoxLayout(this);

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
  vb->addWidget(m_label = new QLabel("hey hey"));
  vb->addWidget(view);
  vb->addWidget(l3view);
  vb->addStretch(1);

}

QModelIndex KLevel2View::indexAt(const QPoint &point) const
{
  return QModelIndex();
}

void KLevel2View::scrollTo(const QModelIndex &index, ScrollHint hint)
{
}


QRect KLevel2View::visualRect(const QModelIndex &index) const
{
  if(selectionModel()->selection().indexes().count() != 1)
    return QRect();

  if(currentIndex() != index)
    return QRect();

  return rect();
}

int KLevel2View::horizontalOffset() const
{
  return horizontalScrollBar()->value();
}

int KLevel2View::verticalOffset() const
{
  return verticalScrollBar()->value();
}

bool KLevel2View::isIndexHidden(const QModelIndex &index) const
{
  return false;
}

QModelIndex KLevel2View::moveCursor(CursorAction cursorAction,
                      Qt::KeyboardModifiers modifiers)
{
  return currentIndex();
}

void KLevel2View::setSelection(const QRect &rect,
                               QItemSelectionModel::SelectionFlags flags)
{

}

QRegion KLevel2View::visualRegionForSelection(const QItemSelection &selection) const
{
  return QRegion();
}

void KLevel2View::dataChanged(const QModelIndex &tofLeft,
                              const QModelIndex &bottomRight)
{
  updateAll();
}

void KLevel2View::selectionChanged(const QItemSelection &selected,
                                   const QItemSelection &deselected)
{
  updateAll();
}

void KLevel2View::updateAll()
{
  int cnt = selectionModel()->selection().indexes().count();
  qDebug() << cnt;

  QModelIndex idx = currentIndex();
  AnTcpu *tcpu = static_cast<AnTcpu*>(idx.internalPointer());

  switch(cnt / 5)
  {
  case 0:
    m_label->setText(tr("<i>No Data</i>"));
//    setDefault();
    break;

  case 1:
    qDebug() << tcpu->name();
    tcpu->sync(1);
    m_label->setText(tcpu->name());
//    m_chipid->setText("0xA4000000087BD6");
//    m_escr->setText("0xbb");
//    m_escr->setToolTip("[0] XXX-YYY<br />[1] YYY-ZZZ");
//    m_escr->setHidden(false);
    break;

  default:
    m_label->setText(tr("<i>Multiple Selection</i>"));
//    m_escr->setHidden(true);
//    setDefault();
  }

  return;
}

void KLevel2View::setDefault() {
//  m_chipid->setText("<font color='red'>--------</red>");
//  m_firmware->setText("---/--");
//  m_escr->setText("--");
}
