/*
 * AnRoot.cpp
 *
 *  Created on: Nov 10, 2008
 *      Author: koheik
 */

#include "AnRoot.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtCore/QVariant>
#include <QtCore/QDebug>

AnRoot::AnRoot(AnCanObject *parent) : AnCanObject (parent) {

	this->setObjectName("ROOT");

  // m_list << new AnThub(AnLAddress(30, 0, 0), AnHAddress(253, 0x40, 0, 0), this);
  // 
  // for(int i = 0; i < 30; i++) {
  //   AnTcpu *tcpu = new AnTcpu(AnLAddress(i+1, 0, 0),
  //                             AnHAddress(255, 0x20, 0, 0),
  //                             this);
  //   m_list << tcpu;
  // }

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("db/configurations.db");
	if (!db.open() )
		qFatal("%s:%d; Couldn't open configuration database.", __FILE__, __LINE__);
	QSqlQuery qry;
	qry.exec("SELECT id, active FROM devices");
	QList<int> dev_id_list;
	while(qry.next()) {
		int id    = qry.value(0).toInt();
		int active = qry.value(1).toInt();
		if (active) { dev_id_list << id; }
		qDebug() << id;
	}
	m_socks = AnSock::open(dev_id_list);
	
	qry.exec("SELECT id, type_id, device_id, canbus_id, active FROM configurations");
	while (qry.next()) {
		int id        = qry.value(0).toInt();
		int type_id   = qry.value(1).toInt();
		int device_id = qry.value(2).toInt();
		int canbus_id = qry.value(3).toInt();
		int active    = qry.value(4).toInt();
		if(!active) continue;
		qDebug() << id << device_id << canbus_id;
		switch(type_id) {
			case 1:	m_list << new AnThub(AnLAddress(id, 0, 0),
										AnHAddress(device_id, canbus_id, 0, 0), this);
			break;
			case 2:	m_list << new AnTcpu(AnLAddress(id, 1, 0),
										AnHAddress(device_id, canbus_id, 0, 0), this);
			break;
		}
	}
}

AnRoot::~AnRoot() {
  // TODO Auto-generated destructor stub
}
