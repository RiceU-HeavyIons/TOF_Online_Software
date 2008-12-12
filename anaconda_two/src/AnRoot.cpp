/*
 * $Id$
 *
 *  Created on: Nov 10, 2008
 *      Author: koheik
 */

#include "AnRoot.h"
#include <QtCore/QVariant>
#include <QtCore/QDebug>

//-----------------------------------------------------------------------------
AnRoot::AnRoot(AnCanObject *parent) : AnCanObject (parent)
{

	this->setObjectName("Root");
	this->setActive(true);

	// setup AnCanNet objects for THUB/TCPU tree root
	for (int i = 1; i <= 2; ++i) {
		m_lnet[i] = AnCanNet(AnAddress(i, 0, 0, 0), AnAddress());
		m_lnet[i].setActive(true);
	}

	m_db = QSqlDatabase::addDatabase("QSQLITE");
	m_db.setDatabaseName("db/configurations.db");
	if (!m_db.open() )
		qFatal("%s:%d; Cannot open configuration database.", __FILE__, __LINE__);
	QSqlQuery qry;
	qry.exec("SELECT id, devid, name, active FROM devices");
	while(qry.next()) {
		int id       = qry.value(0).toInt();
		int devid    = qry.value(1).toInt();		
		QString name = qry.value(2).toString();
		bool active  = qry.value(3).toBool();

		m_devid_map[devid] = id;
		m_hnet[id] = AnCanNet(AnAddress(id, 0, 0, 0), AnAddress(devid, 0, 0, 0));
		m_hnet[id].setActive(active);
		m_hnet[id].setObjectName(name);
	}

	// open devices and create gents 
	m_agents = AnAgent::open(m_devid_map);

	// create THUB objects
	qry.exec("SELECT id, device_id, canbus_id, active FROM thubs");
	while (qry.next()) {
		int id        = qry.value(0).toInt();
		int device_id = qry.value(1).toInt();
		int canbus_id = qry.value(2).toInt();
		bool active   = qry.value(3).toBool();
		int devid     = m_hnet[device_id].devid();
		AnThub *th    = new AnThub(AnAddress(1, id, 0, 0),
		                           AnAddress(devid, canbus_id, 0, 0), this);
//		if(!active) continue;
		th->setActive(active);
		m_lnet[1][id] = th;
		m_hnet[device_id][canbus_id] = th;
		m_devid_thub_map[devid] = th;
	}

	// create TCPU objects
	qry.exec("SELECT id, device_id, canbus_id, active, tray_sn, serdes,"
	         " lv_box, lv_ch, hv_box, hv_ch FROM tcpus");
	while (qry.next()) {
		int id          = qry.value(0).toInt();
		int device_id   = qry.value(1).toInt();
		int canbus_id   = qry.value(2).toInt();
		bool active     = qry.value(3).toBool();
		QString tray_sn = qry.value(4).toString();
		QString serdes  = qry.value(5).toString();
		int lv_box      = qry.value(6).toInt();
		int lv_ch       = qry.value(7).toInt();
		int hv_box      = qry.value(8).toInt();
		int hv_ch       = qry.value(9).toInt();

		int devid = m_hnet[device_id].devid();
//		if(!active) continue;
		AnTcpu *tc = new AnTcpu(AnAddress(2, id, 0, 0),
		                        AnAddress(devid, canbus_id, 0, 0), this);
		tc->setActive(active);
//		tc->setTrayId(tray_id);
		tc->setTraySn(tray_sn);
		tc->setLvHv(lv_box, lv_ch, hv_box, hv_ch);

		m_lnet[2][id] = tc;
		m_hnet[device_id][canbus_id] = tc;

		if (active) {
			int nsrds = serdes[0].toAscii() - 'A' + 1;
			int port  = serdes[1].toAscii() - '0' + 1;
			if (nsrds < 1 || nsrds > 8 || port < 1 || port > 4) {
				qFatal("Invalid Serdes Address is found %d %d", nsrds, port);
			}
			thubByDevid(devid)->serdes(nsrds)->setTcpu(port, tc);
		}
	}
	readModeList();
	readTdcConfig();

	setMode(0);

	initAutoSync();

	// setup various maps and watches for incoming messages
	foreach(AnAgent *ag, m_agents) {
		m_devid_agent_map[ag->devid()] = ag;
		int sock = ag->socket();
		m_socket_agent_map[sock] = ag;
		m_watch[sock] = new QSocketNotifier(sock, QSocketNotifier::Read, this);
		connect(m_watch[sock], SIGNAL(activated(int)), this, SLOT(watcher(int)));
		m_watch[sock]->setEnabled(true);

		connect(ag, SIGNAL(finished(int)), this, SLOT(agentFinished(int)));
		connect(ag, SIGNAL(received(AnRdMsg)), this, SLOT(received(AnRdMsg)));
	}
}

