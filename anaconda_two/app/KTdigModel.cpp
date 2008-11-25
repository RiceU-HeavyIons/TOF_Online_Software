/*
 * KTdigModel.cpp
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */
#include <QtCore/QDebug>

#include "KTdigModel.h"

KTdigModel::KTdigModel(QObject *parent)
 : QAbstractTableModel(parent), m_tcpu(0)
{
	m_statusIcon[0] = QIcon(":icons/black.png");
	m_statusIcon[1] = QIcon(":icons/blue.png");
	m_statusIcon[2] = QIcon(":icons/green.png");
	m_statusIcon[3] = QIcon(":icons/red.png");
}

QVariant KTdigModel::data(const QModelIndex &index, int role) const
{
  if (role == Qt::DisplayRole) {
    int r = index.row();
    switch (index.column()) {
    case 0: return QString("TDIG %1").arg(index.row() + 1);
    case 1: return m_tcpu == NULL ? QString("") : QString::number(m_tcpu->tdig(r+1)->temp(), 'f', 2);
    case 2: return m_tcpu == NULL ? QString("") : "0x" + QString::number(m_tcpu->tdig(r+1)->ecsr(), 16);
    }
  } else if (role == Qt::DecorationRole) {
    switch (index.column()) {
    case 0: return m_statusIcon[m_tcpu->tdig(index.row() + 1)->status()];
    }
  }
  return QVariant();
}

QVariant KTdigModel::headerData(int section, Qt::Orientation orientaion, int role) const
{
  if (role != Qt::DisplayRole)
    return QVariant();

  if (orientaion == Qt::Vertical)
    return QVariant();

  switch(section) {
  case 0: return QString("Name");
  case 1: return QString("Temp");
  case 2: return QString("ECSR");
  default:
    return QVariant();
  }
}

Qt::ItemFlags KTdigModel::flags(const QModelIndex &index) const
{
  if(!index.isValid())
    return Qt::ItemIsEnabled;

  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void KTdigModel::setTcpu(AnTcpu *tcpu)
{
  m_tcpu = tcpu;
  reset();
}

QModelIndex KTdigModel::index(int row, int column, const QModelIndex &parent) const
{
  if (m_tcpu == NULL)
    return createIndex(row, column, NULL);
  else
    return createIndex(row, column, m_tcpu->tdig(row+1));
}
