/*
 * AnSerdes.h
 *
 *  Created on: Nov 22, 2008
 *      Author: koheik
 */

#ifndef AnSerdes_H_
#define AnSerdes_H_

#include "AnBoard.h"
#include "AnTdc.h"

class AnSerdes: public AnBoard {
public:
	AnSerdes(const AnAddress& ladrr, const AnAddress& laddr, AnCanObject *parent = 0);
	virtual ~AnSerdes();

	virtual QString firmwareString() const;
	virtual void sync(int level = 0);

	virtual AnCanObject *at(int i) { return (AnCanObject*)NULL; }
	
	virtual QString ecsrString() const;
	virtual bool isEast() const { return dynamic_cast<AnBoard*>(parent())->isEast(); }
	virtual bool isWest() const { return dynamic_cast<AnBoard*>(parent())->isWest(); }
};

#endif /* AnSerdes_H_ */
