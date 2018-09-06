/*
 * DrvTW835GPIO.h
 *
 *  Created on: 2013-6-17
 *      Author: Administrator
 */

#ifndef DRVTW8835GPIO_H_
#define DRVTW835GPIO_H_

#include <Driver.h>

class DrvTW8835GPIO : public Driver {
public:
	DrvTW8835GPIO(char port_pin,bool activeLevel=true);
	virtual ~DrvTW8835GPIO();
	int init(void);
	void on(void);
	void off(void);
	void setActiveLevel(bool l);
	bool getValue(void);
private:
	char _port_pin;
	bool _activeLevel;
};

#endif /* DRVTW8835GPIO_H_ */
