/***************************************************************************
 *   Copyright (C) 2020 by Liu Pan		                                   *
 *   pliu@zshield.net                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _PDF_QR_BASE64_H_
#define _PDF_QR_BASE64_H_

#include "QRManagerDefines.h"

typedef unsigned char     uint8;
typedef unsigned long    uint32;

class QRMANAGER_API QRBase64
{
public:
	QRBase64(void);
	~QRBase64(void);

public:
	/** Base64编码
	 * \param text: 需要编码的文本
	 * \param text_len: 需要编码的文本的长度
	 * \param encode: 编码后的字符串。可为 NULL，只返回编码后的长度
	 * 
	 * \return : 编码后的长度
	 */
	static uint32 Encode(const uint8 *text, uint32 text_len, uint8 *encode);

	/** Base64解码
	 * \param code: 需要解码的字符串
	 * \param code_len: 需要解码的字符串长度
	 * \param plain: 解码后的字符串。可为 NULL，只返回解码后的长度
	 * 
	 * \return : 解码后的长度
	 */
	static uint32 Decode(const uint8 *code, uint32 code_len, uint8 *plain);
};

#endif

