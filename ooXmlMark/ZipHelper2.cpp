#include "ZipHelper2.h"
#include "Util/debug_printf.h"

#include <boost/filesystem.hpp>

#ifdef _WIN32
#define USEWIN32IOAPI
#include "Minizip/iowin32.h"
#endif

namespace fs = boost::filesystem;

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string& str)
{
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}


void replaceAll(std::string& s, const std::string& search, const std::string& replace) {
	for (size_t pos = 0;; pos += replace.length()) {
		// Locate the substring to replace
		pos = s.find(search, pos);
		if (pos == std::string::npos) break;
		// Replace by erasing and inserting
		s.erase(pos, search.length());
		s.insert(pos, replace);
	}
}

bool dir_recursion(std::string src, int srcLength, std::map<std::string, std::string>& files) {

	WIN32_FIND_DATA findFileData;

	std::wstring inputPath = utf8_decode(src);
	auto pos = inputPath.at(inputPath.size() - 1);
	if (pos != L'\\' && pos != L'/') {
		inputPath.append(L"/");
	}

	HANDLE hFind = ::FindFirstFile((LPCTSTR)(inputPath + L"*").data(), &findFileData);
	size_t file_name_len = 0;
	if (hFind == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {

		while (FindNextFile(hFind, &findFileData)) {

			std::wstring tempPath = inputPath;
			if (!wcscmp((const wchar_t *)findFileData.cFileName, L".") ||
				!wcscmp((const wchar_t *)findFileData.cFileName, L".."))
				continue;

			tempPath.append((const wchar_t *)findFileData.cFileName);
			std::string path_utf8 = utf8_encode(tempPath);
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				dir_recursion(path_utf8, srcLength, files);
			}
			else {
				// AddFile(path_utf8, path_utf8 + strlen(temp_dir2.c_str()));
				files[path_utf8] = path_utf8.substr(srcLength);
			}
		}
		FindClose(hFind);
	}
	return true;
}


/* name of file to get info on */
/* return value: access, modific. and creation times */
/* dostime */
uLong filetime2(const char* f, tm_zip* tmzip, uLong* dt)
{
	int ret = 0;
	{
		FILETIME ftLocal;
		HANDLE hFind;
		WIN32_FIND_DATA ff32;

		std::wstring unicode = utf8_decode(f);

		hFind = FindFirstFile((LPCTSTR)unicode.data(), &ff32);
		if (hFind != INVALID_HANDLE_VALUE){
			FileTimeToLocalFileTime(&(ff32.ftLastWriteTime), &ftLocal);
			FileTimeToDosDateTime(&ftLocal, ((LPWORD)dt) + 1, ((LPWORD)dt) + 0);
			FindClose(hFind);
			ret = 1;
		}
	}
	return ret;
}

ZipHelper2::ZipHelper2() {

}

ZipHelper2::~ZipHelper2() {

}

int ZipHelper2::UnZipFile(std::string Src, std::string Dest)
{
	unzFile uf;

	// change dir context
	// _chdir(Dest.data());

#ifdef _WIN32
	zlib_filefunc64_def ffunc;
	fill_win32_filefunc64A(&ffunc);
	uf = unzOpen2_64(Src.data(), &ffunc);
#else
	uf = unzOpen2(Src.data());
#endif // _WIN32

	if (uf == NULL) {
		throw std::runtime_error("can't open zip file, unzOpen failed.");
		return -1;
	}

	unz_global_info64 gi;
	int ret = unzGetGlobalInfo64(uf, &gi);
	if (ret != UNZ_OK) {
		throw std::runtime_error("enum zip file struct error, unzGetGlobalInfo failed.");
		return -1;
	}


	// lambda extract file
	auto do_extract = [&uf, Dest]() {

		char	filename_fullpath[256];
		// char*	filename_nopath;
		int		err = UNZ_OK;
		char	buf[WRITE_BUFFER_SIZE];
		//void* buf = new unsigned char[WRITE_BUFFER_SIZE];
		//memset(buf, 0x0, WRITE_BUFFER_SIZE);

		unz_file_info64 file_info;
		uLong			ratio = 0;

		err = unzGetCurrentFileInfo64(uf, &file_info,
			filename_fullpath,
			sizeof(filename_fullpath),
			NULL, 0, NULL, 0);

		if (UNZ_OK != err){
			printfTrace("[error] do_extract %d with zipfile in unzGetCurrentFileInfo\n", err);
			return err;
		}

		// 获取文件名
		//char *p = filename_nopath = filename_fullpath;
		//while ((*p) != '\0')
		//{
		//	if (((*p) == '/') || ((*p) == '\\'))
		//		filename_nopath = p + 1;
		//	p++;
		//}
		//if ((*filename_nopath) == '\0') {
		//	// 此处是一个目录，创建目录
		//	err = _mkdir(filename_fullpath);
		//}

		fs::path fullPath = fs::path(Dest) / filename_fullpath;
		fs::path parentPath = fullPath.parent_path();
		if (!(parentPath == "") && !fs::exists(parentPath)){
			fs::create_directories(fullPath.parent_path());
		}

		//err = unzOpenCurrentFilePassword(uf, password);        
		err = unzOpenCurrentFile(uf);
		if (err != UNZ_OK) {
			printfTrace("error %d with zipfile in unzOpenCurrentFilePassword\n", err);
			return err
				;
		}

		//FILE* fExists = NULL;
		//err = fopen_s(&fExists, filename_fullpath, "rb");
		//if (err == 0 && fExists != NULL) {
		//	// 运行此处表示文件已存在
		//	fclose(fExists);
		//}

		FILE* fOut = NULL;
		err = fopen_s(&fOut, fullPath.string().data(), "wb");
		if (err != 0) {
			printfTrace("error opening %s\n", fullPath.string().data());
		}
		else {

			if (!fOut) {
				printfTrace("error %d with zipfile in unzReadCurrentFile\n", err);
				return -1;
			}
			// 内容写入文件
			do {
				err = unzReadCurrentFile(uf, buf, WRITE_BUFFER_SIZE);
				if (err < 0) {
					printfTrace("error %d with zipfile in unzReadCurrentFile\n", err);
					break;
				}
				else {
					fwrite(buf, err, 1, fOut);
				}
			} while (err > 0);
			fclose(fOut);
		}

		err = unzCloseCurrentFile(uf);
		if (err != UNZ_OK) {
			printfTrace("error %d with zipfile in unzCloseCurrentFile\n", err);
		}
		// delete buf;
		return 0;
	};

	for (int i = 0; i < gi.number_entry; i++) {
		if (do_extract() != UNZ_OK) {
			break;
		}

		if (gi.number_entry > i + 1) {
			ret = unzGoToNextFile(uf);
			if (UNZ_OK != ret)
			{
				printfTrace("[error] %d with zipfile in unzGoToNextFile\n", ret);
				break;
			}
		}
	}
	return 0;
}


