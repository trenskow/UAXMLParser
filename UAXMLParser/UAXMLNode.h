/*
 
 Created by Kristian Trenskow (The Famous Software Company)
 
 This source code is licensed under the BSD-License (simplified variant).
 Please see LICENSE for more info, or contact info@thefamoussoftwarecompany.com
 if you obtained this source without a LICENSE file.
 
 */

#import "UAARCSafeObject.h"
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
