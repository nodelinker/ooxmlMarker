/***************************************************************************
 *   Copyright (C) 2020 by Liu Pan		                                   *
 *   pliu@zshield.net                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef QR_MANAGER_API_H_20201020
#define QR_MANAGER_API_H_20201020

#if defined(QRMANAGER_EXPORTS)
    #define COMPILING_SHARED_QRMANAGER
    #undef USING_SHARED_QRMANAGER
#else
    #define USING_SHARED_QRMANAGER
    #undef COMPILING_SHARED_QRMANAGER
#endif

#if defined(COMPILING_SHARED_QRMANAGER) && defined(USING_SHARED_QRMANAGER)
    #error "Both COMPILING_SHARED_QRMANAGER and USING_SHARED_QRMANAGER defined!"
#endif

#if defined(_WIN32)
    #if defined(COMPILING_SHARED_QRMANAGER)
        #define QRMANAGER_API __declspec(dllexport)
	#elif defined(USING_SHARED_QRMANAGER)
		#define QRMANAGER_API __declspec(dllimport)
    #else
        #define QRMANAGER_API
    #endif
#else
    // TODO
#endif

#endif

