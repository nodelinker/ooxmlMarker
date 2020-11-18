#include "WordMarker2.h"
#include "Util/debug_printf.h"


WordMarker2::WordMarker2(std::string wordPath) :m_strWordFilePath(wordPath){

	fs::path tempDocPath = fs::current_path() / fs::unique_path();
	if (fs::exists(tempDocPath) && fs::is_directory(tempDocPath)){
		printfTrace("tmp directory exists.");
		return;
	}

	if (!fs::create_directory(tempDocPath)){
		printfTrace("tmp directory can't create.");
		return;
	}

	m_strWordTempPath = (fs::current_path() / fs::unique_path()).string();

	ZipHelper2 zh;
	int ret = zh.UnZipFile(m_strWordFilePath, m_strWordTempPath);
	if (ret == 0){
		m_bUnzipSuccess = true;
	}

	this->FindWaterMark();
}

WordMarker2::WordMarker2(std::string wordPath, std::string wordTempPath) :
m_strWordFilePath(wordPath), m_strWordTempPath(wordTempPath){

	ZipHelper2 zh;
	int ret = zh.UnZipFile(m_strWordFilePath, m_strWordTempPath);
	if (ret == 0){
		m_bUnzipSuccess = true;
	}

	this->FindWaterMark();
}


bool WordMarker2::FindWaterMark(){

	// 读取header.xml文件，并检查是否存在水印信息
	fs::path documentPathj = fs::path(m_strWordTempPath) / "word/document.xml";
	if (!fs::exists(documentPathj)){
		printfTrace("Can't found Document with : %s \n", documentPathj.string().data());
		return false;
	}

	// Todo: 去header.xml内检查是否存在二维码内容
	xmlXPathObjectPtr xpathObj = NULL;
	auto documentDoc = new XMLOperation(documentPathj.string().data());

	// registe namespace with xml.
	std::vector<std::string> ns;
	ns.push_back("a=http://schemas.openxmlformats.org/drawingml/2006/main");
	ns.push_back("pic=http://schemas.openxmlformats.org/drawingml/2006/picture");
	documentDoc->xmlXPathRegisterNamespace(ns);

	// 这里应该增加一个判断，如果没有header没水印的情况
	// watermark signature tag by desc with "mark"
	int foundSize = documentDoc->xmlXPathFindObjects(
		BAD_CAST "//w:drawing/wp:anchor/a:graphic/a:graphicData/pic:pic/pic:nvPicPr/pic:cNvPr[@descr='mark']",
		xpathObj);

	if (foundSize == 0) {
		printfTrace("[AAA] can't find marked element..");

		if (documentDoc)
			delete documentDoc;

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
	foundSize = documentDoc->xmlXPathFindObjects(picNode, BAD_CAST "pic:blipFill/a:blip", xpathObj);
	if (foundSize == 0) {
		printfTrace("[AAA] can't find pic:blipFill element..");

		if (documentDoc)
			delete documentDoc;

		return false;
	}
	nodes = xpathObj->nodesetval;
	firstNode = nodes->nodeTab[0];
	xmlChar* markedRelId = xmlGetProp(firstNode, BAD_CAST "embed");


	// 读取 /word/_rels/document.xml.rels
	// <Relationship Id="rIdx" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/image" Target="media/image1.png"/>
	fs::path documentRelsPath = (fs::path(m_strWordTempPath) / "word/_rels/document.xml.rels");
	if (!fs::exists(documentRelsPath))
	{
		printfTrace("[AAA] can't find file /word/_rels/document.xml.rels ..");

		if (documentDoc)
			delete documentDoc;

		return false;
	}
	auto documentRelDoc = new XMLOperation(documentRelsPath.string().data());
	foundSize = 0;
	xpathObj = NULL;
	firstNode = NULL;


	// 这里的坑标识一下
	// 如果xml定义了，类似这种没有名字的namespace。
	// xmlns="http://schemas.openxmlformats.org/package/2006/relationships"
	// 必须给他设置一个名字，并在这个namespace中进行xpath查询
	boost::format fmt = boost::format("//null:Relationships/null:Relationship[@Id='%s']") % (char*)markedRelId;
	std::string pattern = fmt.str();
	foundSize = documentRelDoc->xmlXPathFindObjects(BAD_CAST pattern.data(), xpathObj);
	if (foundSize == 0) {
		printfTrace("[AAA] can't find Relationship element id is %d.", markedRelId);

		if (documentDoc)
			delete documentDoc;
		if (documentRelDoc)
			delete documentRelDoc;

		return false;
	}
	nodes = xpathObj->nodesetval;
	firstNode = nodes->nodeTab[0];

	std::string markedFileTarget = (char *)xmlGetProp(firstNode, BAD_CAST "Target");
	fs::path markedFileAbsPath = fs::path(m_strWordTempPath) / "word" / markedFileTarget;

	m_strWaterMarkFile = markedFileAbsPath.string();
	m_bMarked = true;
	printfTrace("Marked file %s, %d.", m_strWaterMarkFile.data(), m_bMarked);

	if (documentDoc)
		delete documentDoc;
	if (documentRelDoc)
		delete documentRelDoc;

	return true;
}

void WordMarker2::GetRelationFileMaxId(std::string relFilePath, std::string &maxID){
	
	std::vector<std::string> vecRefId;
	auto relDoc = XMLOperation(relFilePath.data());
	xmlNodePtr root = relDoc.xmlGetRootNode();

	for (xmlNodePtr tmp = root->children; tmp != NULL; tmp = tmp->next) {

		xmlChar* rIdProp = xmlGetProp(tmp, BAD_CAST "Id");
		xmlChar* typeProp = xmlGetProp(tmp, BAD_CAST "Type");
		xmlChar* targetProp = xmlGetProp(tmp, BAD_CAST "Target");

		if (!rIdProp) {
			xmlFree(targetProp);
			xmlFree(typeProp);
			xmlFree(rIdProp);
			continue;
		}

		if (!typeProp) {
			xmlFree(targetProp);
			xmlFree(typeProp);
			xmlFree(rIdProp);
			continue;
		}
		vecRefId.push_back(std::string((char*)rIdProp));

		xmlFree(targetProp);
		xmlFree(typeProp);
		xmlFree(rIdProp);
	}

	std::sort(vecRefId.begin(), vecRefId.end());
	maxID = vecRefId.back();

	return;
}

std::string WordMarker2::WordHiddenImageTemplte(std::string rId){

	std::string wparagraphMarkContent = R"(
<w:r xmlns:wpc="http://schemas.microsoft.com/office/word/2010/wordprocessingCanvas"
  xmlns:cx="http://schemas.microsoft.com/office/drawing/2014/chartex"
  xmlns:cx1="http://schemas.microsoft.com/office/drawing/2015/9/8/chartex"
  xmlns:cx2="http://schemas.microsoft.com/office/drawing/2015/10/21/chartex"
  xmlns:cx3="http://schemas.microsoft.com/office/drawing/2016/5/9/chartex"
  xmlns:cx4="http://schemas.microsoft.com/office/drawing/2016/5/10/chartex"
  xmlns:cx5="http://schemas.microsoft.com/office/drawing/2016/5/11/chartex"
  xmlns:cx6="http://schemas.microsoft.com/office/drawing/2016/5/12/chartex"
  xmlns:cx7="http://schemas.microsoft.com/office/drawing/2016/5/13/chartex"
  xmlns:cx8="http://schemas.microsoft.com/office/drawing/2016/5/14/chartex"
  xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
  xmlns:aink="http://schemas.microsoft.com/office/drawing/2016/ink"
  xmlns:am3d="http://schemas.microsoft.com/office/drawing/2017/model3d"
  xmlns:o="urn:schemas-microsoft-com:office:office"
  xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships"
  xmlns:m="http://schemas.openxmlformats.org/officeDocument/2006/math"
  xmlns:v="urn:schemas-microsoft-com:vml"
  xmlns:wp14="http://schemas.microsoft.com/office/word/2010/wordprocessingDrawing"
  xmlns:wp="http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing"
  xmlns:w10="urn:schemas-microsoft-com:office:word"
  xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
  xmlns:w14="http://schemas.microsoft.com/office/word/2010/wordml"
  xmlns:w15="http://schemas.microsoft.com/office/word/2012/wordml"
  xmlns:w16cex="http://schemas.microsoft.com/office/word/2018/wordml/cex"
  xmlns:w16cid="http://schemas.microsoft.com/office/word/2016/wordml/cid"
  xmlns:w16="http://schemas.microsoft.com/office/word/2018/wordml"
  xmlns:w16se="http://schemas.microsoft.com/office/word/2015/wordml/symex"
  xmlns:wpg="http://schemas.microsoft.com/office/word/2010/wordprocessingGroup"
  xmlns:wpi="http://schemas.microsoft.com/office/word/2010/wordprocessingInk"
  xmlns:wne="http://schemas.microsoft.com/office/word/2006/wordml"
  xmlns:wps="http://schemas.microsoft.com/office/word/2010/wordprocessingShape">
  <w:rPr>
    <w:b/>
  </w:rPr>
  <w:drawing>
    <wp:anchor distT="0" distB="0" distL="114300" distR="114300" simplePos="1" relativeHeight="251658240" behindDoc="1" locked="0" layoutInCell="1" allowOverlap="1">
      <wp:simplePos x="3314700" y="1569085"/>
      <wp:positionH relativeFrom="column">
        <wp:posOffset>0</wp:posOffset>
      </wp:positionH>
      <wp:positionV relativeFrom="paragraph">
        <wp:posOffset>0</wp:posOffset>
      </wp:positionV>
      <wp:extent cx="2324100" cy="1892300"/>
      <wp:effectExtent l="0" t="0" r="0" b="0"/>
      <wp:wrapNone/>
      <wp:docPr id="2" name="Picture" hidden="1" descr="mark"/>
      <wp:cNvGraphicFramePr>
        <a:graphicFrameLocks xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" noChangeAspect="1"/>
      </wp:cNvGraphicFramePr>
      <a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
        <a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture">
          <pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture">
            <pic:nvPicPr>
              <pic:cNvPr id="2" name="Picture" hidden="1" descr="mark"/>
              <pic:cNvPicPr/>
            </pic:nvPicPr>
            <pic:blipFill>
              <a:blip r:embed="{{RelId}}"/>
              <a:stretch>
                <a:fillRect/>
              </a:stretch>
            </pic:blipFill>
            <pic:spPr>
              <a:xfrm>
                <a:off x="0" y="0"/>
                <a:ext cx="2324100" cy="1892300"/>
              </a:xfrm>
              <a:prstGeom prst="rect">
                <a:avLst/>
              </a:prstGeom>
            </pic:spPr>
          </pic:pic>
        </a:graphicData>
      </a:graphic>
    </wp:anchor>
  </w:drawing>
</w:r>)";

	Jinja2Light::Template TPWaterMark(wparagraphMarkContent);
	TPWaterMark.setValue("RelId", rId);
	std::string stmp = TPWaterMark.render();
	return stmp;
}

