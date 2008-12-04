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
	for (int i = 0; i < 2; ++i) {
		m_cannet[i] = new AnCanNet(AnAddress(i+1, 0, 0, 0), AnAddress(0, 0, 0, 0));
		m_cannet[i]->setList(&m_list[i]);
		m_cannet[i]->setActive(true);
	}

	m_db = QSqlDatabase::addDatabase("QSQLITE");
	m_db.setDatabaseName("db/configurations.db");
	if (!m_db.open() )
		qFatal("%s:%d; Cannot open configuration database.", __FILE__, __LINE__);
	QSqlQuery qry;
	qry.exec("SELECT id, active FROM devices");
	while(qry.next()) {
		int id      = qry.value(0).toInt();
		bool active = qry.value(1).toBool();
		if (active) { m_devid_list << id; }
	}
	m_agents = AnAgent::open(m_devid_list);

	// create THUB objects
	qry.exec("SELECT id, device_id, canbus_id, active FROM thubs");
	while (qry.next()) {
		int id        = qry.value(0).toInt();
		int device_id = qry.value(1).toInt();
		int canbus_id = qry.value(2).toInt();
		bool active   = qry.value(3).toBool();
		AnThub *th = new AnThub(AnAddress(1, id, 0, 0),
										AnAddress(device_id, canbus_id, 0, 0), this);
//		if(!active) continue;
		th->setActive(active);
		m_list[0] << th;
	}

	// create TCPU objects
	qry.exec("SELECT id, device_id, canbus_id, active, tray_id, tray_sn FROM tcpus");
	while (qry.next()) {
		int id          = qry.value(0).toInt();
		int device_id   = qry.value(1).toInt();
		int canbus_id   = qry.value(2).toInt();
		bool active     = qry.value(3).toBool();
		int tray_id     = qry.value(4).toInt();
		QString tray_sn = qry.value(5).toString();

//		if(!active) continue;
		AnTcpu *tc = new AnTcpu(AnAddress(2, id, 0, 0),
									AnAddress(device_id, canbus_id, 0, 0), this);
		tc->setActive(active);
		tc->setTrayId(tray_id);
		tc->setTraySn(tray_sn);
		m_list[1] << tc;
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
	}
}

//-----------------------------------------------------------------------------
AnRoot::~AnRoot()
{
	terminate();
	m_db.close();
	delete   m_cannet[1];
	delete   m_cannet[0];
	foreach(AnAgent *ag, m_agents) delete ag;
}

//-----------------------------------------------------------------------------
/**
 * Dump object
 */
QString AnRoot::dump() const
{
	QStringList sl;

	sl << QString().sprintf("AnCanNet(%p):", this);
	sl << QString("  Name             : ") + name();
	sl << QString("  Hardware Address : ") + haddr().toString().toStdString().c_str();
	sl << QString("  Logical Address  : ") + laddr().toString().toStdString().c_str();
	sl << QString("  Active           : ") + (active() ? "yes" : "no");
	sl << QString("  Synchronized     : ") + synced().toString();
//	sl << QString("  Status           : ") + QString::number(status());
//	sl << QString("  East / West      : ") + (isEast()? "East" : "West");

	return sl.join("\n");
}


void AnRoot::sync(int level)
{
	int n = m_devid_list.count();
	QList<AnBoard*> blist[n];
	foreach(AnBoard *brd, list()) {
		blist[m_devid_list.indexOf(brd->hAddress().at(0))] << brd;
	}

	disableWatch();
	for(int i = 0; i < n; ++i) {
		AnAgent *ag = agent(m_devid_list[i]);
		if (ag->isRunning()) continue; // forget if agent is busy
		ag->init(TASK_SYNC, blist[i]);
		ag->start();
	}
//	for(int i = 0; i < n; ++i) m_agent[i].wait();
// TODO it's too early to set this flag
	setSynced();
}

//-----------------------------------------------------------------------------
void AnRoot::reset()
{
	int n = m_devid_list.count();
	QList<AnBoard*> blist[n];

	foreach(AnBoard *brd, list()) {
		blist[m_devid_list.indexOf(brd->hAddress().at(0))] << brd;
	}

	disableWatch();
	for(int i = 0; i < n; ++i) {
		AnAgent *ag = agent(m_devid_list[i]);
		if (ag->isRunning()) continue; // forget if agent is busy
		ag->init(TASK_RESET, blist[i]);
		ag->start();
	}
//	for(int i = 0; i < n; ++i) m_agent[i].wait();
}

//-----------------------------------------------------------------------------
void AnRoot::config()
{
	int n = nAgents();
	QList<AnBoard*> blist[n];

	foreach(AnBoard *brd, list()) {
		blist[m_devid_list.indexOf(brd->hAddress().at(0))] << brd;
	}

	disableWatch();
	foreach(AnAgent *ag, m_agents) {
		if (ag->isRunning()) continue; // forget if agent is busy
		ag->init(TASK_CONFIG, blist[m_devid_list.indexOf(ag->devid())]);
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
	qry.prepare("SELECT id, addr1, addr2, addr3, addr4, val FROM configs"
	            " WHERE config_set_id=:cs_id AND config_type_id=:ct_id "
	            " ORDER BY rule_order");
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
			if (tdc) tdc->setConfigId(val);
			else qDebug() << "invalid address: " << ad.toString();
		}
	}
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
	m_cur1  = 0;
	m_cur2  = 0;
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
		AnBoard *brd = m_list[m_cur1][m_cur2];
		disableWatch();
		brd->sync(2);
		enableWatch();
		emit updated(brd);
		++m_cur2;
		if ( m_cur2 >= m_list[m_cur1].count() ) {
			m_cur2 = 0;
			++m_cur1;
		}
		if (m_cur1 >= 2) m_cur1 = 0;
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
// TODO read message and deliver to the source device
//  parse
//  set powerup flag and etc on the device, i.e. THUB, TCPU or TDIG
		TPCANRdMsg rmsg;
		ag->read(rmsg, 4);

		// enable watch again
		m_watch[sock]->setEnabled(true);
	}
}
