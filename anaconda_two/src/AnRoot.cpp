/*
 * $Id$
 *
 *  Created on: Nov 10, 2008
 *      Author: koheik
 */
#include <iostream>
using namespace std;

#include "AnRoot.h"
#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QDateTime>

#include <QtGui/QApplication>
#include <QtSql/QSqlQuery>

#include <signal.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "AnMaster.h"
#include "AnEpicsLogger.h"
#include "AnExceptions.h"

/*
  JS:
  To create this version.h file:
  echo -n "#define SVN_VERSION \"" > src/version.h
  svnversion -n . >> src/version.h 
  echo -n "\"" >> src/version.h 
  echo ""  >> src/version.h 
*/

#include "version.h"

// static member initialization
int AnRoot::TCAN_AUTOREPAIR = 0;

//-----------------------------------------------------------------------------
AnRoot::AnRoot(AnCanObject *parent) : AnCanObject (parent)
{
  // lockfile not yet opened
  lockFileFd = -1;
  
  // install a signal handler for TERM and INT signals
  signal(SIGINT,handler);
  signal(SIGTERM,handler);

  // create lock file, or open, if already exists
  umask(0);
  lockFileFd = open(LOCK_FILENAME, O_WRONLY | O_CREAT, 0666);
  // try locking it exclusively
  if(flock(lockFileFd, LOCK_EX | LOCK_NB) != 0) {
    // can't have two instances of Anaconda running
    printf("AnacondaII is already running. Quitting this instance...\n");
    close(lockFileFd);
    lockFileFd = -1;
    exit(0);
  }

  this->setObjectName("Root");
  this->setActive(true);
  
  // setup AnCanNet objects for THUB/TCPU tree root
  for (int i = 1; i <= 2; ++i) {
    m_lnet[i] = AnCanNet(AnAddress(i, 0, 0, 0), AnAddress());
    m_lnet[i].setActive(true);
  }
  
  m_db = QSqlDatabase::addDatabase("QSQLITE");
  
#ifdef __APPLE__
  // With Darwin environment, binary image is under .app/Contents/MacOS/ directory...
  QDir default_path( QString("%1/../../..").arg(QApplication::applicationDirPath()) );
#else
  QDir default_path( QString("%1").arg(QApplication::applicationDirPath()) );
#endif
  char *dbpath = getenv(DB_PATH_ENV);
  if (dbpath != NULL && *dbpath != '\0') {
    m_db.setDatabaseName(dbpath);
  } else {
    m_db.setDatabaseName(default_path.filePath(DB_PATH_DEFAULT));
  }
  
  if (!m_db.open())
    qFatal("%s:%d; Cannot open configuration database.", __FILE__, __LINE__);
  
  QDateTime now = QDateTime::currentDateTime();
  char *logdir = getenv("AII_LOG_DIR");
  if (logdir != NULL && *logdir != '\0') {
    // m_log   = new AnLog(QString("%1/%2").arg(logdir).arg(now.toString("yyyyMMdd.log")));
    // m_tlog  = new AnLog(QString("%1/temp%2").arg(logdir).arg(now.toString("yyyyMMdd.log")));
    m_log   = new AnLog(QString("%1/anaconda.log").arg(logdir));
    m_tlog  = new AnLog(QString("%1/temperature.log").arg(logdir));
  }
  else {
    m_log  = new AnLog(default_path.filePath(now.toString("log/yyyyMMdd.log")));
    m_tlog = new AnLog(default_path.filePath(QString("log/temp%1").arg(now.toString("yyyyMMdd.log"))));
  }
  
  log(QString("AnRoot::AnacondaSVNVersion: %1").arg(SVN_VERSION));

  QSqlQuery qry;
  qry.exec("SELECT id, devid, name, installed FROM devices");
  while(qry.next()) {
    int id          = qry.value(0).toInt();
    int devid       = qry.value(1).toInt();		
    QString name    = qry.value(2).toString();
    bool installed  = qry.value(3).toBool();
    
    m_devid_map[devid] = id;
    m_hnet[id] = AnCanNet(AnAddress(id, 0, 0, 0), AnAddress(devid, 0, 0, 0));
    m_hnet[id].setInstalled(installed);
    m_hnet[id].setActive(installed);
    m_hnet[id].setObjectName(name);
  }
  
  // open devices and create agents 
  m_agents = AnAgent::open(m_devid_map);

  // create THUB objects
  m_numThub = 0;
  qry.exec("SELECT id, device_id, canbus_id, installed FROM thubs");
  while (qry.next()) {
    m_numThub++;
    int id         = qry.value(0).toInt();
    int device_id  = qry.value(1).toInt();
    int canbus_id  = qry.value(2).toInt();
    bool installed = qry.value(3).toBool();
    int devid      = m_hnet[device_id].devid();
    AnThub *th     = new AnThub(AnAddress(1, id, 0, 0),
				AnAddress(devid, canbus_id, 0, 0), this);
    //		if(!active) continue;
    th->setInstalled(installed);
    th->setActive(installed);
    m_lnet[1][id] = th;
    m_hnet[device_id][canbus_id] = th;
    m_devid_thub_map[devid] = th;
  }
  
  // create TCPU objects
  m_numTcpu = 0;
  qry.exec("SELECT id, device_id, canbus_id, installed, tray_sn, thub_id, serdes,"
	   " lv_box, lv_ch, hv_box, hv_ch FROM tcpus");
  while (qry.next()) {
    m_numTcpu++;
    int id          = qry.value(0).toInt();
    int device_id   = qry.value(1).toInt();
    int canbus_id   = qry.value(2).toInt();
    bool installed  = qry.value(3).toBool();
    QString tray_sn = qry.value(4).toString();
    int thub_id     = qry.value(5).toInt();
    QString serdes  = qry.value(6).toString();
    int lv_box      = qry.value(7).toInt();
    int lv_ch       = qry.value(8).toInt();
    int hv_box      = qry.value(9).toInt();
    int hv_ch       = qry.value(10).toInt();
    
    int devid = m_hnet[device_id].devid();
    //		if(!active) continue;
    AnTcpu *tc = new AnTcpu(AnAddress(2, id, 0, 0),
			    AnAddress(devid, canbus_id, 0, 0), this);

    // for MTD:
    if (id > 122) tc->setName(QString("MTD  %1").arg(tray_sn));
    tc->setInstalled(installed);
    tc->setActive(installed);
    //		tc->setTrayId(tray_id);
    tc->setTraySn(tray_sn);
    tc->setLvHv(lv_box, lv_ch, hv_box, hv_ch);
    
    tc->setThub(thub_id);
    
    m_lnet[2][id] = tc;
    m_hnet[device_id][canbus_id] = tc;
    
    if (installed) {
      int nsrds = serdes[0].toAscii() - 'A' + 1;
      int port  = serdes[1].toAscii() - '0' + 1;
      if (nsrds < 1 || nsrds > 8 || port < 1 || port > 4) {
	qFatal("Invalid Serdes Address is found %d %d", nsrds, port);
      }
      dynamic_cast<AnThub*>(m_lnet[1][thub_id])->serdes(nsrds)->setTcpu(port, tc);
      tc->setSerdes(nsrds);
      tc->setSerdesPort(port);
    }
  }
  readModeList();
  readTdcConfig();

  // This starts a new thread for logging temperatures to EPICS
  m_epicsLogger = new AnEpicsLogger(this);
  m_epicsLogger->start();

  m_master = new AnMaster(this);
  setMode(0);

  initAutoSync();



  // setup various maps and watches for incoming messages
  foreach(AnAgent *ag, m_agents) {
    ag->setRoot(this);
    ag->setDeviceName(m_hnet[ag->id()].objectName());
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
 
  // if lockfile is open, unlock it
  if(lockFileFd != -1) {
    flock(lockFileFd, LOCK_UN | LOCK_NB);
    close(lockFileFd);
  }
  
  log("AnacondaII terminating");
  delete m_log;
  delete m_tlog;
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
  sl << QString("  Installed        : ") + (installed() ? "yes" : "no");
  sl << QString("  Active           : ") + (active() ? "yes" : "no");
  sl << QString("  Synchronized     : ") + synced().toString();

  return sl.join("\n");
}

//-----------------------------------------------------------------------------
void AnRoot::init(int level)
{
  if (level >= 1) init(level, list());
}

//-----------------------------------------------------------------------------
void AnRoot::config(int level)
{
  if(level >= 1) config(level, list());
}

//-----------------------------------------------------------------------------
void AnRoot::reset(int level)
{
  if(level >= 1) reset(level, list()); // reset all
}

//-----------------------------------------------------------------------------
void AnRoot::qreset(int level)
{
  if(level >= 1) qreset(level, list()); // reset all
}

//-----------------------------------------------------------------------------
void AnRoot::sync(int level)
{
  if(level >= 1) sync(level, list()); // sync all
}

void AnRoot::relink(int level, const QList<AnBoard*>& blist)
{
  QMap<int, QList<AnBoard*> > bmap;
  foreach (AnBoard* bd, blist)
    bmap[deviceIdFromDevid(bd->haddr().at(0))] << bd;

  emit aboutStart();
  foreach (int id, bmap.keys()) {
    AnAgent *ag = agentById(id);
    if (ag->isRunning()) continue; // forget if agent is busy
    ag->init(TASK_RELINK, level, bmap[id]);
    ag->start();
  }
}

//-----------------------------------------------------------------------------
void AnRoot::init(int level, const QList<AnBoard*>& blist)
{
  QMap<int, QList<AnBoard*> > bmap;
  foreach (AnBoard* bd, blist)
    bmap[deviceIdFromDevid(bd->haddr().at(0))] << bd;

  emit aboutStart();
  foreach (int id, bmap.keys()) {
    AnAgent *ag = agentById(id);
    if (ag->isRunning()) continue; // forget if agent is busy
    ag->init(TASK_INIT, level, bmap[id]);
    ag->start();
  }
}

//-----------------------------------------------------------------------------
void AnRoot::config(int level, const QList<AnBoard*>& blist)
{
  QMap<int, QList<AnBoard*> > bmap;
  foreach (AnBoard* bd, blist)
    bmap[deviceIdFromDevid(bd->haddr().at(0))] << bd;

  emit aboutStart();
  foreach (int id, bmap.keys()) {
    AnAgent *ag = agentById(id);
    if (ag->isRunning()) continue; // forget if agent is busy
    ag->init(TASK_CONFIG, level, bmap[id]);
    ag->start();
  }
}

//-----------------------------------------------------------------------------
void AnRoot::reset(int level, const QList<AnBoard*>& blist)
{
  QMap<int, QList<AnBoard*> > bmap;
  foreach (AnBoard* bd, blist)
    bmap[deviceIdFromDevid(bd->haddr().at(0))] << bd;

  emit aboutStart();
  foreach (int id, bmap.keys()) {
    AnAgent *ag = agentById(id);
    //		qDebug() << "AnRoot::reset id" << id << "ag->id()" << ag->id();
    if (ag->isRunning()) continue; // forget if agent is busy
    ag->init(TASK_RESET, level, bmap[id]);
    ag->start();
  }
}

//-----------------------------------------------------------------------------
void AnRoot::qreset(int level, const QList<AnBoard*>& blist)
{
  QMap<int, QList<AnBoard*> > bmap;
  foreach (AnBoard* bd, blist)
    bmap[deviceIdFromDevid(bd->haddr().at(0))] << bd;

  emit aboutStart();
  foreach (int id, bmap.keys()) {
    AnAgent *ag = agentById(id);
    //		qDebug() << "AnRoot::reset id" << id << "ag->id()" << ag->id();
    if (ag->isRunning()) continue; // forget if agent is busy
    ag->init(TASK_QRESET, level, bmap[id]);
    ag->start();
  }
}

//-----------------------------------------------------------------------------
void AnRoot::tcpurecovery(int level, const QList<AnBoard*>& blist)
{
  QMap<int, QList<AnBoard*> > bmap;
  foreach (AnBoard* bd, blist)
    bmap[deviceIdFromDevid(bd->haddr().at(0))] << bd;

  emit aboutStart();
  foreach (int id, bmap.keys()) {
    AnAgent *ag = agentById(id);
    //		qDebug() << "AnRoot::reset id" << id << "ag->id()" << ag->id();
    if (ag->isRunning()) continue; // forget if agent is busy
    ag->init(TASK_TCPURECOVERY, level, bmap[id]);
    ag->start();
  }
}

//-----------------------------------------------------------------------------
void AnRoot::thubrecovery(int level, const QList<AnBoard*>& blist)
{
  QMap<int, QList<AnBoard*> > bmap;
  foreach (AnBoard* bd, blist)
    bmap[deviceIdFromDevid(bd->haddr().at(0))] << bd;

  emit aboutStart();
  foreach (int id, bmap.keys()) {
    AnAgent *ag = agentById(id);
    //		qDebug() << "AnRoot::reset id" << id << "ag->id()" << ag->id();
    if (ag->isRunning()) continue; // forget if agent is busy
    ag->init(TASK_THUBRECOVERY, level, bmap[id]);
    ag->start();
  }
}


/**
 * Sync all boards in the list
 */
void AnRoot::sync(int level, const QList<AnBoard*>& blist)
{
  QMap<int, QList<AnBoard*> > bmap;
  foreach (AnBoard* bd, blist)
    bmap[deviceIdFromDevid(bd->haddr().at(0))] << bd;

  emit aboutStart();
  foreach (int id, bmap.keys()) {
    AnAgent *ag = agentById(id);
    if (ag->isRunning()) {
      qDebug() << "agent is running and can't issue next task";
      continue; // forget if agent is busy
    }
    ag->init(TASK_SYNC, level, bmap[id]);
    ag->start();
  }

  //	for(int i = 0; i < n; ++i) m_agent[i].wait();
  // TODO it's too early to set this flag
  setSynced();
}

//-----------------------------------------------------------------------------
int AnRoot::status()
{
  int hist[5];
  for(int i = 0; i < 5; ++i) hist[i] = 0;

  foreach (AnBoard *bd, list()) {
    if (bd->active())
      hist[bd->status()]++;
  }
  int ret = 0;
  if(hist[AnBoard::STATUS_ERROR]) ret = AnBoard::STATUS_ERROR;
  if(hist[AnBoard::STATUS_COMM_ERR]) ret = AnBoard::STATUS_COMM_ERR;
  return ret;
}

//-----------------------------------------------------------------------------
// Agents support
bool AnRoot::isRunning() const
{
  bool ret = false;
  foreach(AnAgent *ag, m_agents) ret |= ag->isRunning();

  return ret;
}

//-----------------------------------------------------------------------------
void AnRoot::stop() const {
  foreach(AnAgent *ag, m_agents)
    if (ag->isRunning()) ag->stop();
  foreach(AnAgent *ag, m_agents) ag->wait();
}

//-----------------------------------------------------------------------------
void AnRoot::terminate() const {
  qDebug() << "AnRoot::terminate";
  foreach(AnAgent *ag, m_agents) ag->stop();
  foreach(AnAgent *ag, m_agents) ag->terminate();
  foreach(AnAgent *ag, m_agents) ag->wait();
  m_master->terminate();
  m_master->wait();
}

//-----------------------------------------------------------------------------
void AnRoot::wait() const {
  foreach(AnAgent *ag, m_agents) ag->wait();
}

//-----------------------------------------------------------------------------
void AnRoot::agentFinished(int __attribute__((unused)) id)
{
  if(!isRunning()) {
    emit finished();
    emit updated();
  }
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
  log(QString("AnRoot::setMode: %1").arg(i+1));
  //	disableWatch();
  m_mode_idx = i;
  m_mode = m_mode_list[i].id;
  emit aboutStart();
  m_master->setMode(m_mode);
}

//-----------------------------------------------------------------------------
void AnRoot::doUserCmd(int i)
{
  //	disableWatch();
  emit aboutStart();
  m_master->setMode(i + 100);
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

//-----------------------------------------------------------------------------
/**
 * Find object by Address list and return object list 
 */
QList<AnBoard*> AnRoot::find(const QList<AnAddress> &alist)
{
  QList<AnBoard*> blist;
  foreach(AnAddress lad, alist)
    blist << dynamic_cast<AnBoard*>(find(lad));

  return blist;
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
      //for(int i = 1; i <= 5; ++i) lst2 << AnAddress(a1, i, a3, a4);
      //for(int i = 1; i <= 4; ++i) lst2 << AnAddress(a1, i, a3, a4);
      for(int i = 1; i <= m_numThub; ++i) lst2 << AnAddress(a1, i, a3, a4);
    else if (a1 == 2 && a2 == 255)
      //for(int i = 1; i <= 125; ++i) lst2 << AnAddress(a1, i, a3, a4);
      //for(int i = 1; i <= 122; ++i) lst2 << AnAddress(a1, i, a3, a4);
      for(int i = 1; i <= m_numTcpu; ++i) lst2 << AnAddress(a1, i, a3, a4);
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

//-----------------------------------------------------------------------------
AnCanObject *AnRoot::hfind(const AnAddress &had)
{
  //	qDebug () << "AnRoot::hfind" << had;
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

//-----------------------------------------------------------------------------
QStringList AnRoot::deviceNames() const
{
  QStringList st;
  foreach(AnCanNet a, m_hnet) st << a.name();
  return st;
}


//==============================================================================
// Public Slots


/**
 * Enable all socket watches
 */
void AnRoot::enableWatch()
{
  mtex_watch.lock();
  //	qDebug() << "AnRoot::enableWatch";
  foreach (QSocketNotifier *sn, m_watch) {
    sn->setEnabled(true);
  }
  mtex_watch.unlock();
}

void AnRoot::enableWatch(int id)
{
  mtex_watch.lock();
  //	qDebug() << "AnRoot::enableWatch" << id;
  m_watch[agentById(id)->socket()]->setEnabled(true);	
  mtex_watch.unlock();
}

/**
 * Disable all socket watches
 */
void AnRoot::disableWatch()
{
  mtex_watch.lock();
  //	qDebug() << "AnRoot::disableWatch";
  foreach (QSocketNotifier *sn, m_watch) {
    sn->setEnabled(false);
  }
  mtex_watch.unlock();
}

void AnRoot::disableWatch(int id)
{
  mtex_watch.lock();
  //	qDebug() << "AnRoot::disableWatch" << id;
  m_watch[agentById(id)->socket()]->setEnabled(false);	
  mtex_watch.unlock();
}

void AnRoot::watchStatus()
{
  qDebug() << "AnRoot::watchStatus";
  foreach (QSocketNotifier *sn, m_watch) {
    qDebug() << "isEnabled" << sn->isEnabled() << "socket" << sn->socket();
  }
	
}

/**
 * QSocketNotifier's activated signal triggers this function.
 */
void AnRoot::watcher(int sock)
{
  //log(QString("AnRoot::watcher: sock=%1").arg(sock));

  if (m_watch.contains(sock)) {
    // disable watch again
    m_watch[sock]->setEnabled(false);

    AnAgent *ag = m_socket_agent_map[sock];
    struct can_frame rmsg;
    try {
      ag->read(rmsg, -1);
      //JS: temporarily, to prevent multiple recovery actions:
      if (!isDoingRecovery())
	received(AnRdMsg(ag->devid(), rmsg));
    } catch (AnExCanError ex) {
      log(QString("AnRoot::watcher[%1]: CAN Communication Error Occurred: 0x%2")
	  .arg(sock).arg(ex.status(),0,16));
    }
    // enable watch again
    m_watch[sock]->setEnabled(true);
  }
}

//-----------------------------------------------------------------------------
void AnRoot::received(AnRdMsg rmsg)
{
  //log( QString("received: %1").arg(rmsg.toString()) );
  
  if (rmsg.type() == 0x80) return; // work around segmentation fault
  
  if ((rmsg.payload() == 0x7) && (rmsg.data() == 0xff550000)) {
    log( QString("received: %1").arg(rmsg.toString()) );
    log("received: CAN recovery message");
    doingRecovery(true);
    doUserCmd(3);
  }

 // AnBoard *brd = dynamic_cast<AnBoard*>(hfind(rmsg.source()));
  
  // if (brd != NULL) {
  //   if (rmsg.payload() == 0x7) {
  //     if (rmsg.data() == 0xff000000) { // greeting message
  // 	if(!isRunning() && (AnRoot::autorepair() == 1)) {
  // 	  brd->clearCommError();
  // 	  AnTdig *tdig = dynamic_cast<AnTdig*>( brd );
  // 	  if (tdig) {
  // 	    log( QString("%1: Running config(1)").arg(tdig->name()));
  // 	    tdig->config(1); // just writes the threshold
  // 	  }
  // 	  brd->sync(1);
  // 	  emit updated(brd);
  // 	}
  //     } 
  //     else if (rmsg.data() == 0xff550000) { // recovery message
  // 	log("received: CAN recovery message");
  // 	doUserCmd(3);
  //     } 
  //     else {
  //     	log( QString("received: CAN alert message from %1").arg(brd->name()) );
  //     	brd->sync(3);
  //     }
  //   } else if (rmsg.payload() == 0x4) {
  //     if(!isRunning()) {
  // 	brd->sync(1);
  // 	emit updated(brd);
  //     }
  //   }
  // }
  // else {
  //   log("received: unknown source");
  // }
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

//-----------------------------------------------------------------------------
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
  qDebug("AnRoot::initAutoSync");
  m_timer = new QTimer(this);
  //m_timer = new QTimer(0);
  m_autoSyncList = list();
  m_autoSyncIter = m_autoSyncList.constBegin();
  connect(m_timer, SIGNAL(timeout()), this, SLOT(autosync()));
  m_timer->setInterval(2000);
  //	m_timer->start();
}

/**
 * Start AutoSync
 */
void AnRoot::startAutoSync()
{
  qDebug("AnRoot::startAutoSync");
  m_timer->start();
}

/**
 * Stop AutoSync
 */
void AnRoot::stopAutoSync()
{
  qDebug("AnRoot::stopAutoSync");
  m_timer->stop();
}

/**
 * AutoSync main function
 */
void AnRoot::autosync()
{
  AnCanObject *ab = dynamic_cast<AnCanObject*>(*m_autoSyncIter);
  qDebug() << "AnRoot::autoSync;"
	   << "isRunning" << isRunning()
	   << "isDoingRecovery" << isDoingRecovery()
	   << "lAddress" << ab->lAddress().toString();

  if ((!isRunning()) && (!isDoingRecovery())) {
    disableWatch();
    (*m_autoSyncIter)->sync(2);
    AnTcpu *tcpu = dynamic_cast<AnTcpu*>(*m_autoSyncIter);
    if (tcpu) {
      if (tcpu->status() >= AnBoard::STATUS_ERROR) log(tcpu->errorDump());
    }
    enableWatch();
    emit updated(*m_autoSyncIter);
    ++m_autoSyncIter;
    if (m_autoSyncIter == m_autoSyncList.constEnd())
      m_autoSyncIter = m_autoSyncList.constBegin();
  }
}

void AnRoot::log(QString str)
{
  mtex_log.lock();
  m_log->log(str);
  mtex_log.unlock();
}

void AnRoot::tlog(QString str, int type, int tray, int board, double val)
{
  mtex_tlog.lock();
  m_tlog->log(str);
#ifdef WITH_EPICS
  if (type > 0)
    m_epicsLogger->q_elog(type,tray,board,val);
#else
  Q_UNUSED(type);
  Q_UNUSED(tray);
  Q_UNUSED(board);
  Q_UNUSED(val);
#endif
  mtex_tlog.unlock();
}


void AnRoot::handler(int sigNum)
{
  Q_UNUSED(sigNum);
  qApp->quit();
} 