bool WordMarker2::WaterMarkGenerate(std::string message){
	
	std::string maxRelId;
	fs::path documentRelPath = fs::path(m_strWordTempPath) / "word/_rels/document.xml.rels";
	if (!fs::exists(documentRelPath)){
		printfTrace("Can't found : %s \n", documentRelPath.string().data());
		return false;
	}

	std::vector<std::string> vecRefId;
	auto documentRelDoc = new XMLOperation(documentRelPath.string().data());
	xmlNodePtr root = documentRelDoc->xmlGetRootNode();

	for (xmlNodePtr tmp = root->children; tmp != NULL; tmp = tmp->next) {

		xmlChar* rIdProp = xmlGetProp(tmp, BAD_CAST "Id");
		xmlChar* typeProp = xmlGetProp(tmp, BAD_CAST "Type");
		xmlChar* targetProp = xmlGetProp(tmp, BAD_CAST "Target");

		if (!rIdProp) {
			xmlFree(targetProp);
			xmlFree(typeProp);
			xmlFree(rIdProp);
			continue;
		}

		if (!typeProp) {
			xmlFree(targetProp);
			xmlFree(typeProp);
			xmlFree(rIdProp);
			continue;
		}
		vecRefId.push_back(std::string((char*)rIdProp));

		xmlFree(targetProp);
		xmlFree(typeProp);
		xmlFree(rIdProp);
	}

	std::sort(vecRefId.begin(), vecRefId.end());
	maxRelId = vecRefId.back();

	unsigned int maxId = 0;
	std::regex		_repr("rId(\\d+)$");
	std::smatch		_match;
	std::regex_search(maxRelId, _match, _repr);
	if (_match.size() > 1){
		maxId = atoi(_match.str(1));
	}

	// 创建二维码水印
	fs::path signaturePath = fs::path(m_strWordTempPath) / "word/media/image1.png";
	if (!fs::exists(signaturePath.parent_path()) || !fs::is_directory(signaturePath.parent_path())){
		fs::create_directories(signaturePath.parent_path());
	}
	QRGenerator qrg;
	qrg.GeneratorQR(message.data(), message.length(), signaturePath.string().data(), 0x0);

	// [Content_Types].xml 需要有对header文件的引用
	fs::path docContentTypePath = fs::path(m_strWordTempPath) / "[Content_Types].xml";
	auto contentDoc = new XMLOperation(docContentTypePath.string().data());

	// 检测 <Default Extension="png" ContentType="image/png"/>
	// 如果不存在则添加
	// 嫌麻烦暴力添加
	xmlNodePtr xxxNode = xmlNewNode(0, BAD_CAST "Default");
	xmlNewProp(xxxNode, BAD_CAST "Extension", BAD_CAST "png");
	xmlNewProp(xxxNode, BAD_CAST "ContentType", BAD_CAST "image/png");
	xmlAddChild(contentDoc->xmlGetRootNode(), xxxNode);

	//// 此处写入.bak文件考虑，创建 -> 删除 -> 改名的方式，防止冲突。
	//// 创建临时文件
	//tempPath = docContentTypePath.str() + ".bak";
	//contentDoc.outputFile(tempPath.str());
	//
	//// 删除原文件
	//std::remove(docContentTypePath.str().data());
	//
	//// 修改文件名，替换原文件名。
	//std::rename(tempPath.str().data(), docContentTypePath.str().data());
	contentDoc->outputFile(docContentTypePath.string());
	delete contentDoc;

	maxId++;
	std::string newMaxRelId = std::string(std::string("rId") + std::to_string(maxId));

	// 读取word/_rels/document.xml.rels,在其创建media/xxx.png的引用
	// <Relationship Id="rId9" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/image" Target="media/image1.png"/>

	xmlNodePtr newNode = xmlNewNode(0, BAD_CAST "Relationship");
	xmlNewProp(newNode, BAD_CAST "Id", BAD_CAST newMaxRelId.data());
	xmlNewProp(newNode, BAD_CAST "Type", BAD_CAST "http://schemas.openxmlformats.org/officeDocument/2006/relationships/image");
	xmlNewProp(newNode, BAD_CAST "Target", BAD_CAST "media/image1.png");
	xmlAddChild(documentRelDoc->xmlGetRootNode(), newNode);
	documentRelDoc->outputFile(documentRelPath.string());
	delete documentRelDoc;


	std::string tplWaterMark = WordHiddenImageTemplte(newMaxRelId);
	xmlDocPtr paragraphDoc = xmlReadMemory(
		tplWaterMark.data(), tplWaterMark.length() + 1,
		NULL, NULL, 0);

	if (!paragraphDoc){
		printfTrace("Read template failed \n");
		xmlFreeDoc(paragraphDoc);
		return false;
	}

	fs::path documentPath = fs::path(m_strWordTempPath) / "word/document.xml";;
	if (!fs::exists(documentPath)){
		printfTrace("failed to read from %s \n", documentPath.string().data());
		xmlFreeDoc(paragraphDoc);
		return false;
	}

	xmlXPathObjectPtr xpathObj = NULL;
	XMLOperation *xmlOp = new XMLOperation(documentPath.string().data());

	// 找到document.xml内所有段落
	int foundSize = xmlOp->xmlXPathFindObjects(BAD_CAST "//w:p", xpathObj);
	if (foundSize == 0) {
		printfTrace("[AAA] does not enough paragraphs ..");
		xmlFreeDoc(paragraphDoc);
		delete xmlOp;
		return false;
	}
	xmlNodeSetPtr nodes = xpathObj->nodesetval;

	std::vector<xmlNodePtr> NodeList;
	for (int i = 0; i < foundSize; i++){
		xmlNodePtr firstNode = nodes->nodeTab[i];
		xmlNodePtr paragraphNode = xmlDocCopyNode(
			xmlDocGetRootElement(paragraphDoc),
			firstNode->doc,
			1);

		NodeList.push_back(paragraphNode);
		
		if (!paragraphNode){
			goto cleanup;
		}
		xmlNodePtr addedNode = xmlAddChildList(firstNode, paragraphNode->children);
		if (!addedNode) {
			goto cleanup;
		}
		 paragraphNode->children = NULL; // Thanks to milaniez from stackoverflow
		 paragraphNode->last = NULL;     // for fixing
	}
	
	xmlOp->outputFile(documentPath.string());

cleanup:
	// clean xml Node
	for (int i = 0; i < NodeList.size(); i++){
		xmlFreeNode(NodeList[i]);
	}
	xmlFreeDoc(paragraphDoc);
	
	if (xmlOp)
		delete xmlOp;

	return true;
}

