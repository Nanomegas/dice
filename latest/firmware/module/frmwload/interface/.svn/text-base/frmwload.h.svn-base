#ifndef _FRMW_LOAD_H_
#define	_FRMW_LOAD_H_

#ifdef _FRMWLOAD

#include "TCTypes.h"
#include "ErrorCodes.h"

typedef HRESULT (*FRMW_CALLBACK)(void);

HRESULT		frmwLoadResumeTasks(void);
HRESULT		frmwLoadSuspendTasks(void);

HRESULT		frmwLoadInitialize(void);
HRESULT		frmwLoadInitializeTasks(void);

HRESULT		frmwLoad_RESET(void);
HRESULT		frmwLoadStartBoot(void);
HRESULT		frmwLoadSetResetCB(FRMW_CALLBACK resetCB);

HRESULT		hostTestRequest(void);

#endif//_FRMWLOAD
#endif//_FRMW_LOAD_H_