//-----------------------------------------------------------------------------
AnRoot::~AnRoot()
{
	terminate();
	m_db.close();
	foreach(AnAgent *ag, m_agents) delete ag;
}

//-----------------------------------------------------------------------------
/**
 * Dump object
 */
QString AnRoot::dump() const
{
	QStringList sl;

	sl << QString().sprintf("AnRoot(%p):", this);
	sl << QString("  Name             : ") + name();
	sl << QString("  Hardware Address : ") + haddr().toString().toStdString().c_str();
	sl << QString("  Logical Address  : ") + laddr().toString().toStdString().c_str();
	sl << QString("  Active           : ") + (active() ? "yes" : "no");
	sl << QString("  Synchronized     : ") + synced().toString();

	return sl.join("\n");
}


void AnRoot::sync(int level)
{
	disableWatch();
	foreach (int id, m_hnet.keys()) {
		AnAgent *ag = agentById(id);
		if (ag->isRunning()) continue; // forget if agent is busy
		ag->init(TASK_SYNC, m_hnet[id].list());
		ag->start();
	}
//	for(int i = 0; i < n; ++i) m_agent[i].wait();
// TODO it's too early to set this flag
	setSynced();
}

//-----------------------------------------------------------------------------
void AnRoot::reset()
{
	disableWatch();
	foreach (int id, m_hnet.keys()) {
		AnAgent *ag = agentById(id);
		if (ag->isRunning()) continue; // forget if agent is busy
		ag->init(TASK_RESET, m_hnet[id].list());
		ag->start();
	}
//	for(int i = 0; i < n; ++i) m_agent[i].wait();
}

//-----------------------------------------------------------------------------
void AnRoot::config()
{
	disableWatch();
	foreach (int id, m_hnet.keys()) {
		AnAgent *ag = agentById(id);
		if (ag->isRunning()) continue; // forget if agent is busy
		ag->init(TASK_CONFIG, m_hnet[id].list());
		ag->start();
	}
//	for(int i = 0; i < n; ++i) m_agent[i].wait();
}

//-----------------------------------------------------------------------------
void AnRoot::write()
{
	disableWatch();
	foreach (int id, m_hnet.keys()) {
		AnAgent *ag = agentById(id);
		if (ag->isRunning()) continue; // forget if agent is busy
		ag->init(TASK_WRITE, m_hnet[id].list());
		ag->start();
	}
//	for(int i = 0; i < n; ++i) m_agent[i].wait();

}

//-----------------------------------------------------------------------------
// Agents support
bool AnRoot::isRunning() const
{
	bool ret = false;
	foreach(AnAgent *ag, m_agents) ret |= ag->isRunning();

	return ret;
}

void AnRoot::stop() const {
	foreach(AnAgent *ag, m_agents)
		if (ag->isRunning()) ag->stop();
	foreach(AnAgent *ag, m_agents) ag->wait();
}

void AnRoot::terminate() const {
	foreach(AnAgent *ag, m_agents) ag->stop();
	foreach(AnAgent *ag, m_agents) ag->terminate();
	foreach(AnAgent *ag, m_agents) ag->wait();
}

