/*
 
 Created by Kristian Trenskow (The Famous Software Company)
 
 This source code is licensed under the BSD-License (simplified variant).
 Please see LICENSE for more info, or contact info@thefamoussoftwarecompany.com
 if you obtained this source without a LICENSE file.
 
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
