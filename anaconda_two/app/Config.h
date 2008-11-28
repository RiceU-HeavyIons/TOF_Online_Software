/*
 * Config.h
 *
 *  Created on: Nov 7, 2008
 *      Author: koheik
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include <QtCore/QThread>

class Config : public QThread {
  Q_OBJECT
public:
    Config(int i, int ms = 100);
    void run();
private:

  int m_id;
  int m_sleep;

signals:
   void step(int, int);
};

#endif /* CONFIG_H_ */
