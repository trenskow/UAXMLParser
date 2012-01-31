/*
 
 Created by Kristian Trenskow (The Famous Software Company)
 
 This source code is licensed under the BSD-License (simplified variant).
 Please see LICENSE for more info, or contact info@thefamoussoftwarecompany.com
 if you obtained this source without a LICENSE file.
 
 */

#include <string>

#include <assert.h>
#include <string.h>

#if !defined(MIN)
#define MIN(a, b) ((a < b) ? a : b)
#endif

#include "BoyerMoore.h"
#include "XMLNodeList.h"

#include "XMLNode.h"

char* mallocstrcpy(const char* src);

using namespace xml;

XMLNode::XMLNode() {

	_parentNode = NULL;
    _childNodes = NULL;
	_nodeName = _buffer = NULL;
	_subNodesCache = NULL;
	_attributesCacheCount = _subNodesCacheCount = _length = 0;

	_attributesCache = NULL;
	_innerValue = NULL;
	
}

XMLNode::XMLNode(char* buffer, size_t length, XMLNode* parentNode) {

	assert(buffer != NULL && length > 0);

	_buffer = buffer;
	_length = length;

	_parentNode = parentNode;
    _childNodes = NULL;

	_nodeName = NULL;
	_subNodesCache = NULL;
	_attributesCacheCount = _subNodesCacheCount = 0;
	
	_attributesCache = NULL;
	_innerValue = NULL;
	
}

XMLNode::~XMLNode() {

	// Data has been copied to the buffer by the ParseData function. Therefore it needs to be freed.
	if (_parentNode == NULL && _buffer != NULL)
		free(_buffer);
	
    if (_childNodes)
        delete _childNodes;
    
	if (_subNodesCache != NULL) {

		for (size_t x = 0 ; x < _subNodesCacheCount ; x++) {
			if (_subNodesCache[x]._name != NULL)
				free(_subNodesCache[x]._name);
			delete _subNodesCache[x]._nodeList;
		}

		free(_subNodesCache);

	}
	
	if (_attributesCache != NULL) {
		
		for (size_t x = 0 ; x < _attributesCacheCount ; x++) {
			free(_attributesCache[x]._name);
			free(_attributesCache[x]._value);
		}
		
		free(_attributesCache);
		
	}
	
	_attributesCache = NULL;
	
	// For debugging purposes, we release this last
	
	if (_nodeName != NULL)
		free(_nodeName);
	
	if (_innerValue != NULL)
		free(_innerValue);

}

bool XMLNode::ValidateXML(const char* buffer, size_t length) {

    size_t offset = 0;
    while (length - offset > 0 && buffer[offset] != '<')
        offset++;
    
	if (length - offset > 5)
		return memcmp(&buffer[offset], "<?xml", 5) == 0;

	return false;

}

void XMLNode::ParseData(const char* buffer, size_t length) {

	assert(_parentNode == NULL && _buffer == NULL && length > 0);
	
    size_t offset = 0;
    while (length - offset > 0 && buffer[offset] != '<')
        offset++;
    
	_buffer = (char*) malloc(sizeof(char) * (length - offset));
	memcpy(_buffer, &buffer[offset], length - offset);

	_length = length - offset;

}

XMLNode* XMLNode::ParentNode() {

	return _parentNode;

}

const char* XMLNode::GetName() {

	_parseNodeName();
	
	return _nodeName;
	
}

bool XMLNode::IsDocumentNode() {
    
    return (_parentNode == NULL);
    
}

