#ifndef ECG_H_
#define ECG_H_

#include "serial_slave.h"
#include "common.h"

void ecg_initialize(void);
void ecg_interrupt_handler(void);

#endif // __ECG_H