bool WordMarker2::readMark(std::string &message){

	if (!m_bMarked){
		printfTrace("this file does not marked. \n");
		return false;
	}

	QRReader *qrr = new QRReader();

	char* oldMessage = NULL;
	std::string filepath = m_strWaterMarkFile;

	qrr->ReadQR(filepath.data(), &oldMessage);
	std::cout << "QR message:" << oldMessage << std::endl;
	delete qrr;

	message = std::string(oldMessage);
	QRCommon::QRFree(oldMessage);

	return true;
}

std::string WordMarker2::readMark(){

	if (!m_bMarked){
		printfTrace("this file does not marked. \n");
		return "";
	}

	QRReader qrr = QRReader();

	char *oldMessage = NULL;
	std::string filepath = m_strWaterMarkFile;
	qrr.ReadQR(filepath.data(), &oldMessage);

	std::string ret(oldMessage);

	std::cout << "QR message:" << oldMessage << std::endl;
	QRCommon::QRFree(oldMessage);
	return ret;
}

bool WordMarker2::WaterMarkUpdate(std::string message){

	if (!m_bMarked){
		printfTrace("this file does not marked. \n");
		return false;
	}

	// 删除原水印文件
	fs::remove(m_strWaterMarkFile);

	// 更新水印
	QRGenerator qrg;
	qrg.GeneratorQR(message.data(), message.length(), m_strWaterMarkFile.data(), 0x0);

	return true;
}

void WordMarker2::SaveToFile(std::string outputPath){
	ZipHelper2 zh;
	zh.ZipFile(m_strWordTempPath, outputPath);

	fs::remove_all(m_strWordTempPath);
}

WordMarker2::~WordMarker2(){
}