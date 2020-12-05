#ifndef STORAGE_H_
#define STORAGE_H_

#include "serial_slave.h"
#include "common.h"

void storage_init(void);
void storage_interrupt_handler(void);

#endif // __STORAGE_H