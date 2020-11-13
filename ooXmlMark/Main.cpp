#include <Windows.h>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include <QRManager/QRReader.h>
#include <QRManager/QRGenerator.h>

#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include "ZipHelper2.h"
#include "Util/Jinja2Light.h"
#include "Util/Logging.h"

#include "XmlOperation.h"
#include "WordMarker.h"


namespace fs = boost::filesystem;

//const fs::path wordTemplates = fs::current_path() / "templates" / "word";
//const fs::path excelTemplates = fs::current_path() / "templates" / "excel";
//const fs::path pptTemplates = fs::current_path() / "templates" / "ppt";
//
//
//int docxMarkerDemo(int argc, char **argv)
//{
//	LoggingInit();
//
//	fs::path demoPath = fs::current_path() / "demo.marked.docx";
//	if (!fs::exists(demoPath)){
//		std::cout << "demo.docx can't found." << "" << std::endl;
//		return -1;
//	}
//
//	fs::path tempDocPath = fs::current_path() / fs::unique_path();
//	if (fs::exists(tempDocPath) && fs::is_directory(tempDocPath)){
//		std::cout << "tmp directory exists." << "" << std::endl;
//		return -1;
//	}
//	
//	if (!fs::create_directory(tempDocPath)){
//		std::cout << "tmp directory can't create." << std::endl;
//		return -1;
//	}
//
//	std::cout << demoPath << std::endl;
//	std::cout << tempDocPath << std::endl;
//	
//	// 解压
//	ZipHelper2 zh;
//	zh.UnZipFile(demoPath.string(), tempDocPath.string());
//
//	bool bFoundHeader = false;
//	std::string strHeaderId = "";
//	std::string strHeaderTarget = "";
//
//	std::vector<std::string> refIdVec;
//
//	// step 1 检查是否存在header在主文档的relation文件中。
//	// C:\\Users\\xszhang\\Desktop\\数字水印\\sample\\docx\\a\\demo1\\word\\_rels\\document.xml.rels
//	fs::path docRelsPath = (tempDocPath / fs::path("word\\_rels\\document.xml.rels"));
//
//	auto relWordDoc = XMLOperation(docRelsPath.string().data());
//	xmlNodePtr root = relWordDoc.xmlGetRootNode();
//
//	// mark header refer
//	//std::regex pattern("schemas\.openxmlformats\.org/officeDocument/(\d+)/relationships/header$");
//	std::regex pattern(R"((http|https)://schemas\.openxmlformats\.org/officeDocument/(\d+)/relationships/header$)");
//
//	for (xmlNodePtr tmp = root->children; tmp != NULL; tmp = tmp->next) {
//
//		xmlChar* rIdProp = xmlGetProp(tmp, BAD_CAST "Id");
//		xmlChar* typeProp = xmlGetProp(tmp, BAD_CAST "Type");
//		xmlChar* targetProp = xmlGetProp(tmp, BAD_CAST "Target");
//
//		if (!rIdProp) {
//			continue;
//		}
//
//		if (!typeProp) {
//			continue;
//		}
//
//		refIdVec.push_back(std::string((char*)rIdProp));
//
//		std::string data = std::string((char*)typeProp);
//		if (std::regex_match(data, pattern)) {
//			printf("found Property Type: %s, ID %s\n", typeProp, rIdProp);
//			bFoundHeader = true;
//			strHeaderId = (char*)rIdProp;
//			strHeaderTarget = (char*)targetProp;
//		}
//
//		xmlFree(targetProp);
//		xmlFree(typeProp);
//		xmlFree(rIdProp);
//	}
//
//	std::sort(refIdVec.begin(), refIdVec.end());
//	std::string maxRefId = refIdVec.back();
//	DEBUGPRINT("document relationship max reference id {}", maxRefId);
//
//	if (bFoundHeader) {
//
//		DEBUGPRINT("AAAAAAAAAAAAAAAAAAAAAAAAAAAA", maxRefId);
//		// 如果header存在检查id
//		std::cout << strHeaderId << std::endl;
//
//		// 读取header.xml文件，并检查是否存在水印信息
//		fs::path headerPath = tempDocPath / fs::path("word") / fs::path(strHeaderTarget);
//		std::cout << headerPath << std::endl;
//
//		// Todo: 去header.xml内检查是否存在二维码内容
//		xmlXPathObjectPtr xpathObj = NULL;
//		auto headerDoc = XMLOperation(headerPath.string().data());
//
//		std::vector<std::string> ns;
//		ns.push_back("a=http://schemas.openxmlformats.org/drawingml/2006/main");
//		ns.push_back("pic=http://schemas.openxmlformats.org/drawingml/2006/picture");
//		headerDoc.xmlXPathRegisterNamespace(ns);
//
//		int foundSize = headerDoc.xmlXPathFindObjects(
//			BAD_CAST "//w:hdr/w:p/w:r/w:drawing/wp:anchor/a:graphic/a:graphicData/pic:pic/pic:nvPicPr/pic:cNvPr[@descr='mark']",
//			xpathObj);
//
//		if (foundSize == 0) {
//			DEBUGPRINT("[AAA] can't find marked element..");
//			return -1;
//		}
//
//		xmlNodeSetPtr nodes = xpathObj->nodesetval;
//		xmlNodePtr firstNode = nodes->nodeTab[0];
//
//		if (firstNode->parent) {
//			firstNode = firstNode->parent;
//		}
//
//		xmlNodePtr picNode = NULL;
//		if (firstNode->parent) {
//			picNode = firstNode->parent;
//		}
//
//		// pic:nvPicPr
//		foundSize = 0;
//		xpathObj = NULL;
//		foundSize = headerDoc.xmlXPathFindObjects(picNode, BAD_CAST "pic:blipFill/a:blip", xpathObj);
//		if (foundSize == 0) {
//			DEBUGPRINT("[AAA] can't find pic:blipFill element..");
//			return -1;
//		}
//
//		nodes = xpathObj->nodesetval;
//		firstNode = nodes->nodeTab[0];
//
//		xmlChar* markedRelId = xmlGetProp(firstNode, BAD_CAST "embed");
//
//		fs::path headerRelsPath = (tempDocPath / fs::path("word\\_rels\\header1.xml.rels"));
//		auto headerRelDoc = XMLOperation(headerRelsPath.string().data());
//
//		foundSize = 0;
//		xpathObj = NULL;
//		firstNode = NULL;
//
//
//		// 这里的坑标识一下
//		// 如果xml定义了，类似这种没有名字的namespace。
//		// xmlns="http://schemas.openxmlformats.org/package/2006/relationships"
//		// 必须给他设置一个名字，并在这个namespace中进行xpath查询
//
//		boost::format fmt = boost::format("//null:Relationships/null:Relationship[@Id='%s']") % (char*)markedRelId;
//		std::string pattern = fmt.str();
//		foundSize = headerRelDoc.xmlXPathFindObjects(BAD_CAST pattern.data(), xpathObj);
//		if (foundSize == 0) {
//			DEBUGPRINT("[AAA] can't find Relationship element id is {}.", markedRelId);
//			return -1;
//		}
//		nodes = xpathObj->nodesetval;
//		firstNode = nodes->nodeTab[0];
//
//		std::string markedFileTarget = (char *)xmlGetProp(firstNode, BAD_CAST "Target");
//
//		fs::path markedFileAbsPath = (tempDocPath / "word" /fs::path(markedFileTarget));
//		std::cout << "Marker file." << markedFileAbsPath << " " << fs::exists(markedFileAbsPath) << std::endl;
//
//		QRReader *qrr = new QRReader();
//		std::string qrMsg;
//
//		std::string filepath = markedFileAbsPath.string();
//		qrr->ReadQR(filepath, qrMsg);
//		std::cout << "QR message:" << qrMsg << std::endl;
//		delete qrr;
//
//		qrMsg = qrMsg + "; update Message";
//
//		// 删除原水印文件
//		fs::remove(markedFileAbsPath);
//
//		// 更新水印
//		QRGenerator qrg;
//		qrg.GeneratorQR(qrMsg, markedFileAbsPath.string());
//	}
//	else {
//		DEBUGPRINT("BBBBBBBBBBBBBBBBBBBBBBBBBBBB", maxRefId);
//
//		fs::path tempPath;
//		// 如果没有header ref文件引用，则使用template建立新的header并添加reference.
//
//		unsigned int maxId = 0;
//		std::cout << maxRefId << std::endl;
//
//		std::regex		_repr("rId(\\d+)$");
//		std::smatch		_match;
//		std::regex_search(maxRefId, _match, _repr);
//		if (_match.size() > 1){
//			maxId = atoi(_match.str(1));
//		}
//
//		// RE2::FullMatch(maxRefId, "rId(\\d+)$", &maxId);
//		// DEBUGPRINT("document relationship max reference id: {}", maxId);
//		std::cout << maxRefId << std::endl;
//
//		// add header rels with template.
//		fs::path docHeaderRelPath = (tempDocPath / fs::path("word\\_rels\\header1.xml.rels"));
//		fs::copy_file(wordTemplates / "default-header.xml.rels", docHeaderRelPath);
//
//		// 创建二维码水印
//		fs::path signaturePath = tempDocPath / "word/media/image1.png";
//		if (!fs::exists(signaturePath.parent_path()) || !fs::is_directory(signaturePath.parent_path())){
//			fs::create_directories(signaturePath.parent_path());
//		}
//		QRGenerator qrg;
//		qrg.GeneratorQR("message", signaturePath.string());
//
//
//		// 通过template渲染的方式写入，暂时不用
//		// writeFile(docHeaderRelPath.string(), env_default.render_file("\\default-header.xml.rels", data));
//
//		// render default header template
//		fs::path docHeaderPath = (tempDocPath / fs::path("word\\header1.xml"));
//		fs::copy_file(wordTemplates / "default-header-mark.xml", docHeaderPath);
//
//		// 通过template渲染的方式写入，暂时不用
//		// writeFile(docHeaderPath.string(), env_default.render_file("\\default-header-mark.xml", data));
//
//		// [Content_Types].xml 需要有对header文件的引用
//		fs::path docContentTypePath = (tempDocPath / fs::path("[Content_Types].xml"));
//		auto contentDoc = XMLOperation(docContentTypePath.string().data());
//
//		xmlNodePtr xxxNode = xmlNewNode(0, BAD_CAST "Override");
//		xmlNewProp(xxxNode, BAD_CAST "PartName", BAD_CAST "/word/header1.xml");
//		xmlNewProp(xxxNode, BAD_CAST "ContentType", BAD_CAST "application/vnd.openxmlformats-officedocument.wordprocessingml.header+xml");
//
//		xmlAddChild(contentDoc.xmlGetRootNode(), xxxNode);
//
//		// 检测 <Default Extension="png" ContentType="image/png"/>
//		// 如果不存在则添加
//		// 嫌麻烦暴力添加
//		xxxNode = xmlNewNode(0, BAD_CAST "Default");
//		xmlNewProp(xxxNode, BAD_CAST "Extension", BAD_CAST "png");
//		xmlNewProp(xxxNode, BAD_CAST "ContentType", BAD_CAST "image/png");
//		xmlAddChild(contentDoc.xmlGetRootNode(), xxxNode);
//
//
//		//// 此处写入.bak文件考虑，创建 -> 删除 -> 改名的方式，防止冲突。
//		//// 创建临时文件
//		//tempPath = docContentTypePath.str() + ".bak";
//		//contentDoc.outputFile(tempPath.str());
//		//
//		//// 删除原文件
//		//std::remove(docContentTypePath.str().data());
//		//
//		//// 修改文件名，替换原文件名。
//		//std::rename(tempPath.str().data(), docContentTypePath.str().data());
//
//		tempPath = docContentTypePath.string();
//		contentDoc.outputFile(tempPath.string());
//
//		maxId++;
//		std::string maxRefId = std::string(std::string("rId") + std::to_string(maxId)).data();
//
//		// 读取word/_rels/document.xml.rels,在其创建header id并引用
//		xmlNodePtr newNode = xmlNewNode(0, BAD_CAST "Relationship");
//		xmlNewProp(newNode, BAD_CAST "Id", BAD_CAST maxRefId.data());
//		xmlNewProp(newNode, BAD_CAST "Type", BAD_CAST "http://schemas.openxmlformats.org/officeDocument/2006/relationships/header");
//		xmlNewProp(newNode, BAD_CAST "Target", BAD_CAST "header1.xml");
//		xmlAddChild(root, newNode);
//
//		tempPath = docRelsPath;
//		relWordDoc.outputFile(tempPath.string());
//
//		/**
//		* 此时relationship已经创建完成了，该操作word/document.xml文件了
//		* 如果没有header的话,需要创建element对header引用
//		*/
//
//		fs::path wordDocPath = (tempDocPath / "word" / "document.xml");
//
//		// read main doc, document.xml
//		xmlXPathObjectPtr xpathObj = NULL;
//		auto wordDoc = XMLOperation(wordDocPath.string().data());
//		int foundSize = wordDoc.xmlXPathFindObjects(BAD_CAST "//w:document/w:body/w:sectPr", xpathObj);
//		if (foundSize == 0) {
//			DEBUGPRINT("[BBB] document.xml can't find element..");
//		}
//
//		// 这里libxml2看来不需要Qname, 这里的w需要跟root对其。
//		// 可能需要用href反查，容错率会比较高。
//		xmlNodePtr newNodeRef = xmlNewNode(0, BAD_CAST "w:headerReference");
//		xmlNewProp(newNodeRef, BAD_CAST "r:id", BAD_CAST maxRefId.data());
//		xmlNewProp(newNodeRef, BAD_CAST "w:type", BAD_CAST "default");
//
//		// xpath founded node
//		xmlNodeSetPtr nodes = xpathObj->nodesetval;
//		xmlNodePtr firstNode = nodes->nodeTab[0];
//		xmlAddChild(firstNode, newNodeRef);
//		wordDoc.outputFile(wordDocPath.string());
//
//		//xmlFree(newNodeRef);
//		//xmlFree(xxxNode);
//		//xmlFree(newNode);
//	}
//
//	fs::path outputPath = tempDocPath / "sample.docx";
//	zh.ZipFile(tempDocPath.string(), outputPath.string());
//
//	return 0;
//}


