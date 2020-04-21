/* Texas A&M University
** Electronics Systems Engineering Technology
** ESET-420 Capstone Design
** Author: Jack Smith & Christian Ledgard
** File: monolith.h
** --------
**
**/
//gpio 23,25,26,27,30,29,28,24
//gpio 23,24,25,26,27,28,29,30

#ifndef MONOLITHTASK_H_
#define MONOLITHTASK_H_

void MonolithTask(UArg arg0, UArg arg1);
double processReading(uint16_t ndx);

#endif /* MONOLITHTASK_H_ */
