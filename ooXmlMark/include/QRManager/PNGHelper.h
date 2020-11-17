/***************************************************************************
 *   Copyright (C) 2020 by Liu Pan		                                   *
 *   pliu@zshield.net                                                      *
 *                                                                         *
 ***************************************************************************/

#ifndef _PDF_PNG_HELPER_H_
#define _PDF_PNG_HELPER_H_

#include "QRManagerDefines.h"
#include <png.h>
#include <iostream>


struct QRMANAGER_API pngReadDataStr
{
    pngReadDataStr(const unsigned char* data, png_size_t size):_data(data), _pos(0), _size(size) {}
    
    void read(png_bytep data, png_size_t length)
    {
        if (length > _size - _pos)
        {
            memcpy(data, &_data[_pos], _size - _pos);
            _pos = _size;
        }
        else
        {
            memcpy(data, &_data[_pos], length);
            _pos += length;
        }
    }
    
    private:
    const unsigned char* _data;
    png_size_t _pos;
    png_size_t _size;
};


struct QRMANAGER_API pngWriteDataStr
{
    pngWriteDataStr(unsigned char* data, png_size_t size):_data(data), _pos(0), _size(size) {}
    
    void write(png_bytep data, png_size_t length)
    {
        if (length > _size - _pos)
        {
            if (_size - _pos > 0) 
            {
                memcpy((void*)&_data[_pos], data, _size - _pos);
                _pos = _size;
            }
            else
            {
                std::cout << "png write error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            }
            
        }
        else
        {
            memcpy((void*)&_data[_pos], data, length);
            _pos += length;
        }
    }

    png_size_t get_size() {return _pos;}
    
    private:
    const unsigned char* _data;
    png_size_t _pos;
    png_size_t _size;
};


class QRMANAGER_API PNGHelper
{
public:
	PNGHelper(void);
	~PNGHelper(void);

public:
    /** 根据图像的RGB数据，生成PNG图像文件
     * \param outfile: 生成的PNG图像文件的路径
     * \param pData: PNG图像的RGB数据
     * \param lLen: PNG图像的RGB数据的长度
     * \param width: PNG图像的宽度
     * \param height: PNG图像的高度
     * \param color_type: PNG图像的颜色格式
     */
	static bool WritePNG( const char *outfile, const unsigned char* pData, const long lLen, const int width, const int height, int color_type = PNG_COLOR_TYPE_RGB, int nBitsPerComponent = 8 );

    /** 获取指定PNG图像文件的数据
     * \param name: PNG图像文件的路径
     * \param width: 输出PNG图像的宽度
     * \param height: 输出PNG图像的高度
     * \param raw: 输出PNG图像的行数据
     * \param rawLen: 输出PNG图像的行数据长度
     * \param grey: 是否获取灰度数据
     */
	static bool GetData( const char *name, int *width, int *height, void **raw, long *rawLen, bool grey );

    /** 解析PNG数据，获取RGB数据
     * \param pPngData: PNG图像的数据
     * \param lPngDataLen: PNG图像的数据的长度
     * \param ppRgbData: 解析的PNG图像的RGB数据
     * \param lRgbDataLen: 解析的PNG图像的RGB数据长度
     * \param unWidth: 解析的PNG图像的宽度
     * \param unHeight: 解析的PNG图像的高度
     */
	static bool GetRgbDataFromPngData( const unsigned char* pPngData, const long lPngDataLen, unsigned char** ppRgbData, long& lRgbDataLen, unsigned int& unWidth, unsigned int& unHeight );

    /** 根据RGB数据，生成PNG数据
     * \param pRgbData: 图像的RGB数据
     * \param lRgbDataLen: 图像的RGB数据长度
     * \param unWidth: 图像的宽度
     * \param unHeight: 图像的高度
     * \param ppPngData: 生成的PNG数据
     * \param lPngDataLen: 生成的PNG数据的长度
     * \param color_type: 颜色格式
     */
    static bool GetPngDataFromPiexlData( const unsigned char* pRgbData, 
                                        long lRgbDataLen, unsigned int unWidth, 
                                        unsigned int unHeight, 
                                        int nColorType, 
                                        int nBitsPerComponent, 
                                        char** ppPngData, 
                                        long& lPngDataLen );
};

#endif

