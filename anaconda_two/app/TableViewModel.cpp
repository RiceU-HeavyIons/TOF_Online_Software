/*
 * TableViewModel.cpp
 *
 *  Created on: Nov 6, 2008
 *      Author: koheik
 */

#include "TableViewModel.h"
#include <QtCore/QDebug>
#include <QtGui/QIcon>

TableViewModel::TableViewModel(QObject *parent) :
  QAbstractTableModel(parent)
{
  m_sort_order = Qt::AscendingOrder;
  m_sort_column = 1;
  m_root = new AnRoot();
  foreach(AnCanObject *cobj, m_root->list())
    cobj->sync(0);

  m_rows = m_root->count();
  m_columns = 5;
}

int TableViewModel::rowCount(const QModelIndex &parent) const
{
  return m_rows;
}

int TableViewModel::columnCount(const QModelIndex &parent) const
{
  return m_columns;
}

QVariant TableViewModel::data(const QModelIndex &index, int role) const
{
  int r, c;
  static Qt::SortOrder prev_order = m_sort_order;
  static int prev_column = m_sort_column;

  if (prev_order != m_sort_order ||
      prev_column != m_sort_column) { // rebuild mapping

    prev_order = m_sort_order;
    prev_column = m_sort_column;
  }

  if(m_sort_order == Qt::DescendingOrder) {
    r = m_rows - index.row() - 1;
  } else {
    r = index.row();
  }
  c = index.column();
  AnTcpu *cobj = dynamic_cast<AnTcpu*>(m_root->at(r));
  if(cobj == NULL) return QVariant();

  switch(role) {
  case Qt::DisplayRole:
    switch(c) {
    case 0: return cobj->name();
    case 1: return cobj->lAddress().toString();
    case 2: return (r / 30)? QString("East") : QString("West");
    case 3: return cobj->temp();
    case 4: return cobj->ecsr();
    default:
      return (r) * (c);
    }
    break;

  case Qt::ToolTipRole:
    return QString("%1 x %2").arg(r).arg(c);
  case Qt::DecorationRole:
    if(c == 0) return QIcon("./icons/blue.PNG");
  default:
    return QVariant();
  }
}

QVariant TableViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Vertical) {
    return QString("%1").arg(section+1);
  } else
    switch(section) {
    case 0:
      return QString(tr("Name"));
    case 1:
      return QString(tr("Address"));
    case 2:
      return QString(tr("East/West"));
    case 3:
      return QString(tr("Max Temp"));
    case 4:
      return QString(tr("ECSR"));
    }
    return QString("Field %1").arg(section + 1);
}

Qt::ItemFlags TableViewModel::flags(const QModelIndex &index) const
{
  if(!index.isValid())
    return Qt::ItemIsEnabled;

  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void TableViewModel::toggleMode(int i) {
  qDebug() << "got " << i;

  if(i == 0) {
    m_rows = 2;
    m_columns = 2;
  } else {
    m_rows = 10;
    m_columns = 10;
  }
  reset();
}

void TableViewModel::sort(int column, Qt::SortOrder order)
{
  switch(order) {
  case Qt::DescendingOrder:
    qDebug() << "descending";
    break;
  case Qt::AscendingOrder:
    qDebug() << "ascending";
  }
  qDebug() << "column: " << column;
  m_sort_column = column;
  m_sort_order = order;
  reset();
}

QModelIndex TableViewModel::index(int row, int column, const QModelIndex &parent) const
{
  return createIndex(row, column, m_root->at(row));
}

