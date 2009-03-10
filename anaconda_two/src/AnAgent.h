/*
 * $Id$
 *
 *  Created on: Nov 29, 2008
 *      Author: koheik
 */

#ifndef AnAgent_H_
#define AnAgent_H_
#include <fcntl.h>
#include <glob.h>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QDebug>

#include "libpcan.h"

#include "AnTdcConfig.h"
#include "AnRdMsg.h"

#define AGENT_COMM_ERROR_THRESHOLD     4

class AnBoard;
class AnRoot;

class AnAgent : public QThread {
	Q_OBJECT

public:
	AnAgent(QObject *parent = 0);
	~AnAgent();

	static int TCAN_DEBUG;
	static const char *PCAN_DEVICE_PATTERN;

	static void set_msg(TPCANMsg &msg, ...);
	static void print(const TPCANMsg &msg);
	static void print(const TPCANRdMsg &rmsg);
	static QMap<int, AnAgent*> open(QMap<int, int> &devid_map);
	static int debug() { return TCAN_DEBUG; }
	static int setDebug(int d) { return (TCAN_DEBUG = d); }

	int open(quint8 dev_id);
	quint64 read(TPCANRdMsg &rmsg,
	            int return_length, unsigned int time_out = 5000000);
	void raw_write(TPCANMsg &msg, int time_out = 5000000);
	quint64 write_read(TPCANMsg &msg, TPCANRdMsg &rmsg,
	      unsigned int return_length, unsigned int time_out = 5000000);

	int id() const { return m_id; }
	int setId(int id) { return (m_id = id); }
	void init(int mode, int level, QList<AnBoard*> list);
	void setTdcConfigs(const QMap<int, AnTdcConfig*>& tcnfs);

	quint8 devid() const { return addr; }
	int socket() const { return LINUX_CAN_FileHandle(m_handle); }

	QString deviceName() const { return m_deviceName; }
	QString setDeviceName(const QString& nm) { return (m_deviceName = nm); }

	AnTdcConfig *tdcConfig(int i) const { return m_tcnfs[i]; }
	
	AnRoot *setRoot(AnRoot *root) { return (m_root = root); }

	int commError() const { return m_comm_err; }
	int setCommError(int ce) { return (m_comm_err = ce); }
	int incCommError() { return ++m_comm_err; }
	int decCommError() { return (m_comm_err > 0) ? --m_comm_err : 0; }
	void clearCommError() { m_comm_err = 0; }

public slots:
	void stop() { m_cancel = true; }

signals:
	void init(int id);
	void progress(int id, int percent);
	void finished(int id);

	void received(AnRdMsg);
	void debug_recv(AnRdMsg);
	void debug_send(AnRdMsg);

protected:
	virtual void run();

private:
	void print_send(const TPCANMsg &msg);
	void print_recv(const TPCANMsg &msg);
	bool match(TPCANMsg &snd, TPCANMsg &rcv);
	void error_handle(int er);
	
	void pre_check();

	int                      m_mode;
	int                      m_level;
	QList<AnBoard*>          m_list;

	int                      m_id;
	bool                     m_cancel;
	QMap<int, AnTdcConfig*>  m_tcnfs; // thread local copy of tdc configs

	HANDLE                   m_handle;
	quint8                   addr;
	QString                  dev_path;

	QString                  m_deviceName;
	AnRoot                  *m_root;

	int                      m_comm_err;

};
#endif
