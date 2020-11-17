/***************************************************************************
 *   Copyright (C) 2020 by Liu Pan		                                   *
 *   pliu@zshield.net                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _PDF_QR_COMMON_H_
#define _PDF_QR_COMMON_H_

#include "QRManagerDefines.h"
#include <iostream>


class QRMANAGER_API QRCommon
{
public:
	QRCommon(void);
	~QRCommon(void);

public:
	static void* QRMalloc(int size);

	static void QRFree(void* pVoid);
};

#endif
