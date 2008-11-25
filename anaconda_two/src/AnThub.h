/*
 * AnThub.h
 *
 *  Created on: Nov 11, 2008
 *      Author: koheik
 */

#ifndef ANTHUB_H_
#define ANTHUB_H_
#include "AnBoard.h"
#include "AnSerdes.h"

class AnThub : public AnBoard {
public:
	AnThub(const AnAddress &laddr, const AnAddress &haddr, AnCanObject *parent = 0);
	virtual ~AnThub();

//  from AnCanbusObject
	virtual void sync(int level = 0);
	virtual AnCanObject *at(int i) { return (i > 0) ? m_serdes[i-1] : static_cast<AnCanObject*>(this); }
	
	bool isEast() const { return (lAddress().at(1) <= 2); }
	bool isWest() const { return (lAddress().at(1)  > 2); }

	AnSerdes *serdes(int i) const { return m_serdes[i-1]; }

private:
	AnSerdes *m_serdes[8];
};

#endif /* ANTHUB_H_ */
