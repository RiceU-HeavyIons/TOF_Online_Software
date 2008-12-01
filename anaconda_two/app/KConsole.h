// $Id$
// Kohei Kajimoto, November 28, 2008
#ifndef KConsole_H_
#define KConsole_H_

#include <QtCore/QString>
#include <QtCore/QTime>

#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>
#include <QtGui/QTextEdit>

#include "AnRoot.h"


class KConsole : public QDialog
{
	Q_OBJECT

public:
	KConsole(AnRoot *root, QWidget *parnt = 0);

public slots:
	void textChanged(const QString &text);
	void clicked(bool b);

private:
	QFont        m_font;

	QPushButton *m_button;
	QPushButton *m_clearb;
	QLineEdit   *m_ledit;
	QTextEdit   *m_tedit;
	QString      m_text;
	
	AnRoot      *m_root;
	
	QTime        m_start;
	QTime        m_stop;

	void cmd_sync  (const QStringList &cmdl);
	void cmd_reset (const QStringList &cmdl);
	void cmd_config(const QStringList &cmdl);

	void cmd_show  (const QStringList &cmdl);
	void cmd_read  (const QStringList &cmdl);
	void cmd_write (const QStringList &cmdl);
	void cmd_expand(const QStringList &cmdl);

	void cmd_sql   (const QStringList &cmdl);
	void cmd_about (const QStringList &cmdl);

	int  parse_int(QString str) const;
	void tm_start();
	void tm_stop();
};
#endif /* KConsole_H_ */
