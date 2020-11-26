#pragma once

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "../Util/debug_printf.h"
#include "../Util/StringHelper.h"


#if defined(LIBXML_XPATH_ENABLED) && defined(LIBXML_SAX1_ENABLED)


class XMLOperation {

public:
	XMLOperation(const char *content, unsigned int length) {

		if (xmlParse(content, length)) {
			
			// 获取所有根节点的namespace信息
			xmlGetNamespace();

			// 注册所有获得到的根节点namespace
			xmlXPathRegisterNamespace();
		}
		else {
			throw std::runtime_error("xml parse error.");
		}
	};

	XMLOperation(const char* filepath) {
		if (xmlParse(filepath)) {

			// 获取所有根节点的namespace信息
			xmlGetNamespace();

			// 注册所有获得到的根节点namespace
			xmlXPathRegisterNamespace();
		}
		else {
			throw std::runtime_error("xml parse error.");
		}
	}

	~XMLOperation() {
		// clean up
		if (this->xpathObj) {
			xmlXPathFreeObject(xpathObj);
		}

		if (this->xpathCtx) {
			xmlXPathFreeContext(xpathCtx);
		}

		if (this->doc) {
			xmlFreeDoc(doc);
		}
	};

	// 注册当前获取到的namespace
	void xmlXPathRegisterNamespace() {

		std::map< const xmlChar*, const xmlChar* >::iterator it;
		for (it = nsmap.begin(); it != nsmap.end(); it++) {
			const xmlChar* prefix = it->first;
			const xmlChar* href = it->second;
			//SPDLOG_INFO("key {}, value {}", prefix, href);

			/* do register namespace */
			if (xmlXPathRegisterNs(xpathCtx, prefix, href) != 0) {
				//auto msg = boost::format("Error: unable to register NS with prefix=\"%s\" and href=\"%s\"") % prefix % href;
				//BOOST_LOG_TRIVIAL(info) << msg.str();

				printfTrace("Error: unable to register NS with prefix=\"%s\" and href=\"%s\"", prefix, href);
				return;
			}
		}
	}

	// 注册外部给的namespace列表
	// 外部传入参数形式为 "prefix=href"
	void xmlXPathRegisterNamespace(std::vector<std::string> &vec) {

		if (vec.empty()) {
			printfTrace("register namespace must not empty!");
			return;
		}

		auto stringSplit = [](std::string s, std::string delimiter){
			size_t pos_start = 0, pos_end, delim_len = delimiter.length();
			std::string token;
			std::vector<std::string> res;

			while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
				token = s.substr(pos_start, pos_end - pos_start);
				pos_start = pos_end + delim_len;
				res.push_back(token);
			}

			res.push_back(s.substr(pos_start));
			return res;
		};

