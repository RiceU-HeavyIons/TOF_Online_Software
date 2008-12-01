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

class AnBoard;

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
	static QMap<int, AnAgent*> open(QList<int> &dev_id_list);

	int open(quint8 dev_id);
	quint64 write_read(TPCANMsg &msg, TPCANRdMsg &rmsg,
	      unsigned int return_length, unsigned int time_out = 4000000);

	int setId(int id) { return (m_id = id); }
	void init(int mode, QList<AnBoard*> list);
	void setTdcConfigs(const QMap<int, AnTdcConfig*>& tcnfs);

	quint8 devid() const { return addr; }

	AnTdcConfig *tdcConfig(int i) const { return m_tcnfs[i]; }

public slots:
	void stop() { m_cancel = true; }

protected:
	virtual void run();

private:
	int                      m_mode;
	QList<AnBoard*>          m_list;
	int                      m_id;
	bool                     m_cancel;
	QMap<int, AnTdcConfig*>  m_tcnfs; // thread local copy of tdc configs

	HANDLE                   m_handle;
	quint8                   addr;
	QString                  dev_path;

signals:
	void progress(int id, int percent);
};
#endif
