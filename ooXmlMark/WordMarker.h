#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include <QRManager/QRReader.h>
#include <QRManager/QRGenerator.h>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "ZipHelper2.h"
#include "XmlOperation.h"
#include "Util/Jinja2Light.h"

namespace fs = boost::filesystem;

class WordMarker{

public:
	WordMarker(std::string wordPath);
	WordMarker(std::string wordPath, std::string wordTempPath);
	~WordMarker();

	bool FindWater();

	bool WaterMarkGenerate(std::string message);
	// bool WaterMarkGenerate(std::string message, std::string outputPath);

	bool WaterMarkUpdate(std::string message);
	// bool WaterMarkUpdate(std::string message, std::string outputPath);

	void CheckHeader();
	std::string GetMaxRefId();

	void SaveToFile(std::string outputPath);

	bool isHeader(){ return m_bHasHeader; }
	bool isMarked(){ return m_bMarked; }


private:
	fs::path	m_DocWordRelPath;
	XMLOperation *m_DocWordRelation;

	std::vector<std::string> m_vecRefId;
	std::vector<fs::path> m_vecHdrs;

	std::string m_strWordFilePath = "";
	std::string m_strWordTempPath = "";

	// 涓€涓枃妗ｉ噷鍙兘瀛樺湪澶氫釜header鐨勬儏鍐碉紝demo鏆傛椂涓嶈€冭檻
	// 鍋囧畾鍙湁涓€涓猦eader鏂囦欢琚紩鐢?
	// 浣嗘槸杩欓噷浼氬奖鍝嶆按鍗版洿鏂板繀椤诲厛璇存槑
	std::string m_strHeaderId = "";
	std::string m_strHeaderTarget = "";
	std::string m_strWaterMarkFile = "";

	std::string m_maxRefId = "";
	
	bool m_bHasHeader = false;
	bool m_bMarked = false;
	bool m_bUnzipSuccess = false;

	const fs::path wordTemplates = fs::current_path() / "templates" / "word";
};