XMLNodeList* XMLNode::ChildNodes() {
	
    if (_childNodes == NULL) {
    
        _childNodes = new XMLNodeList();
        
        if (_isEndNode(_buffer) || _isCDataNode())
            return _childNodes;
                
        char* tagstart = &_buffer[1];
        signed long long clength = (signed long long) _length - 1;
        
        while (clength > 0) {
            
            while (tagstart[0] != '<' && tagstart - _buffer < _length)
                tagstart = &tagstart[1];
            
            if (tagstart[0] != '<' || tagstart >= &_buffer[_length])
                break;
            
            clength = (signed long long)_length - (signed long long)(tagstart - _buffer);
            
            if (clength > 0 && tagstart[1] == '/') {
                tagstart = (char*) memchr(tagstart, '>', clength);
                clength = _length - (tagstart - _buffer);
                continue;
            }
            
            char tagname[512];
            _getTagName(tagstart, clength, tagname, 512);
            
            if (_isEndNode(tagstart)) {
                
                _childNodes->_pushNode(new XMLNode(tagstart, tagstart - strchr(tagstart, '>'), this));
                tagstart = (char*) memchr(tagstart, '>', clength);
                tagstart = &tagstart[1];
                
            } else {
                
                int taglength = strlen(tagname);
                
                char startagname[taglength + 2];
                char endtagname[taglength + 4];
                sprintf(startagname, "<%s", tagname);
                sprintf(endtagname, "</%s>", tagname);
                
                BoyerMoore* beginTag = new BoyerMoore(startagname, taglength + 1);
                BoyerMoore* endTag = new BoyerMoore(endtagname, taglength + 3);
                
                // Searching for an closing tag matching our opening tag			
                
                long bmEndRet[1];
                if (endTag->Search(&tagstart[taglength], clength - taglength, bmEndRet, 1, BM_NONEDSTEDRESULTS)) {
                    
                    // closing tag was found
                    
                    const char* cBeginSearchPos = &tagstart[taglength + 2];
                    const char* cEndSearchPos = &tagstart[taglength + bmEndRet[0]];
                    long bmBeginRet[1];
                    
                    // Search for opening tags in between.
                    
                    while (beginTag->Search(cBeginSearchPos, cEndSearchPos - cBeginSearchPos, bmBeginRet, 1, BM_NONEDSTEDRESULTS) > 0) {
                        
                        // Consider this:
                        //
                        // <tag> <-- Searching for this opening-tags corresponding closing tag
                        //    <tag> <-- this was found
                        //       Something
                        //    </tag> <-- so closing tag found above is this one
                        // </tag> <-- not this (which is what we're actually looking for)
                        //
                        // Keep searching...
                        //
                        // For every opening tag matching our tag we find, we need to find it's closing tag.
                        
                        // Check whether this opening tag is just another tag name that starts with the same as
                        // the one we are searching for.
                        
                        if (cBeginSearchPos[bmBeginRet[0] + taglength + 1] == ' ' ||
                            cBeginSearchPos[bmBeginRet[0] + taglength + 1] == '/' ||
                            cBeginSearchPos[bmBeginRet[0] + taglength + 1] == '>') {
                            
                            cBeginSearchPos = &cBeginSearchPos[bmBeginRet[0] + taglength + 1];
                            cEndSearchPos = &cEndSearchPos[taglength + 1];
                            if (endTag->Search(cEndSearchPos, clength - (cEndSearchPos - tagstart), bmEndRet, 1, BM_NONEDSTEDRESULTS) > 0)
                                cEndSearchPos = &cEndSearchPos[bmEndRet[0]];
                            else {
                                
                                // Unexpected end of document.
                                // Cleaning up and returning NULL!
                                
                                delete beginTag;
                                delete endTag;
                                delete _childNodes;
                                _childNodes = NULL;
                                
                                return NULL;
                                
                            }
                            
                        } else
                            cBeginSearchPos = &cBeginSearchPos[bmBeginRet[0] + taglength + 1];
                        
                    }
                    
                    cEndSearchPos = (const char*) memchr(cEndSearchPos, '>', clength - (cEndSearchPos - tagstart));
                    cEndSearchPos = &cEndSearchPos[1];
                    
                    size_t totaltaglength = cEndSearchPos - tagstart;
                    
                    _childNodes->_pushNode(new XMLNode(tagstart, totaltaglength, this));
                    
                    tagstart = &tagstart[totaltaglength];
                    clength = _length - (tagstart - _buffer);
                    
                }
                
                delete beginTag;
                delete endTag;
                
            }
            
        }
        
        // Add result to cache
        
    }

	return _childNodes; // Is released when cache is released upon deletion of this object

}

XMLNodeList* XMLNode::GetNodesWithTagName(const char* name) {
    
    XMLNodeList* childNodes = this->ChildNodes();
    
    if (name == NULL)
        return childNodes;
    
    // Find the requested tagname in our cache
    
    for (int i = 0 ; i < _subNodesCacheCount ; i++)
        if (strcmp(name, _subNodesCache[i]._name) == 0)
            return _subNodesCache[i]._nodeList;
    
    // Tagname was not found in cache. Creating new.
    
    XMLNodeList* ret = new XMLNodeList();
    ret->_refNodes = true;

    for (int i = 0 ; i < childNodes->Count() ; i++)
        if (strcmp(childNodes->NodeAtIndex(i)->GetName(), name) == 0) {
            ret->_pushNode(childNodes->NodeAtIndex(i));
        }
    
    _addNodeListToCache(ret, name);
    
    return ret;
    
}

