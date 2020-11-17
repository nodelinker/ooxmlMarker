/***************************************************************************
 *   Copyright (C) 2020 by Liu Pan		                                   *
 *   pliu@zshield.net                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _PDF_QR_GENERATOR_H_
#define _PDF_QR_GENERATOR_H_

#include "QRManagerDefines.h"
#include <iostream>


class QRMANAGER_API QRGenerator
{
public:
	QRGenerator(void);
	~QRGenerator(void);

public:
	/** 生成二维码图像
	 * \param szInputString: 二维码包含的字符串内容
	 * \param ppRBGData: 生成的二维码图像的RGB数据
	 * \param pLenData: 生成的二维码图像的RGB数据长度
	 * \param pWidth: 生成的二维码图像的宽度
	 * \param pHeight: 生成的二维码图像的高度
	 * \param bIsRGBorRGBA: true 为 RGB，false 为 RGBA
	 */
	bool GeneratorQR( const std::string& szInputString, unsigned char **ppRBGData, long* pLenData, int* pWidth, int* pHeight, bool bIsRGBorRGBA = true);

	/** 生成二维码图像
	 * \param pszInputString: 二维码包含的字符串内容
	 * \param nStrSize: pszInputString的大小
	 * \param ppRBGData: 生成的二维码图像的RGB数据
	 * \param pLenData: 生成的二维码图像的RGB数据长度
	 * \param pWidth: 生成的二维码图像的宽度
	 * \param pHeight: 生成的二维码图像的高度
	 * \param bIsRGBorRGBA: true 为 RGB，false 为 RGBA
	 */
	bool GeneratorQR( const char* pszInputString, int nStrSize, unsigned char **ppRBGData, long* pLenData, int* pWidth, int* pHeight, bool bIsRGBorRGBA = true);

	/** 生成二维码图像（PNG）
	 * \param szInputString: 二维码包含的字符串内容
	 * \param szOutputImageName: 生成的二维码图像的路径
	 * \param ucTransp: PNG图像的透明度
	 */
	bool GeneratorQR( const std::string& szInputString, const std::string& szOutputImageName, const unsigned char ucTransp = 255 );

	/** 生成二维码图像（PNG）
	 * \param pszInputString: 二维码包含的字符串内容
	 * \param nStrSize: pszInputString的大小
	 * \param pszOutputImageName: 生成的二维码图像的路径
	 * \param ucTransp: PNG图像的透明度
	 */
	bool GeneratorQR( const char* pszInputString, int nStrSize, const char* pszOutputImageName, const unsigned char ucTransp = 255 );
};

#endif


