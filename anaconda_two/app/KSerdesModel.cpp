/*
 * $Id$
 *
 *  Created on: Nov 22, 2008
 *      Author: koheik
 */
#include <QtCore/QDebug>
#include <QtGui/QIcon>

#include "KSerdesModel.h"
#include "AnBoard.h"
KSerdesModel::KSerdesModel(QObject *parent)
 : QAbstractTableModel(parent), m_thub(0)
{
	m_statusIcon[AnBoard::STATUS_UNKNOWN]  = QIcon(":icons/black.png");
	m_statusIcon[AnBoard::STATUS_STANDBY]  = QIcon(":icons/blue.png");
	m_statusIcon[AnBoard::STATUS_ON]       = QIcon(":icons/green.png");
	m_statusIcon[AnBoard::STATUS_WARNING]  = QIcon(":icons/orange.png");
	m_statusIcon[AnBoard::STATUS_ERROR]    = QIcon(":icons/red.png");
	m_statusIcon[AnBoard::STATUS_COMM_ERR] = QIcon(":icons/gray.png");
}

QVariant KSerdesModel::data(const QModelIndex &index, int role) const
{
	if (m_thub == NULL) return QVariant();

	if (role == Qt::DisplayRole) {
		int r = index.row();
		switch (index.column()) {
			case 0: return QVariant();
			case 1: return QString(tr("SERDES %1")).arg(index.row() + 1);
			case 2: return m_thub == NULL ? QString("") : "0x" + QString::number(m_thub->serdes(r+1)->ecsr(), 16);
		}
	} else if (role == Qt::DecorationRole) {
		switch (index.column()) {
			case 0: return m_statusIcon[m_thub->serdes(index.row() + 1)->status()];
		}
	} else if (role == Qt::SizeHintRole) {
		return QSize(20, 20);
	} else if (role == Qt::TextAlignmentRole) {
		switch (index.column()) {
			case 1: return Qt::AlignCenter;
			case 2: return Qt::AlignCenter;
		}
	}
	return QVariant();
}

QVariant KSerdesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
		switch(section) {
			case 1: return QString("Name");
			case 2: return QString("Register");
		}
	}
	if (role == Qt::SizeHintRole && orientation == Qt::Horizontal) {
		return QSize(0, 20);
	}
	if (role == Qt::SizeHintRole && orientation == Qt::Vertical) {
		return QSize(0, 0);
	}

	return QVariant();
}

Qt::ItemFlags KSerdesModel::flags(const QModelIndex &index) const
{
  if(!index.isValid())
    return Qt::ItemIsEnabled;

  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void KSerdesModel::setThub(AnThub *thub)
{
  m_thub = thub;
  reset();
}

QModelIndex KSerdesModel::index(int row, int column, const QModelIndex &parent) const
{
  if (m_thub == NULL)
    return createIndex(row, column, NULL);
  else
    return createIndex(row, column, m_thub->serdes(row+1));
}