const char* XMLNode::GetAttributeValue(const char* attributeName, const char* attributeNamespace) {

	assert(attributeName != NULL);
	
	size_t namelen = strlen(attributeName) + (attributeNamespace != NULL ? strlen(attributeNamespace) + 1: 0);
	char name[namelen + 1];

	snprintf(name, namelen + 1, "%s", attributeName);
	
	if (attributeNamespace != NULL)
		snprintf(name, namelen + 1, "%s:%s", attributeNamespace, attributeName);
	
	// Search cache
	for (int i = 0 ; i < _attributesCacheCount ; i++)
		if (0 == strcmp(_attributesCache[i]._name, name))
			return _attributesCache[i]._value;
	
	size_t taglength = (const char*) memchr(_buffer, '>', _length) - _buffer;

	char attname[namelen + 3];
	sprintf(attname, " %s=", name);
	
	char* ret = NULL;

	BoyerMoore* bm = new BoyerMoore(attname, namelen + 2);
	long bmret[1];

	if (bm->Search(_buffer, taglength, bmret, 1, BM_NONEDSTEDRESULTS)) {

		const char* start = &_buffer[bmret[0] + namelen + 3];
		const char* end = (const char*) memchr(start, '"', taglength - (start - _buffer));

		size_t length = end - start;

		_attributesCache = (attributesCache*) realloc(_attributesCache, sizeof(attributesCache) * (_attributesCacheCount + 1));
		
		_attributesCache[_attributesCacheCount]._name = (char*)malloc(sizeof(char) * strlen(name));
		strcpy(_attributesCache[_attributesCacheCount]._name, name);
		
		ret = _attributesCache[_attributesCacheCount]._value = (char*)malloc(sizeof(char) * (length + 1));
		memcpy(_attributesCache[_attributesCacheCount]._value, start, length);
		_attributesCache[_attributesCacheCount]._value[length] = '\0';
		
		_decode(_attributesCache[_attributesCacheCount]._value, length);

		_attributesCacheCount++;
		
	}

	delete bm;

	return ret;

}

char* mallocstrcpy(const char* src) {
	
	if (src != NULL) {
		
		char* ret = (char*) malloc(sizeof(char) * (strlen(src) + 1));
		strcpy(ret, src);
		return ret;
		
	}
	
	return NULL;
	
}

XMLNode& XMLNode::operator=(const XMLNode &rs) {

	// Reset this. We do this simply by calling destructor
	this->~XMLNode();

	this->_parentNode = rs._parentNode;
	this->_buffer = rs._buffer;
	this->_length = rs._length;
	
	// Right side object is a document object. We therefore copy it's buffer, but we do not copy it's caches
	// This object will bould it's own cache
	if (rs._parentNode == NULL && rs._buffer != NULL)
		memcpy(this->_buffer, rs._buffer, rs._length);
	
	this->_innerValue = mallocstrcpy(rs._innerValue);
	this->_nodeName = mallocstrcpy(rs._nodeName);

	this->_subNodesCache = NULL;
	this->_attributesCache = NULL;
	
	this->_subNodesCacheCount = this->_attributesCacheCount = 0;
	
	return *this;
	
}

const char* XMLNode::GetInnerValue() {
	
	if (_isEndNode(_buffer))
		return NULL;
	
	if (_innerValue != NULL)
		return _innerValue;

	const char* start = (const char*) memchr(_buffer, '>', _length);
	const char* end = &_buffer[_length];

	size_t cpos = _length;
	while (end != &start[1] && _buffer[cpos] != '<')
		end = &_buffer[cpos--];

	start = &start[1];
	end = &end[-1];

	size_t length = end - start;
    
    bool shouldDecode = true;
    if (_isCDataNode()) {
        start += 9;
        length -= 12;
        shouldDecode = false;
    }

	_innerValue = (char*)malloc(sizeof(char) * length + 1);
	
	memcpy(_innerValue, start, length);
	_innerValue[length] = '\0';
    if (shouldDecode)
        _decode(_innerValue, length);

	return _innerValue;

}

