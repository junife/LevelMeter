#ifndef APPL_H_
#define APPL_H_

#ifdef  APPL_GLOBALS
#define APPL_EXT
#else
#define APPL_EXT  extern
#endif

APPL_EXT void McuInit(void);
APPL_EXT void ApplInit(void);
APPL_EXT void AppCycleUpdate(void);
#endif	/* endif APPL_H_ */