		for (auto value : vec) {
			auto v = stringSplit(value, "=");

			std::string href = v[1];
			std::string prefix = v[0];

			/* do register namespace */
			if (xmlXPathRegisterNs(xpathCtx, (const xmlChar*)prefix.data(),(const xmlChar*)href.data()) != 0) {
				//auto msg = boost::format("Error: unable to register NS with prefix=\"%s\" and href=\"%s\"") % prefix % href;
				//BOOST_LOG_TRIVIAL(error) << msg.str();

				printfTrace("Error: unable to register NS with prefix=\"%s\" and href=\"%s\"", prefix, href);
				return ;
			}
		}
	}


	unsigned int xmlXPathFindObjects(const xmlChar* xpathExpr) {

		unsigned int size = 0;

		xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
		if (xpathObj == NULL) {
			//auto msg = boost::format("Error: unable to evaluate xpath expression \"%s\"") % xpathExpr;
			//BOOST_LOG_TRIVIAL(error) << msg.str();
			printfTrace("Error: unable to evaluate xpath expression \"%s\"", xpathExpr);

			xmlXPathFreeContext(xpathCtx);
			// xmlFreeDoc(doc);
			size = 0;
			return size;
		}
		xmlNodeSetPtr nodes = xpathObj->nodesetval;
		size = (nodes) ? nodes->nodeNr : 0;

		return size;
	}

	// 通过xpath查找某一节点下元素
	unsigned int xmlXPathFindObjects(xmlNodePtr node, const xmlChar* xpathExpr, xmlXPathObjectPtr& xpathParamObj) {
		unsigned int size = 0;

		xpathObj = xmlXPathNodeEval(node, xpathExpr, xpathCtx);
		if (xpathObj == NULL) {
			//auto msg = boost::format("Error: unable to evaluate xpath expression \"{%s}\"") % xpathExpr;
			//BOOST_LOG_TRIVIAL(error) << msg.str();
			printfTrace("Error: unable to evaluate xpath expression \"{%s}\"", xpathExpr);

			xmlXPathFreeContext(xpathCtx);
			// xmlFreeDoc(doc);
			size = 0;
			return size;
		}

		xmlNodeSetPtr nodes = xpathObj->nodesetval;
		size = (nodes) ? nodes->nodeNr : 0;
		xpathParamObj = this->xpathObj;

		return size;
	}

	// xpath find elements
	unsigned int xmlXPathFindObjects(const xmlChar* xpathExpr, xmlXPathObjectPtr &xpathParamObj) {

		unsigned int size = 0;

		xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
		if (xpathObj == NULL) {
			//auto msg = boost::format("Error: unable to evaluate xpath expression \"{%s}\"") % xpathExpr;
			//BOOST_LOG_TRIVIAL(error) << msg.str();
			printfTrace("Error: unable to evaluate xpath expression \"{%s}\"", xpathExpr);

			xmlXPathFreeContext(xpathCtx);
			// xmlFreeDoc(doc);
			size = 0;
			return size;
		}
		xmlNodeSetPtr nodes = xpathObj->nodesetval;
		size = (nodes) ? nodes->nodeNr : 0;

		xpathParamObj = this->xpathObj;

		return size;
	}

	unsigned int xmlXPathFindObjects(const xmlChar* xpathExpr, int &size) {
		
		xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
		if (xpathObj == NULL) {

			//auto msg = boost::format("Error: unable to evaluate xpath expression \"{%s}\"") % xpathExpr;
			//BOOST_LOG_TRIVIAL(error) << msg.str();
			printfTrace("Error: unable to evaluate xpath expression \"{%s}\"", xpathExpr);

			xmlXPathFreeContext(xpathCtx);
			// xmlFreeDoc(doc);
			size = 0;
			return false;
		}

		xmlNodeSetPtr nodes = xpathObj->nodesetval;
		size = (nodes) ? nodes->nodeNr : 0;

		return true;
	}

	/* get xml root element */
	xmlNodePtr xmlGetRootNode() {
		return xmlDocGetRootElement(doc);
	}

	// 获取根节点的namespace
	void xmlGetNamespace() {
		xmlNodePtr root = xmlGetRootNode();

		for (xmlNs* node = root->nsDef; node != NULL; node = node->next) {
			/*printf("%s\n", node->href);*/
			if (node->prefix) {
				nsmap[node->prefix] = node->href;
				nsobjs[node->prefix] = (xmlNsPtr)node;
			}
			else {
				nsmap[BAD_CAST "null"] = node->href;
				nsobjs[BAD_CAST "null"] = (xmlNsPtr)node;
			}
		}

		//std::map< const xmlChar*, const xmlChar* >::iterator it;
		//for (it = nsmap.begin(); it != nsmap.end(); it++) {
		//	const xmlChar* key = it->first;
		//	const xmlChar* value = it->second;
		//	SPDLOG_INFO("XXX key {}, value {}", key, value);
		//}

		return;
	}

	// 获取传入节点的namespace, 并更新数据
	void xmlGetNamespace(xmlNodePtr curElem) {

		for (xmlNs* node = curElem->nsDef; node != NULL; node = node->next) {
			/*printf("%s\n", node->href);*/
			nsmap[node->prefix] = node->href;
			nsobjs[node->prefix] = (xmlNsPtr)node;
		}

		//std::map< const xmlChar*, const xmlChar* >::iterator it;
		//for (it = nsmap.begin(); it != nsmap.end(); it++) {
		//	const xmlChar* key = it->first;
		//	const xmlChar* value = it->second;
		//	SPDLOG_INFO("YYY key {}, value {}", key, value);
		//}

		return;
	}

	/* print xpath founded node element, debug. */
	void xmlPrintXPathResults(FILE* output=stdout) {

		xmlNodePtr cur;
		int size;
		int i;

		xmlNodeSetPtr nodes = xpathObj->nodesetval;

		assert(output);
		size = (nodes) ? nodes->nodeNr : 0;

		fprintf(output, "Result (%d nodes):\n", size);
		for (i = 0; i < size; ++i) {
			assert(nodes->nodeTab[i]);

			if (nodes->nodeTab[i]->type == XML_NAMESPACE_DECL) {
				xmlNsPtr ns;

				ns = (xmlNsPtr)nodes->nodeTab[i];
				cur = (xmlNodePtr)ns->next;
				if (cur->ns) {
					fprintf(output, "= namespace \"%s\"=\"%s\" for node %s:%s\n",
						ns->prefix, ns->href, cur->ns->href, cur->name);
				}
				else {
					fprintf(output, "= namespace \"%s\"=\"%s\" for node %s\n",
						ns->prefix, ns->href, cur->name);
				}
			}
			else if (nodes->nodeTab[i]->type == XML_ELEMENT_NODE) {
				cur = nodes->nodeTab[i];
				if (cur->ns) {
					fprintf(output, "= element node \"%s:%s\"\n",
						cur->ns->href, cur->name);
				}
				else {
					fprintf(output, "= element node \"%s\"\n",
						cur->name);
				}
			}
			else {
				cur = nodes->nodeTab[i];
				fprintf(output, "= node \"%s\": type %d\n", cur->name, cur->type);
			}
		}
	}

	//通过名称获取xml namespace对象地址
	xmlNsPtr xmlGetNsObject(const xmlChar* prefix) {
		auto it = nsobjs.find(prefix);
		if (it == nsobjs.end()) {
			return NULL;
		}

		return it->second;
	}

	// 格式化打印xml
	void xmlDocDebugDump() {
		xmlDocDump(stdout, doc);
	}

	// xml导出到文件
	void outputFile(std::string filepath) {
		xmlSaveFile(filepath.data(), doc);
	}

	