int ZipHelper2::ZipFile(std::string Dir, std::string Dest) {


	//void* buf = NULL;
	//void* buf = new unsigned char[WRITE_BUFFER_SIZE];
	//memset(buf, 0x0, WRITE_BUFFER_SIZE);
	//if (!buf) {
	//	return -1;
	//}
	char buf[WRITE_BUFFER_SIZE];
	std::map<std::string, std::string> files_;
	dir_recursion(Dir, Dir.length() + 1, files_);


	int err = 0;
	zlib_filefunc64_def ffunc;
	fill_win32_filefunc64A(&ffunc);

	unzFile zf = zipOpen2_64(Dest.data(), APPEND_STATUS_CREATE, NULL, &ffunc);
	if (zf == NULL) {
		std::cout << "can't open " << Dest << " zip file " << std::endl;
		return -1;
	}

	for (auto ite = files_.begin(); ite != files_.end(); ++ite) {


		std::string fileinzip = ite->second;
		std::string zipfile = ite->first;
		int opt_compress_level = Z_DEFAULT_COMPRESSION;
		zip_fileinfo	zi;
		unsigned long	crcFile = 0;
		int				zip64 = 0;
		int				size_read = 0;

		zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
			zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
		zi.dosDate = 0;
		zi.internal_fa = 0;
		zi.external_fa = 0;
		filetime2(zipfile.data(), &zi.tmz_date, &zi.dosDate);

		err = zipOpenNewFileInZip3_64(zf, fileinzip.data(), &zi,
			NULL, 0, NULL, 0, NULL,
			(opt_compress_level != 0) ? Z_DEFLATED : 0,
			opt_compress_level, 0,
			-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
			NULL, crcFile, zip64);
		if (err != ZIP_OK) {
			printfTrace("[error] XXX in opening %s in zipfile\n", Dest.data());
			return -1;
		}

		FILE* file = NULL;
		fopen_s(&file, zipfile.data(), "rb");
		if (file == NULL) {
			err = ZIP_ERRNO;
			printfTrace("[error] XXX in opening %s for reading\n", Dest.data());
			return -1;
		}

		do {
			err = ZIP_OK;
			size_read = (int)fread(buf, 1, WRITE_BUFFER_SIZE, file);
			if (size_read < WRITE_BUFFER_SIZE) {
				if (feof(file) == 0) {
					printfTrace("[error] XXX in reading %s\n", Dest.data());
					err = ZIP_ERRNO;
				}
			}
			if (size_read > 0) {
				err = zipWriteInFileInZip(zf, buf, size_read);
				if (err < 0) {
					printfTrace("[error] XXX in writing %s in the zipfile\n", Dest.data());
				}
			}
		} while ((err == ZIP_OK) && (size_read > 0));

		if (file) {
			fclose(file);
		}

		if (err < 0) {
			err = ZIP_ERRNO;
		}
		else {
			err = zipCloseFileInZip(zf);
			if (err != ZIP_OK) {
				printfTrace("error in closing %s in the zipfile\n", Dest.data());
			}
		}
	}

	int errclose = zipClose(zf, NULL);
	if (errclose != ZIP_OK) {
		printfTrace("error in closing %s\n", Dest.data());
		return -1;
	}

	return 0;
}
