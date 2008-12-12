/*
 * $Id$
 *
 *  Created on: Nov 20, 2008
 *      Author: koheik
 */

#include "KLevel1Model.h"
#include <QtCore/QDebug>

//-----------------------------------------------------------------------------
KLevel1Model::KLevel1Model(AnRoot *root, QObject *parent) :
	QAbstractTableModel(parent), m_root(root)
{
	m_sort_order = Qt::AscendingOrder;
	m_sort_column = 1;

	m_list = m_root->list(); // full copy the list

	foreach(AnCanObject *cobj, m_list)
	  cobj->sync(0);

	m_rows = m_list.count();
	m_columns = 6;

	m_selectionList << "All" << "THUBs" << "TCPUs";
	m_selectionList << m_root->deviceNames();
	m_selectionList << "Errors";

	m_statusIcon[0] = QIcon(":icons/black.png");
	m_statusIcon[1] = QIcon(":icons/blue.png");
	m_statusIcon[2] = QIcon(":icons/green.png");
	m_statusIcon[3] = QIcon(":icons/red.png");
	m_statusIcon[4] = QIcon(":icons/white.png");	

	connect(m_root, SIGNAL(updated(AnBoard*)), this, SLOT(updated(AnBoard*)));
	connect(m_root, SIGNAL(updated()),         this, SLOT(updated()));
}
//-----------------------------------------------------------------------------
int KLevel1Model::rowCount(const QModelIndex &parent) const
{
  return m_rows;
}

//-----------------------------------------------------------------------------
int KLevel1Model::columnCount(const QModelIndex &parent) const
{
  return m_columns;
}

//-----------------------------------------------------------------------------
QVariant KLevel1Model::data(const QModelIndex &index, int role) const
{
	int r, c;
	static Qt::SortOrder prev_order = m_sort_order;
	static int prev_column = m_sort_column;

	if (prev_order != m_sort_order ||
			prev_column != m_sort_column) { // rebuild mapping
		prev_order = m_sort_order;
		prev_column = m_sort_column;
	}

	if (m_sort_order == Qt::DescendingOrder) {
		r = m_rows - index.row() - 1;
	} else {
		r = index.row();
	}

	c = index.column();

	AnBoard *cobj = dynamic_cast<AnBoard*>(m_list.at(r));
	if (cobj == NULL) return QVariant();

	if (role == Qt::DisplayRole) {
		switch (c) {
			case 0: return QVariant();
			case 1: return cobj->name();
			case 2: return m_root->deviceNameByDevid(cobj->hAddress().at(0));
			case 3: return cobj->lvString();
			case 4: return cobj->hvString();
			case 5: return QString::number(cobj->maxTemp(), 'f', 2);
		}
	} else if (role == Qt::ToolTipRole) {
		switch(c) {
			case 1: return cobj->objectName();
			case 2: return cobj->hAddress().toString();
			default: return QVariant();
		}

	} else if (role == Qt::DecorationRole) {
		if (c == 0) return m_statusIcon[cobj->status()];

	} else if (role == Qt::TextAlignmentRole) {
		switch (c) {
			case 0: return Qt::AlignCenter;
			case 2: return Qt::AlignCenter;
			case 3: return Qt::AlignCenter;
			case 4: return Qt::AlignCenter;
			case 5: return Qt::AlignCenter;
		}
	}
	return QVariant();
}

//-----------------------------------------------------------------------------
QVariant KLevel1Model::headerData(int section,
								Qt::Orientation orientation, int role) const
{
  if(role != Qt::DisplayRole)
    return QVariant();

  if (orientation == Qt::Vertical) {
    return QString("%1").arg(section+1);
  } else
    switch (section) {
	case 0: return QVariant();
    case 1: return QString(tr("Name"));
    case 2: return QString(tr("CANBus Name"));
    case 3: return QString(tr("LV"));
    case 4: return QString(tr("HV"));
    case 5: return QString(tr("Max Temp"));
    }
    return QString("Field %1").arg(section + 1);
}

//-----------------------------------------------------------------------------
Qt::ItemFlags KLevel1Model::flags(const QModelIndex &index) const
{
  if(!index.isValid())
    return Qt::ItemIsEnabled;

  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

//-----------------------------------------------------------------------------
void KLevel1Model::toggleMode(int i) {
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

//-----------------------------------------------------------------------------
void KLevel1Model::sort(int column, Qt::SortOrder order)
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

//-----------------------------------------------------------------------------
QModelIndex KLevel1Model::index(int row, int column,
											const QModelIndex &parent) const
{
  return createIndex(row, column, m_list.at(row));
}

//-----------------------------------------------------------------------------
void KLevel1Model::sync(int level)
{
//	foreach(AnBoard *brd, m_list) brd->sync(level);
	m_root->sync();
	qDebug() << "end sync";
	reset();
}

//-----------------------------------------------------------------------------
void KLevel1Model::setSelection(int slt) {
	if (m_selection == slt) return;
	m_selection = slt;

	m_list.clear();
	if (m_selection == 0) {
		m_list = m_root->list();

	} else if (m_selection == 1) {
		foreach(AnBoard *b, m_root->list())
			if (dynamic_cast<AnThub*>(b)) m_list << b;

	} else if (m_selection == 2) {
		foreach(AnBoard *b, m_root->list())
			if (dynamic_cast<AnTcpu*>(b)) m_list << b;

	} else if (m_selection <= 2 + m_root->nDevices()) {
		quint8 devid = m_root->devidByIndex(m_selection - 3);
		foreach(AnBoard *b, m_root->list())
			if (dynamic_cast<AnBoard*>(b)->haddr().at(0) == devid) m_list << b;

	} else {
		foreach(AnBoard *b, m_root->list())
			if (b->status() == AnBoard::STATUS_ERROR) m_list << b;
	}

	m_rows = m_list.count();

	reset();
}

void KLevel1Model::updated(AnBoard *brd)
{
	int row;
	if ( (row = m_list.indexOf(brd)) >= 0) {
		QModelIndex m1 = createIndex(row, 0, brd);
		QModelIndex m2 = createIndex(row, m_columns, brd);
		emit dataChanged(m1, m2);
	}
}
void KLevel1Model::updated()
{
	reset();
}