bool XMLNode::_isCDataNode() {
    
	const char* start = (const char*) memchr(_buffer, '>', _length);
	const char* end = &_buffer[_length];
    
	size_t cpos = _length;
	while (end != &start[1] && _buffer[cpos] != '<')
		end = &_buffer[cpos--];
    
	start = &start[1];
	end = &end[-1];
    
	size_t length = end - start;
    
    if (length > 9 && memcmp(start, "<![CDATA[", 9) == 0)
        return true;
    
    return false;
    
}

int XMLNode::_hexToDec(const char* num) {

	assert(num != NULL);

	int ret = 0;
	int cshift = 0;
	size_t len = strlen(num);

	for (int x = len - 1 ; x >= 0 ; x--) {

		char cChar = num[x];
		int cNum = 0;
		if (cChar >= 'A' && cChar <= 'F')
			cNum = 10 + (cChar - 'A');
		else if (cChar >= 'a' && cChar <= 'f')
			cNum = 10 + (cChar - 'a');
		else if (cChar >= '0' && cChar <= '9')
			cNum = cChar - '0';
		else
			return -1;


		ret = ret | (cNum << cshift);
		cshift += 4;

	}

	return ret;

}

void XMLNode::_decode(char* buffer, size_t length) {

	size_t x;

	for (x = 0 ; x < length ; x++) {

		if (buffer[x] == '&') {

			char* end = (char*) memchr(&buffer[x], ';', length - x);
			size_t codelen = end - &buffer[x] + 1;
			char code[codelen + 1];
			memcpy(code, &buffer[x], codelen);
			code[codelen] = '\0';

			length -= codelen - 1;

			if (codelen > 1 && code[1] == '#') {

				size_t ascpos = 2;
				bool isHex = false;
				if ((isHex = (code[ascpos] == 'x')))
					ascpos++;
				code[codelen - 1] = '\0';
				int asccode = 0;
				if (isHex)
					asccode = _hexToDec(&code[ascpos]);
				else
					sscanf(&code[ascpos], "%d", &asccode);
				buffer[x] = asccode;

			} else if (0 == strcmp(code, "&amp;"))
				buffer[x] = '&';
            else if (0 == strcmp(code, "&apos;"))
                buffer[x] = '\'';
			else if (0 == strcmp(code, "&quot;"))
				buffer[x] = '"';
			else if (0 == strcmp(code, "&gt;"))
				buffer[x] = '>';
			else if (0 == strcmp(code, "&lt;"))
				buffer[x] = '<';

			memcpy(&buffer[x + 1], &buffer[x + codelen], length - x);

		}

	}

	buffer[x] = '\0';

}

bool XMLNode::_isEndNode(const char* nodestart) {

	size_t end = (size_t) (strchr(nodestart, '>') - nodestart);

	if (end == 0 || (nodestart[end - 1] != '/' && nodestart[1] != '!'))
		return false;

	return true;

}

void XMLNode::_addNodeListToCache(XMLNodeList* list, const char* name) {
	
	_subNodesCache = (subNodesCache*) realloc(_subNodesCache, sizeof(subNodesCache) * (_subNodesCacheCount + 1));
	bzero(&_subNodesCache[_subNodesCacheCount], sizeof(subNodesCache));
	if (name != NULL) {
		_subNodesCache[_subNodesCacheCount]._name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
		strcpy(_subNodesCache[_subNodesCacheCount]._name, name);
	}
	_subNodesCache[_subNodesCacheCount]._nodeList = list;
	_subNodesCacheCount++;
	
}

/* This function is used in parsing */
void XMLNode::_getTagName(const char* buffer, size_t length, char* outbuffer, size_t outlength) {

	int x;
	for (x = 1 ; x < length ; x++)
		if (buffer[x] == ' ' || buffer[x] == '/' || buffer[x] == '>')
			break;

	memcpy(outbuffer, &buffer[1], MIN(x - 1, outlength - 1));
	outbuffer[MIN(x - 1, outlength - 1)] = '\0';

}

/* This function is used by GetNodeName and GetNamespaceName */
void XMLNode::_parseNodeName() {
	
	if (_nodeName == NULL) {
	
		char* start = &_buffer[1];
		size_t len;
		
		for (len = 0 ; len < _length ; len++)
			if (start[len] == ' ' || start[len] == '/' || start[len] == '>')
				break;
		
        // Comment next three lines when enabling namespace code!
        
        _nodeName = (char*)malloc(sizeof(char) * len + 1);
        memcpy(_nodeName, start, len);
        _nodeName[len] = '\0';
                 
	}
	
}
