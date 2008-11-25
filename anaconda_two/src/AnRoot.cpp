/*
 * AnRoot.cpp
 *
 *  Created on: Nov 10, 2008
 *      Author: koheik
 */

#include "AnRoot.h"
#include <QtCore/QVariant>
#include <QtCore/QDebug>

AnRoot::AnRoot(AnCanObject *parent) : AnCanObject (parent) {

	this->setObjectName("ROOT");

	m_cannet[0] = new AnCanNet(AnAddress(1, 0, 0, 0), AnAddress(0, 0, 0, 0));
	m_cannet[1] = new AnCanNet(AnAddress(2, 0, 0, 0), AnAddress(0, 0, 0, 0));
	m_cannet[0]->setList(&m_list[0]);
	m_cannet[1]->setList(&m_list[1]);

	m_db = QSqlDatabase::addDatabase("QSQLITE");
	m_db.setDatabaseName("db/configurations.db");
	if (!m_db.open() )
		qFatal("%s:%d; Cannot open configuration database.", __FILE__, __LINE__);
	QSqlQuery qry;
	qry.exec("SELECT id, active FROM devices");
	while(qry.next()) {
		int id     = qry.value(0).toInt();
		bool active = qry.value(1).toBool();
		if (active) { m_devid_list << id; }
	}
	m_socks = AnSock::open(m_devid_list);
	
	// create THUB objects
	qry.exec("SELECT id, device_id, canbus_id, active FROM thubs");
	while (qry.next()) {
		int id        = qry.value(0).toInt();
		int device_id = qry.value(1).toInt();
		int canbus_id = qry.value(2).toInt();
		bool active   = qry.value(3).toBool();
		if(!active) continue;
		m_list[0] << new AnThub(AnAddress(1, id, 0, 0),
										AnAddress(device_id, canbus_id, 0, 0), this);
	}
	
	// create TCPU objects
	qry.exec("SELECT id, device_id, canbus_id, active FROM tcpus");
	while (qry.next()) {
		int id        = qry.value(0).toInt();
		int device_id = qry.value(1).toInt();
		int canbus_id = qry.value(2).toInt();
		bool active   = qry.value(3).toBool();
		if(!active) continue;
		m_list[1] << new AnTcpu(AnAddress(2, id, 0, 0),
									AnAddress(device_id, canbus_id, 0, 0), this);
	}

	readModeList();
	setMode(0);
}

AnRoot::~AnRoot() {
	m_db.close();
	delete m_cannet[1];
	delete m_cannet[0];
}

void AnRoot::sync(int level)
{
	foreach(AnBoard *brd, list()) brd->sync(level);
}

void AnRoot::reset()
{
	foreach(AnBoard *brd, list()) brd->reset();
}

QStringList AnRoot::modeList() const
{
	QStringList list;
	foreach(mode md, m_mode_list) list << md.name;
	return list;
}

