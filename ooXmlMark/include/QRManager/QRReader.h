/***************************************************************************
 *   Copyright (C) 2020 by Liu Pan		                                   *
 *   pliu@zshield.net                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _PDF_QR_READER_H_
#define _PDF_QR_READER_H_

#include "QRManagerDefines.h"
#include <iostream>


class QRMANAGER_API QRReader
{
public:
	QRReader(void);
	~QRReader(void);

public:
	/** 解析二维码
	 * \param szInputFile: 二维码图像文件路径（PNG图像）
	 * \param szOutputString: 解析后的字符串
	 */
	bool ReadQR( const std::string& szInputFile, std::string& szOutputString );

	/** 解析二维码
	 * \param pData: 二维码图像的RGB数据
	 * \param datLen: 二维码图像的RGB数据长度
	 * \param width: 二维码图像的宽度
	 * \param height: 二维码图像的高度
	 * \param szOutputString: 解析后的字符串
	 */
	bool ReadQR( const unsigned char* pRGBData, const int datLen, const int width, const int height, std::string& szOutputString );
};

#endif

