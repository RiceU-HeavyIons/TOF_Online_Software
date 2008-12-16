/*
 * $Id$
 *
 *  Created on: Dec 13, 2008
 *      Author: koheik
 */
#ifndef AnMaster_H_
#define AnMaster_H_ 1
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QQueue>

class AnRoot;

class AnMaster : public QThread
{
	Q_OBJECT
public:
	AnMaster(AnRoot *root, QObject *parent = 0);
	virtual ~AnMaster();

public slots:
	void setMode(int i);

protected:
	virtual void run();

private:
	AnRoot     *m_root;
	QQueue<int> m_q;
	QMutex      m_mutex;
	
	void p_setMode(int i);
};
#endif