int main(int argc, char **argv){


	//fs::path demoPath = fs::current_path() / "demo.docx";
	//if (!fs::exists(demoPath)){
	//	std::cout << "demo.docx can't found." << "" << std::endl;
	//	return -1;
	//}
	//
	//fs::path tempDocPath = fs::current_path() / fs::unique_path();
	//if (fs::exists(tempDocPath) && fs::is_directory(tempDocPath)){
	//	std::cout << "tmp directory exists." << "" << std::endl;
	//	return -1;
	//}
	//	
	//if (!fs::create_directory(tempDocPath)){
	//	std::cout << "tmp directory can't create." << std::endl;
	//	return -1;
	//}
	//
	//WordMarker *wmk = new WordMarker(demoPath.string(), tempDocPath.string());
	//wmk->WaterMarkGenerate("hello world");

	//fs::path markOutputPath = demoPath.parent_path() / "demo.marked.docx";
	//wmk->SaveToFile(markOutputPath.string());
	//delete wmk;

	//// 测试更新
	//tempDocPath = fs::current_path() / fs::unique_path();
	//if (fs::exists(tempDocPath) && fs::is_directory(tempDocPath)){
	//	std::cout << "tmp directory exists." << "" << std::endl;
	//	return -1;
	//}

	//if (!fs::create_directory(tempDocPath)){
	//	std::cout << "tmp directory can't create." << std::endl;
	//	return -1;
	//}
	//fs::path outputPath2 = demoPath.parent_path() / "demo.updated.docx";
	//wmk = new WordMarker(markOutputPath.string(), tempDocPath.string());
	//wmk->WaterMarkUpdate("AAAAAAAAAAAAAA");
	//wmk->SaveToFile(outputPath2.string());
	//delete wmk;


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
      <wp:docPr id="2" name="图片" hidden="1" title="klsjlfksj" descr="mark"/>
      <wp:cNvGraphicFramePr>
        <a:graphicFrameLocks xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main" noChangeAspect="1"/>
      </wp:cNvGraphicFramePr>
      <a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main">
        <a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture">
          <pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture">
            <pic:nvPicPr>
              <pic:cNvPr id="2" name="图片 1" descr="This is the description" hidden="1" title="This is the title"/>
              <pic:cNvPicPr/>
            </pic:nvPicPr>
            <pic:blipFill>
              <a:blip r:embed="rId6"/>
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

	xmlDocPtr paragraphDoc = xmlReadMemory(
		wparagraphMarkContent.data(), wparagraphMarkContent.length() + 1,
		NULL, NULL, 0);

	if (!paragraphDoc){
		std::cout << "XML INIT FROM STRING FAILED.." << std::endl;
		return -1;
	}

	fs::path documentPath = fs::current_path() / "demo-1/word/document.xml";
	assert(fs::exists(documentPath));

	xmlXPathObjectPtr xpathObj = NULL;
	XMLOperation *xmlOp = new XMLOperation(documentPath.string().data());

	// 找到document.xml内所有段落
	int foundSize = xmlOp->xmlXPathFindObjects(BAD_CAST "//w:p", xpathObj);
	if (foundSize == 0) {
		std::cout << "[AAA] can't find marked element.." << std::endl;
		return -1;
	}
	std::cout << "found node size :" << foundSize << std::endl;


	xmlNodeSetPtr nodes = xpathObj->nodesetval;
	xmlNodePtr firstNode = nodes->nodeTab[0];

	xmlNodePtr paragraphNode = xmlDocCopyNode(
		xmlDocGetRootElement(paragraphDoc),
		firstNode->doc,
		1);
	xmlFreeDoc(paragraphDoc);

	if (!paragraphNode){
		std::cout << "Copy node failed." << std::endl;
		return -1;
	}
	
	xmlNodePtr addedNode = xmlAddChildList(firstNode, paragraphNode->children);
	if (!addedNode) {
		xmlFreeNode(paragraphNode);
		return -1;
	}
	paragraphNode->children = NULL; // Thanks to milaniez from stackoverflow
	paragraphNode->last = NULL;     // for fixing
	
	fs::path outputPath = fs::current_path() / "demo-1/word/document.xml.bak";
	xmlOp->outputFile(outputPath.string());

	delete xmlOp;


	return 0;
}