void AnRoot::wait() const {
	foreach(AnAgent *ag, m_agents) ag->wait();
}

//-----------------------------------------------------------------------------
QStringList AnRoot::modeList() const
{
	QStringList list;
	foreach(mode md, m_mode_list) list << md.name;
	return list;
}

//-----------------------------------------------------------------------------
void AnRoot::setMode(int i)
{
	m_mode = m_mode_list[i].id;

	qDebug() << "new mode: " << m_mode;

	QSqlQuery qry;
	qry.prepare("SELECT c.id, ct.name, c.addr1, c.addr2, c.addr3, c.addr4, c.val"
	            " FROM configs c INNER JOIN config_types ct on c.config_type_id = ct.id"
	            " WHERE c.config_set_id=:cs_id "
	            " ORDER BY c.config_set_order");
	qry.bindValue(":cs_id", m_mode);
	qry.exec();
	while (qry.next()) {
		int id        = qry.value(0).toInt();
		QString ct    = qry.value(1).toString();
		int addr1     = qry.value(2).toInt();
		int addr2     = qry.value(3).toInt();
		int addr3     = qry.value(4).toInt();
		int addr4     = qry.value(5).toInt();
		int val       = qry.value(6).toInt();
		AnAddress addr(addr1, addr2, addr3, addr4);

		// THUB
		if (ct == "THUB_ENABLE") {
			foreach(AnAddress ad, expand(addr)) {
				AnThub *thub = dynamic_cast<AnThub*>( find(ad) );
				if (thub) thub->setActive(val);
				else qDebug() << "invalid address: " << ad.toString();
			}
		}

		if (ct == "THUB_RESET") {
			foreach(AnAddress ad, expand(addr)) {
				AnThub *thub = dynamic_cast<AnThub*>( find(ad) );
				if (thub) thub->reset();
				else qDebug() << "invalid address: " << ad.toString();
			}
		}

		// Serdes
		if (ct == "SRDS_ENABLE") {
			foreach(AnAddress ad, expand(addr)) {
				AnSerdes *srds = dynamic_cast<AnSerdes*>( find(ad) );
				if (srds) srds->setActive(val);
				else qDebug() << "invalid address: " << ad.toString();
			}
		}
		if (ct == "SRDS_PLDREG9XBASE") {
			foreach(AnAddress ad, expand(addr)) {
				AnSerdes *srds = dynamic_cast<AnSerdes*>( find(ad) );
				if (srds) srds->setPld9xBase(val);
				else qDebug() << "invalid address: " << ad.toString();
			}
		}
		if (ct == "SRDS_RESET") {
			foreach(AnAddress ad, expand(addr)) {
				AnSerdes *srds = dynamic_cast<AnSerdes*>( find(ad) );
				if (srds) srds->reset();
				else qDebug() << "invalid address: " << ad.toString();
			}
		}

		// TCPU
		if (ct == "TCPU_ENABLE") {
			foreach(AnAddress ad, expand(addr)) {
				AnTcpu *tcpu = dynamic_cast<AnTcpu*>( find(ad) );
				if (tcpu) tcpu->setActive(val);
				else qDebug() << "invalid address: " << ad.toString();
			}
		}
		if (ct == "TCPU_PLDREG02") {
			foreach(AnAddress ad, expand(addr)) {
				AnTcpu *tcpu = dynamic_cast<AnTcpu*>( find(ad) );
				if (tcpu) tcpu->setPldReg02(val);
				else qDebug() << "invalid address: " << ad.toString();
			}
		}
		if (ct == "TCPU_RESET") {
			foreach(AnAddress ad, expand(addr)) {
				AnTcpu *tcpu = dynamic_cast<AnTcpu*>( find(ad) );
				if (tcpu) tcpu->reset();
				else qDebug() << "invalid address: " << ad.toString();
			}
		}

		// TDIG
		if (ct == "TDIG_ENABLE") {
			foreach(AnAddress ad, expand(addr)) {
				AnTdig *tdig = dynamic_cast<AnTdig*>( find(ad) );
				if (tdig) tdig->setActive(val);
				else qDebug() << "invalid address: " << ad.toString();
			}
		}
		if (ct == "TDIG_THRESHOLD") {
			foreach(AnAddress ad, expand(addr)) {
				AnTdig *tdig = dynamic_cast<AnTdig*>( find(ad) );
				if (tdig) tdig->setThreshold(val);
				else qDebug() << "invalid address: " << ad.toString();
			}
		}
		if (ct == "TDIG_RESET") {
			qDebug() << ct;
			foreach(AnAddress ad, expand(addr)) {
				AnTdig *tdig = dynamic_cast<AnTdig*>( find(ad) );
				if (tdig) tdig->reset();
				else qDebug() << "invalid address: " << ad.toString();
			}
		}

		// TDC
		if (ct == "TDC_ENABLE"){
			foreach(AnAddress ad, expand(addr)) {
				AnTdc *tdc = dynamic_cast<AnTdc*>( find(ad) );
				if (tdc) tdc->setActive(val);
				else qDebug() << "invalid address: " << ad.toString();
			}
		}	
		if (ct == "TDC_CONFIG"){
			foreach(AnAddress ad, expand(addr)) {
				AnTdc *tdc = dynamic_cast<AnTdc*>( find(ad) );
				if (tdc) tdc->setConfigId(val);
				else qDebug() << "invalid address: " << ad.toString();
			}
		}	
	}
	emit updated();
}

