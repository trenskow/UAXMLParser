/*
 *  XMLNodeList.cpp
 *  nntptest
 *
 *  Created by Kristian Trenskow on 6/20/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <assert.h>

#include "XMLNodeList.h"

using namespace xml;

XMLNodeList::XMLNodeList() {

	_nodeCount = 0;
	_nodes = NULL;
    _refNodes = false;
	
}

XMLNodeList::~XMLNodeList() {

	if (_nodes != NULL) {
		
        if (!_refNodes) {
            for (size_t x = 0 ; x < _nodeCount ; x++)
                delete _nodes[x];
        }
	
		free(_nodes);
		
	}

}

size_t XMLNodeList::Count() {

	return _nodeCount;
	
}

XMLNode* XMLNodeList::NodeAtIndex(size_t index) {
	
	assert(index >= 0 && index < _nodeCount);
	
	return _nodes[index];
	
}

XMLNode* XMLNodeList::operator[](size_t index) {

	assert(index >= 0 && index < _nodeCount);
	
	return _nodes[index];
	
}

void XMLNodeList::_pushNode(XMLNode* node) {
	
	_nodes = (XMLNode**) realloc(_nodes, sizeof(XMLNode*) * (_nodeCount + 1));
	_nodes[_nodeCount++] = node;
	
}
