#include "WordMarker.h"

WordMarker::WordMarker(std::string wordPath):m_strWordFilePath(wordPath){

	fs::path tempDocPath = fs::current_path() / fs::unique_path();
	if (fs::exists(tempDocPath) && fs::is_directory(tempDocPath)){
		DEBUGPRINT("tmp directory exists.");
		return ;
	}

	if (!fs::create_directory(tempDocPath)){
		DEBUGPRINT("tmp directory can't create.");
		return ;
	}

	m_strWordTempPath = (fs::current_path() / fs::unique_path()).string();

	ZipHelper2 zh;
	int ret = zh.UnZipFile(m_strWordFilePath, m_strWordTempPath);
	if (ret == 0){
		m_bUnzipSuccess = true;
	}

	this->CheckHeader();
	this->FindWater();
}

WordMarker::WordMarker(std::string wordPath, std::string wordTempPath) : 
m_strWordFilePath(wordPath),m_strWordTempPath(wordTempPath){
	
	ZipHelper2 zh;
	int ret = zh.UnZipFile(m_strWordFilePath, m_strWordTempPath);
	if (ret == 0){
		m_bUnzipSuccess = true;
	}

	this->CheckHeader();
	this->FindWater();
}

void WordMarker::CheckHeader(){

	// step 1 ����Ƿ����header�����ĵ���relation�ļ��С�
	// C:\\Users\\xszhang\\Desktop\\����ˮӡ\\sample\\docx\\a\\demo1\\word\\_rels\\document.xml.rels
	m_DocWordRelPath = fs::path(m_strWordTempPath) / "word/_rels/document.xml.rels";
	m_DocWordRelation = new XMLOperation(m_DocWordRelPath.string().data());
	xmlNodePtr root = m_DocWordRelation->xmlGetRootNode();

	// mark header refer
	//std::regex pattern("schemas\.openxmlformats\.org/officeDocument/(\d+)/relationships/header$");
	std::regex pattern(R"((http|https)://schemas\.openxmlformats\.org/officeDocument/(\d+)/relationships/header$)");

	for (xmlNodePtr tmp = root->children; tmp != NULL; tmp = tmp->next) {

		xmlChar* rIdProp = xmlGetProp(tmp, BAD_CAST "Id");
		xmlChar* typeProp = xmlGetProp(tmp, BAD_CAST "Type");
		xmlChar* targetProp = xmlGetProp(tmp, BAD_CAST "Target");

		if (!rIdProp) {
			continue;
		}

		if (!typeProp) {
			continue;
		}

		m_vecRefId.push_back(std::string((char*)rIdProp));

		std::string data = std::string((char*)typeProp);
		if (std::regex_match(data, pattern)) {
			DEBUGPRINT("found Property Type: %s, ID %s\n", typeProp, rIdProp);
			m_bHasHeader = true;
			m_strHeaderId = (char*)rIdProp;
			m_strHeaderTarget = (char*)targetProp;
		}

		xmlFree(targetProp);
		xmlFree(typeProp);
		xmlFree(rIdProp);
	}

}

std::string WordMarker::GetMaxRefId(){
	std::sort(m_vecRefId.begin(), m_vecRefId.end());
	m_maxRefId = m_vecRefId.back();
	return m_maxRefId;
}

