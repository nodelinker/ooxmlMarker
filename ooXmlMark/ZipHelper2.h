
#pragma once
#include <iostream>
#include <cstdio>
#include <string>
#include <filesystem>
#include <map>

#include <direct.h>

#include "Minizip/unzip.h"
#include "Minizip/zip.h"


#define CASE_SENSITIVITY (0)
#define WRITE_BUFFER_SIZE (8192)
#define MAX_FILE_NAME (256)


class ZipHelper2
{

public:

	ZipHelper2();
	~ZipHelper2();

	// 文件解压
	int UnZipFile(std::string Src, std::string Dest);

	// 文件压缩，office opc
	int ZipFile(std::string Src, std::string Dest);

private:

};

 