/*
 * KLevel3View.cpp
 *
 *  Created on: Nov 7, 2008
 *      Author: koheik
 */

#include <QtGui/QGridLayout>
#include "KLevel3View.h"

KLevel3View::KLevel3View(QWidget *parent) : QAbstractItemView(parent) {

  QGroupBox *box = new QGroupBox();
  box->setTitle(QString("TDIG 8"));
  QGridLayout *box_gl = new QGridLayout(this->viewport());
  box_gl->addWidget(m_label = new QLabel("text"), 0, 0, 1, 1);
  box_gl->addWidget(new QLabel("Chip ID:"), 1, 0, 1, 1);
  box_gl->addWidget(m_chipid = new QLabel("0xA4000000087BD6"), 1, 2, 1, 1);

  box_gl->addWidget(new QLabel("Firmware:"), 2, 0, 1, 1);
  box_gl->addWidget(m_firmware = new QLabel("11S/74"), 2, 2, 1, 1);

//  box_gl->addWidget(new QLabel("Temperature:"), 2, 0, 1, 1);
//  box_gl->addWidget(new QLabel("30.5555"), 2, 2, 1, 1);

  box_gl->addWidget(new QLabel("ESCR:"), 3, 0, 1, 1);
  box_gl->addWidget(m_escr = new QLabel("0xb0"), 3, 2, 1, 1);

  for(int i = 0; i < 3; ++i) {
    QLabel *l;
    box_gl->addWidget(l = new QLabel(QString("TDC %1:").arg(i+1)), 3*i+4, 0, 1, 1);
    box_gl->addWidget(new QLabel("Status:"), 3*i+4, 1, 1, 1);
    box_gl->addWidget(new QLabel("0xFFFFFFFFFF"), 3*i+4, 2, 1, 1);
    box_gl->addWidget(new QLabel("Mask:"), 3*i+5, 1, 1, 1);
    box_gl->addWidget(new QLabel("0b11111111"), 3*i+5, 2, 1, 1);
  }
  m_mainBox = box;
}

QModelIndex KLevel3View::indexAt(const QPoint &point) const
{
  return QModelIndex();
}

void KLevel3View::scrollTo(const QModelIndex &index, ScrollHint hint)
{
}


QRect KLevel3View::visualRect(const QModelIndex &index) const
{
  if(selectionModel()->selection().indexes().count() != 1)
    return QRect();

  if(currentIndex() != index)
    return QRect();

  return rect();
}

int KLevel3View::horizontalOffset() const
{
  return horizontalScrollBar()->value();
}

int KLevel3View::verticalOffset() const
{
  return verticalScrollBar()->value();
}

bool KLevel3View::isIndexHidden(const QModelIndex &index) const
{
  return false;
}

QModelIndex KLevel3View::moveCursor(CursorAction cursorAction,
                      Qt::KeyboardModifiers modifiers)
{
  return currentIndex();
}

void KLevel3View::setSelection(const QRect &rect,
                               QItemSelectionModel::SelectionFlags flags)
{

}

QRegion KLevel3View::visualRegionForSelection(const QItemSelection &selection) const
{
  return QRegion();
}

void KLevel3View::dataChanged(const QModelIndex &tofLeft,
                              const QModelIndex &bottomRight)
{
  updateAll();
}

void KLevel3View::selectionChanged(const QItemSelection &selected,
                                   const QItemSelection &deselected)
{
  updateAll();
}

void KLevel3View::updateAll()
{
  switch(selectionModel()->selection().indexes().count())
  {
  case 0:
    m_label->setText(tr("<i>No Data</i>"));
    setDefault();
    break;

  case 1:
    m_label->setText(model()->data(currentIndex()).toString());
    m_chipid->setText("0xA4000000087BD6");
    m_escr->setText("0xbb");
    m_escr->setToolTip("[0] XXX-YYY<br />[1] YYY-ZZZ");
    m_escr->setHidden(false);
    break;

  default:
    m_label->setText(tr("<i>Multiple Selection</i>"));
    m_escr->setHidden(true);
    setDefault();
  }

  return;
}

void KLevel3View::setDefault() {
  m_chipid->setText("<font color='red'>--------</red>");
  m_firmware->setText("---/--");
  m_escr->setText("--");
}