bool WordMarker::FindWater(){

	// Ŀǰˮӡ��Ϣ��ʱ����ͷ����ͷ����ֱ�ӷ���ʧ��
	if (!m_bHasHeader){
		return false;
	}

	// ��ȡheader.xml�ļ���������Ƿ����ˮӡ��Ϣ
	fs::path headerPath = m_strWordTempPath / fs::path("word") / m_strHeaderTarget;
	if (!fs::exists(headerPath)){
		return false;
	}
	std::cout << headerPath.string() << std::endl;
	

	// Todo: ȥheader.xml�ڼ���Ƿ���ڶ�ά������
	xmlXPathObjectPtr xpathObj = NULL;
	auto headerDoc = XMLOperation(headerPath.string().data());

	// registe namespace with xml.
	std::vector<std::string> ns;
	ns.push_back("a=http://schemas.openxmlformats.org/drawingml/2006/main");
	ns.push_back("pic=http://schemas.openxmlformats.org/drawingml/2006/picture");
	headerDoc.xmlXPathRegisterNamespace(ns);

	// ����Ӧ������һ���жϣ����û��headerûˮӡ�����
	// watermark signature tag by desc with "mark"
	int foundSize = headerDoc.xmlXPathFindObjects(
		BAD_CAST "//w:hdr/w:p/w:r/w:drawing/wp:anchor/a:graphic/a:graphicData/pic:pic/pic:nvPicPr/pic:cNvPr[@descr='mark']",
		xpathObj);

	if (foundSize == 0) {
		DEBUGPRINT("[AAA] can't find marked element..");
		return false;
	}

	xmlNodeSetPtr nodes = xpathObj->nodesetval;
	xmlNodePtr firstNode = nodes->nodeTab[0];

	if (firstNode->parent) {
		firstNode = firstNode->parent;
	}

	xmlNodePtr picNode = NULL;
	if (firstNode->parent) {
		picNode = firstNode->parent;
	}

	// pic:nvPicPr
	foundSize = 0;
	xpathObj = NULL;
	foundSize = headerDoc.xmlXPathFindObjects(picNode, BAD_CAST "pic:blipFill/a:blip", xpathObj);
	if (foundSize == 0) {
		DEBUGPRINT("[AAA] can't find pic:blipFill element..");
		return false;
	}

	nodes = xpathObj->nodesetval;
	firstNode = nodes->nodeTab[0];

	xmlChar* markedRelId = xmlGetProp(firstNode, BAD_CAST "embed");

	fs::path headerRelsPath = (fs::path(m_strWordTempPath) / "word/_rels/header1.xml.rels");
	if (!fs::exists(headerRelsPath))
	{
		return false;
	}
	auto headerRelDoc = XMLOperation(headerRelsPath.string().data());

	foundSize = 0;
	xpathObj = NULL;
	firstNode = NULL;


	// ����Ŀӱ�ʶһ��
	// ���xml�����ˣ���������û�����ֵ�namespace��
	// xmlns="http://schemas.openxmlformats.org/package/2006/relationships"
	// �����������һ�����֣��������namespace�н���xpath��ѯ

	boost::format fmt = boost::format("//null:Relationships/null:Relationship[@Id='%s']") % (char*)markedRelId;
	std::string pattern = fmt.str();
	foundSize = headerRelDoc.xmlXPathFindObjects(BAD_CAST pattern.data(), xpathObj);
	if (foundSize == 0) {
		DEBUGPRINT("[AAA] can't find Relationship element id is {}.", markedRelId);
		return false;
	}
	nodes = xpathObj->nodesetval;
	firstNode = nodes->nodeTab[0];

	std::string markedFileTarget = (char *)xmlGetProp(firstNode, BAD_CAST "Target");
	fs::path markedFileAbsPath = fs::path(m_strWordTempPath) / "word" / markedFileTarget;
	std::cout << "Marker file." << markedFileAbsPath << " " << fs::exists(markedFileAbsPath) << std::endl;

	m_strWaterMarkFile = markedFileAbsPath.string();
	m_bMarked = true;
	return true;
}