//-----------------------------------------------------------------------------
/**
 * Find object from logical address. Validation is not implemented and causes
 * segmentation fault very very easily.
 */
// TODO Include validation process
AnCanObject *AnRoot::find(const AnAddress &lad)
{
	return dynamic_cast<AnCanObject*>
				( at(lad.at(0))->at(lad.at(1))->at(lad.at(2))->at(lad.at(3)) );
}

// TODO Implement array version of find

/**
 * Expand wild card address. This function is really simple and desen't check
 * availability of addresses.
 */
// TODO Validate input and expanded address
QList<AnAddress> AnRoot::expand(const AnAddress &lad)
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

	// first round
	foreach(AnAddress ad, lst1) {
		a1 = ad.at(0);
		if (a1 == 1 && a2 == 255)
			for(int i = 1; i <= 4; ++i) lst2 << AnAddress(a1, i, a3, a4);
		else if (a1 == 2 && a2 == 255)
			for(int i = 1; i <= 120; ++i) lst2 << AnAddress(a1, i, a3, a4);
		else
			lst2 << AnAddress(a1, a2, a3, a4);
	}

	// second round
	lst1.clear();
	foreach(AnAddress ad, lst2) {
		a1 = ad.at(0);
		a2 = ad.at(1);
		if (a3 == 255)
			for(int i = 1; i <= 8; ++i) lst1 << AnAddress(a1, a2, i, a4);
		else
			lst1 << AnAddress(a1, a2, a3, a4);
	}

	// third round
	lst2.clear();
	foreach(AnAddress ad, lst1) {
		a1 = ad.at(0);
		a2 = ad.at(1);
		a3 = ad.at(2);
		if (a1 == 1 && a4 == 255)
			lst2 << AnAddress(a1, a2, a3, 0);
		else if (a1 == 2 && a4 == 255)
			for(int i = 1; i <= 3; ++i) lst2 << AnAddress(a1, a2, a3, i);
		else
			lst2 << AnAddress(a1, a2, a3, a4);
	}

	return lst2;
}

AnCanObject *AnRoot::hfind(const AnAddress &had)
{
	return dynamic_cast<AnCanObject*>
				( hat(had.at(0))->hat(had.at(1))->hat(had.at(2))->hat(had.at(3)) );
}

// QList<AnBoard*> AnRoot::boardsByIndex(int i) const
// {
// 	int id = m_hnet.values().at(i).laddr().at(0);
// 	qDebug() << id;
// 	qDebug() << m_hnet;
// 	qDebug() << m_hnet[id];
// 	qDebug() << m_hnet[id].list();
// 	return m_hnet[id].list();
// }