void AnRoot::setMode(int i)
{
	m_mode = m_mode_list[i].id;

	qDebug() << "new mode: " << m_mode;
	
	QSqlQuery qry;
	qry.prepare("SELECT id, addr1, addr2, addr3, addr4, val FROM configurations"
	            " WHERE config_set_id=:cs_id AND config_type_id=:ct_id ORDER BY rule_order");
	qry.bindValue(":cs_id", m_mode);

	// TCPU_PLD02
	qry.bindValue(":ct_id", CT_TCPU_PLDREG02);
	qry.exec();
	while (qry.next()) {
		int id        = qry.value(0).toInt();
		int addr1     = qry.value(1).toInt();
		int addr2     = qry.value(2).toInt();
		int addr3     = qry.value(3).toInt();
		int addr4     = qry.value(4).toInt();
		int val       = qry.value(5).toInt();
		AnAddress addr(addr1, addr2, addr3, addr4);
		qDebug() << id << addr.toString() << val;
		foreach(AnAddress ad, expand(addr)) {
			AnTcpu *tcpu = dynamic_cast<AnTcpu*>( find(ad) );
			if (tcpu) tcpu->setPldReg02(val);
			else qDebug() << "invalid address: " << ad.toString();
		}
	}

	// TDIG_THRESHOLD
	qry.bindValue(":ct_id", CT_TDIG_THRESHOLD);
	qry.exec();
	while (qry.next()) {
		int id        = qry.value(0).toInt();
		int addr1     = qry.value(1).toInt();
		int addr2     = qry.value(2).toInt();
		int addr3     = qry.value(3).toInt();
		int addr4     = qry.value(4).toInt();
		int val       = qry.value(5).toInt();
		AnAddress addr(addr1, addr2, addr3, addr4);
		qDebug() << id << addr.toString() << val;
		foreach(AnAddress ad, expand(addr)) {
			AnTdig *tdig = dynamic_cast<AnTdig*>( find(ad) );
			if (tdig) tdig->setThreshold(val);
			else qDebug() << "invalid address: " << ad.toString();
		}
	}

	// TDC_CONFIG
	qry.bindValue(":ct_id", CT_TDC_CONFIG);
	qry.exec();
	while (qry.next()) {
		int id        = qry.value(0).toInt();
		int addr1     = qry.value(1).toInt();
		int addr2     = qry.value(2).toInt();
		int addr3     = qry.value(3).toInt();
		int addr4     = qry.value(4).toInt();
		int val       = qry.value(5).toInt();
		AnAddress addr(addr1, addr2, addr3, addr4);
		qDebug() << id << addr.toString() << val;
		foreach(AnAddress ad, expand(addr)) {
			AnTdc *tdc = dynamic_cast<AnTdc*>( find(ad) );
			if (tdc) tdc->setConfig(val);
			else qDebug() << "invalid address: " << ad.toString();
		}
	}
}

AnCanObject *AnRoot::find(AnAddress &lad)
{
	return dynamic_cast<AnCanObject*>( at(lad.at(0))->at(lad.at(1))->at(lad.at(2))->at(lad.at(3)) );
}

QList<AnAddress> AnRoot::expand(AnAddress &lad)
{
	int a1 = lad.at(0);
	int a2 = lad.at(1);
	int a3 = lad.at(2);
	int a4 = lad.at(3);

	QList<AnAddress> lst1, lst2;

	if (a1 == 255)
		lst1 << AnAddress(1, a2, a3, a4) << AnAddress(2, a2, a3, a4);
	else
		lst1 << AnAddress(a1, a2, a3, a4);

	foreach(AnAddress ad, lst1) {
		a1 = ad.at(0);
		if (a1 == 1 && a2 == 255)
			for(int i = 1; i <= 4; ++i) lst2 << AnAddress(a1, i, a3, a4);
		else if (a1 == 2 && a2 == 255)
			for(int i = 1; i <= 120; ++i) lst2 << AnAddress(a1, i, a3, a4);
		else
			lst2 << AnAddress(a1, a2, a3, a4);
	}

	lst1.clear();
	foreach(AnAddress ad, lst2) {
		a1 = ad.at(0);
		a2 = ad.at(1);
		if (a3 == 255)
			for(int i = 1; i <= 8; ++i) lst1 << AnAddress(a1, a2, i, a4);
		else
			lst1 << AnAddress(a1, a2, a3, a4);
	}
	
	lst2.clear();
	foreach(AnAddress ad, lst1) {
		a1 = ad.at(0);
		a2 = ad.at(1);
		a3 = ad.at(2);
		if (a1 == 2 && a4 == 255)
			for(int i = 1; i <= 3; ++i) lst2 << AnAddress(a1, a2, a3, i);
		else
			lst2 << AnAddress(a1, a2, a3, a4);
	}

	return lst2;
}	

// private functions
void AnRoot::readModeList()
{
	m_mode_list.clear();

	QSqlQuery qry;
	qry.exec("SELECT id, name, description FROM config_sets");
	while(qry.next()) {
		mode md;
		md.id          = qry.value(0).toInt();
		md.name        = qry.value(1).toString();
		md.description = qry.value(2).toString();
		m_mode_list << md;
	}
}