bool WordMarker::WaterMarkGenerate(std::string message){

	// ���û��header ref�ļ����ã���ʹ��template�����µ�header�����reference.
	std::string maxRefId = this->GetMaxRefId();
	unsigned int maxId = 0;

	std::regex		_repr("rId(\\d+)$");
	std::smatch		_match;
	std::regex_search(maxRefId, _match, _repr);
	if (_match.size() > 1){
		maxId = atoi(_match.str(1));
	}

	// add header rels with template.
	fs::path docHeaderRelPath = fs::path(m_strWordTempPath) / "word/_rels/header1.xml.rels";
	fs::copy_file(wordTemplates / "default-header.xml.rels", docHeaderRelPath);

	// ������ά��ˮӡ
	fs::path signaturePath = fs::path(m_strWordTempPath) / "word/media/image1.png";
	if (!fs::exists(signaturePath.parent_path()) || !fs::is_directory(signaturePath.parent_path())){
		fs::create_directories(signaturePath.parent_path());
	}
	QRGenerator qrg;
	qrg.GeneratorQR("message", signaturePath.string());

	// ͨ��template��Ⱦ�ķ�ʽд�룬��ʱ����
	// writeFile(docHeaderRelPath.string(), env_default.render_file("\\default-header.xml.rels", data));

	// render default header template
	fs::path docHeaderPath = fs::path(m_strWordTempPath) / "word/header1.xml";
	fs::copy_file(wordTemplates / "default-header-mark.xml", docHeaderPath);

	// ͨ��template��Ⱦ�ķ�ʽд�룬��ʱ����
	// writeFile(docHeaderPath.string(), env_default.render_file("\\default-header-mark.xml", data));

	// [Content_Types].xml ��Ҫ�ж�header�ļ�������
	fs::path docContentTypePath = fs::path(m_strWordTempPath) / "[Content_Types].xml";
	auto contentDoc = XMLOperation(docContentTypePath.string().data());

	xmlNodePtr xxxNode = xmlNewNode(0, BAD_CAST "Override");
	xmlNewProp(xxxNode, BAD_CAST "PartName", BAD_CAST "/word/header1.xml");
	xmlNewProp(xxxNode, BAD_CAST "ContentType", BAD_CAST "application/vnd.openxmlformats-officedocument.wordprocessingml.header+xml");

	xmlAddChild(contentDoc.xmlGetRootNode(), xxxNode);

	// ��� <Default Extension="png" ContentType="image/png"/>
	// ��������������
	// ���鷳�������
	xxxNode = xmlNewNode(0, BAD_CAST "Default");
	xmlNewProp(xxxNode, BAD_CAST "Extension", BAD_CAST "png");
	xmlNewProp(xxxNode, BAD_CAST "ContentType", BAD_CAST "image/png");
	xmlAddChild(contentDoc.xmlGetRootNode(), xxxNode);

	//// �˴�д��.bak�ļ����ǣ����� -> ɾ�� -> �����ķ�ʽ����ֹ��ͻ��
	//// ������ʱ�ļ�
	//tempPath = docContentTypePath.str() + ".bak";
	//contentDoc.outputFile(tempPath.str());
	//
	//// ɾ��ԭ�ļ�
	//std::remove(docContentTypePath.str().data());
	//
	//// �޸��ļ������滻ԭ�ļ�����
	//std::rename(tempPath.str().data(), docContentTypePath.str().data());
	contentDoc.outputFile(docContentTypePath.string());

	maxId++;
	std::string newMaxRefId = std::string(std::string("rId") + std::to_string(maxId));

	// ��ȡword/_rels/document.xml.rels,���䴴��header id������
	xmlNodePtr newNode = xmlNewNode(0, BAD_CAST "Relationship");
	xmlNewProp(newNode, BAD_CAST "Id", BAD_CAST newMaxRefId.data());
	xmlNewProp(newNode, BAD_CAST "Type", BAD_CAST "http://schemas.openxmlformats.org/officeDocument/2006/relationships/header");
	xmlNewProp(newNode, BAD_CAST "Target", BAD_CAST "header1.xml");
	xmlAddChild(m_DocWordRelation->xmlGetRootNode(), newNode);
	m_DocWordRelation->outputFile(m_DocWordRelPath.string());

	/**
	* ��ʱrelationship�Ѿ���������ˣ��ò���word/document.xml�ļ���
	* ���û��header�Ļ�,��Ҫ����element��header����
	*/
	// read main doc, document.xml
	fs::path wordDocPath = fs::path(m_strWordTempPath) / "word/document.xml";
	xmlXPathObjectPtr xpathObj = NULL;
	auto wordDoc = XMLOperation(wordDocPath.string().data());
	int foundSize = wordDoc.xmlXPathFindObjects(BAD_CAST "//w:document/w:body/w:sectPr", xpathObj);
	if (foundSize == 0) {
		DEBUGPRINT("[BBB] document.xml can't find element..");
		return false;
	}

	// ����libxml2��������ҪQname, �����w��Ҫ��root���䡣
	// ������Ҫ��href���飬�ݴ��ʻ�Ƚϸߡ�
	xmlNodePtr newNodeRef = xmlNewNode(0, BAD_CAST "w:headerReference");
	xmlNewProp(newNodeRef, BAD_CAST "r:id", BAD_CAST newMaxRefId.data());
	xmlNewProp(newNodeRef, BAD_CAST "w:type", BAD_CAST "default");

	// xpath founded node
	xmlNodeSetPtr nodes = xpathObj->nodesetval;
	xmlNodePtr firstNode = nodes->nodeTab[0];
	xmlAddChild(firstNode, newNodeRef);
	wordDoc.outputFile(wordDocPath.string());

	return true;
}

bool WordMarker::WaterMarkUpdate(std::string message){

	if (!m_bMarked){
		DEBUGPRINT("can't found watermark file.");
		return false;
	}

	QRReader *qrr = new QRReader();
	std::string qrMsg;

	std::string filepath = m_strWaterMarkFile;
	qrr->ReadQR(filepath, qrMsg);
	std::cout << "QR message:" << qrMsg << std::endl;
	delete qrr;

	qrMsg = qrMsg + ";" + message;

	// ɾ��ԭˮӡ�ļ�
	fs::remove(m_strWaterMarkFile);

	// ����ˮӡ
	QRGenerator qrg;
	qrg.GeneratorQR(qrMsg, m_strWaterMarkFile);

	return true;
}

void WordMarker::SaveToFile(std::string outputPath){
	ZipHelper2 zh;
	zh.ZipFile(m_strWordTempPath, outputPath);

	fs::remove_all(m_strWordTempPath);
}

WordMarker::~WordMarker(){
	if (m_DocWordRelation)
		delete m_DocWordRelation;
}