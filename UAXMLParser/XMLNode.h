/*
 *  XMLNode.h
 *  nntptest
 *
 *  Created by Kristian Trenskow on 6/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __XMLNODE_H
#define __XMLNODE_H

#include <stdio.h>

#include "XMLNodeList.h"

namespace xml {

	class XMLNodeList;
	
	class XMLNode {

	public:
		XMLNode();
		~XMLNode();
		
		void ParseData(const char* buffer, size_t length);
		
		XMLNode* ParentNode();

		const char* GetName();
//		const char* GetNamespace();
		
        XMLNodeList* ChildNodes();
		XMLNodeList* GetNodesWithTagName(const char* name);
		
		const char* GetAttributeValue(const char* attributeName, const char* attributeNamespace = NULL);
		const char* GetInnerValue();
        
        bool IsDocumentNode();
		
		static bool ValidateXML(const char* buffer, size_t length);
		
		XMLNode& operator=(const XMLNode &rs);
		
	private:
		
		XMLNode(char* buffer, size_t length, XMLNode* parentNode = NULL);

		char* _buffer;
		size_t _length;
		
		XMLNode* _parentNode;
		char* _nodeName;
		char* _nodeNamespace;
        
        XMLNodeList* _childNodes;

		struct subNodesCache {
			char* _name;
			XMLNodeList* _nodeList;
		} *_subNodesCache;
		size_t _subNodesCacheCount;
		
		char* _innerValue;
		
		struct attributesCache {
			char* _name;
			char* _value;
		} *_attributesCache;
		size_t _attributesCacheCount;
		
		int _hexToDec(const char* num);
		void _decode(char* buffer, size_t length);
		bool _isEndNode(const char* nodestart);
		void _addNodeListToCache(XMLNodeList* list, const char* name);
		void _getTagName(const char* buffer, size_t length, char* outbuffer, size_t outlength);
		
		void _getAttributeValueStartAndEnd(const char* attributeName, const char** start, const char** end);
		void _getInnerValueStartAndEnd(const char** start, const char** end);

		void _parseNodeName();
		
	protected:
		void SetNextNode(XMLNode* node);
		void SetPreviousNode(XMLNode* node);
		
	};

}
	
#endif