QStringList AnRoot::deviceNames() const
{
	QStringList st;
	foreach(AnCanNet a, m_hnet) st << a.name();
	return st;
}
//==============================================================================
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

void AnRoot::readTdcConfig()
{
	m_tcnfs.clear();

	QSqlQuery qry;
	qry.exec("SELECT id, name, length, checksum, bit_string FROM tdc_configs");
	while(qry.next()) {
		quint32 id       = qry.value(0).toUInt();
		QString name     = qry.value(1).toString();
		quint32 length   = qry.value(2).toUInt();
		quint32 checksum = qry.value(3).toUInt();
		QString bstr     = qry.value(4).toString();
		AnTdcConfig *cnf = new AnTdcConfig(this);
		cnf->setId(id);
		cnf->loadFromString(bstr);

		if (cnf->length() != length || cnf->checksum() != checksum) {
			qWarning("failed to load tdc_config[%d]", id);
		} else {
			qDebug() << "id" << id;
			m_tcnfs[id] = cnf;
		}

	}

	// copy to agents
	foreach (AnAgent *ag, m_agents) ag->setTdcConfigs(m_tcnfs);
}

/**
 * Initialize AutoSync
 */
void AnRoot::initAutoSync() {
	m_timer = new QTimer(this);
	m_cur1  = 1;
	m_cur2  = 1;
	connect(m_timer, SIGNAL(timeout()), this, SLOT(autosync()));
	m_timer->setInterval(2000);
//	m_timer->start();
}

/**
 * Start AutoSync
 */
void AnRoot::startAutoSync()
{
	if (!m_timer->isActive()) m_timer->start();
}

//==============================================================================
// Public Slots

/**
 * Stop AutoSync
 */
void AnRoot::stopAutoSync()
{
	if (m_timer->isActive()) m_timer->stop();
}

/**
 * AutoSync main function
 */
void AnRoot::autosync()
{

	if (!isRunning()) {
		qDebug() << "autosync" << m_cur1 << m_cur2;
		AnBoard *brd = m_lnet[m_cur1][m_cur2];
		disableWatch();
		brd->sync(2);
		enableWatch();
		emit updated(brd);
		++m_cur2;
		if ( m_cur2 >= m_lnet[m_cur1].count() ) {
			m_cur2 = 1;
			++m_cur1;
		}
		if (m_cur1 >= 2) m_cur1 = 1;
	}
}

/**
 * Enable all socket watches
 */
void AnRoot::enableWatch()
{
	foreach (QSocketNotifier *sn, m_watch) {
		sn->setEnabled(true);
	}
}

/**
 * Disable all socket watches
 */
void AnRoot::disableWatch()
{
	foreach (QSocketNotifier *sn, m_watch) {
		sn->setEnabled(false);
	}
}
/**
 * QSocketNotifier's activated signal triggers this function.
 */
void AnRoot::watcher(int sock)
{
	if (m_watch.contains(sock)) {
		// disable watch again
		m_watch[sock]->setEnabled(false);

		AnAgent *ag = m_socket_agent_map[sock];
		TPCANRdMsg rmsg;
		ag->read(rmsg, 4);
		received(AnRdMsg(ag->devid(), rmsg));

		// enable watch again
		m_watch[sock]->setEnabled(true);
	}
}

void AnRoot::agentFinished(int id)
{
	m_watch[agentById(id)->socket()]->setEnabled(true);
}

void AnRoot::received(AnRdMsg rmsg)
{
	AnBoard *brd = dynamic_cast<AnBoard*>(hfind(rmsg.source()));
	if (brd != NULL) {
		if (rmsg.payload() == 0x7) {
			if (rmsg.data() == 0xff000000) {
				brd->write();
			} else {
				qDebug() << "Received error message: " << rmsg;
			}
		}
	} else {
		qFatal("Received message from unknown source");
	}
}
