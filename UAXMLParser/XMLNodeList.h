/*
 *  XMLNodeList.h
 *  nntptest
 *
 *  Created by Kristian Trenskow on 6/20/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "XMLNode.h"

#ifndef __XMLNODELIST_H
#define __XMLNODELIST_H

#include <stdio.h>

namespace xml {

	class XMLNode;

	class XMLNodeList {
		
		friend class XMLNode;

	public:
		~XMLNodeList();
		
		size_t Count();
		XMLNode* NodeAtIndex(size_t index);
		
		XMLNode* operator[](size_t index);

	private:
		XMLNodeList();
		
        bool _refNodes;
		size_t _nodeCount;
		XMLNode** _nodes;
		
		void _pushNode(XMLNode* node);
		
	};

}
	
#endif
