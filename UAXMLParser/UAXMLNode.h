//
//  UAXMLNode.h
//  AU Find
//
//  Created by Kristian Trenskow on 3/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#ifdef __UAXMLPARSER_PRIVATE
#define __UAXMLNODE_PRIVATE
#endif

#ifdef __UAXMLNODE_PRIVATE
#include "XMLNode.h"
#endif

@interface UAXMLNode : NSObject
#ifdef __UAXMLNODE_PRIVATE
{
    
    xml::XMLNode* node;
    NSMutableDictionary* attributesCache;
    NSString* nodeName;
    
}

- (id)initWithNode:(xml::XMLNode*)theNode;

#endif

@property (readonly) UAXMLNode* parentNode;
@property (readonly) BOOL isDocumentNode;
@property (readonly) NSString* name;
@property (readonly) NSArray* childNodes;
@property (readonly) NSString* innerValue;

- (NSArray*)nodesWithTagName:(NSString*)tagname;
- (NSString*)valueOfAttributeWithName:(NSString*)attributeName;

@end