protected:

	// read xml from memory
	bool xmlParse(const char* content, unsigned int length) {

		doc = xmlReadMemory(content, length, "Document.xml", NULL, 0);
		if (doc == NULL) {
			// BOOST_LOG_TRIVIAL(error) << "read xml memory error.";
			printfTrace("read xml memory error.");
			return false;
		}


		/* Create xpath evaluation context */
		xpathCtx = xmlXPathNewContext(doc);
		if (xpathCtx == NULL) {
			// BOOST_LOG_TRIVIAL(error) << "unable to create new XPath context";
			printfTrace("unable to create new XPath context");
			return false;
		}

		return true;
	}

	// read xml from file
	bool xmlParse(const char* filepath) {
		/* Load XML document */
		doc = xmlParseFile(filepath);
		if (doc == NULL) {
			//auto msg = boost::format("unable to parse file \"{%s}\"") % filepath;
			//BOOST_LOG_TRIVIAL(error) << msg.str();
			printfTrace("unable to parse file \"{%s}\"", filepath);
			return false;
		}

		/* Create xpath evaluation context */
		xpathCtx = xmlXPathNewContext(doc);
		if (xpathCtx == NULL) {
			//auto msg = boost::format("unable to create new XPath context");
			//BOOST_LOG_TRIVIAL(error) << msg.str();
			printfTrace("unable to create new XPath context\n");

			return false;
		}

		return true;
	}

private:
	xmlDocPtr			doc = 0;
	xmlChar*			xpathExpr = 0;

	xmlXPathContextPtr	xpathCtx = 0;
	xmlXPathObjectPtr	xpathObj = 0;


	std::string		encoding = "utf-8";

	// std::map<std::string, std::string > nsmap;
	std::map<const xmlChar*, const xmlChar*> nsmap;
	std::map<const xmlChar*, xmlNsPtr > nsobjs;
	
};

//----------------------- cut line -----------------------------------
//#include <cstdio>
//#include <iostream>
//#include <libxml/tree.h>
//
//#include "WordXml.h"
//
//int main(int argc, char** argv) {
//
//	auto wordDoc = WordXml("D:\\c++dev\\libxml2example\\Debug\\document.xml");
//
//
//	std::vector<std::string> vec;
//	vec.push_back("w=http://schemas.openxmlformats.org/wordprocessingml/2006/main");
//	wordDoc.xmlXPathRegisterNamespace(vec);
//
//
//	wordDoc.xmlXPathFindObjects((const xmlChar*)"//w:body");
//	wordDoc.xmlPrintXPathResults();
//	wordDoc.xmlGetNamespace();
//
//	return 0;
//}

#endif


