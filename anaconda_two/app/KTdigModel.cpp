/*
 * $Id$
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
	m_statusIcon[4] = QIcon(":icons/gray.png");
}

QVariant KTdigModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
		int r = index.row();
		switch (index.column()) {
			case 1: return QString(tr("TDIG %1")).arg(index.row() + 1);
			case 2: return m_tcpu == NULL ? QString("") : QString::number(m_tcpu->tdig(r+1)->temp(), 'f', 2);
			case 3: return m_tcpu == NULL ? QString("") : "0x" + QString::number(m_tcpu->tdig(r+1)->ecsr(), 16);
			case 4: return m_tcpu == NULL ? QString("") : "0x" + QString::number(m_tcpu->tdig(r+1)->pldReg03(), 16);
		}
	} else if (role == Qt::DecorationRole) {
		switch (index.column()) {
			case 0: return m_statusIcon[m_tcpu->tdig(index.row() + 1)->status()];
		}
	} else if (role == Qt::SizeHintRole) {
		return QSize(20, 20);
	} else if (role == Qt::TextAlignmentRole) {
		switch (index.column()) {
			case 1: return Qt::AlignCenter;
			case 2: return Qt::AlignCenter;
			case 3: return Qt::AlignCenter;
			case 4: return Qt::AlignCenter;
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
  case 1: return QString("Name");
  case 2: return QString("Temp");
  case 3: return QString("ECSR");
  case 4: return QString("PLD[03]");
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
