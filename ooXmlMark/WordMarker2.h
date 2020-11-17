

#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include <QRManager/QRCommon.h>
#include <QRManager/QRReader.h>
#include <QRManager/QRGenerator.h>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "ZipHelper2.h"
#include "XmlOperation.h"
#include "Util/Jinja2Light.h"

namespace fs = boost::filesystem;

class WordMarker2{

public:
	WordMarker2(std::string wordPath);
	WordMarker2(std::string wordPath, std::string wordTempPath);
	~WordMarker2();

	bool FindWaterMark();

	bool WaterMarkGenerate(std::string message);
	bool WaterMarkUpdate(std::string message);

	void GetRelationFileMaxId(std::string relFilePath, std::string &maxID);

	std::string WordHiddenImageTemplte(std::string rId);

	void SaveToFile(std::string outputPath);

	bool readMark(std::string &message);
	std::string readMark();
 

	bool isMarked(){ return m_bMarked; }


private:

	std::string m_strWordFilePath = "";
	std::string m_strWordTempPath = "";

	std::string m_strHeaderTarget = "";
	std::string m_strWaterMarkFile = "";

	bool m_bHasHeader = false;
	bool m_bMarked = false;
	bool m_bUnzipSuccess = false;

};