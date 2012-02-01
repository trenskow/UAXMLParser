/*
 
 Created by Kristian Trenskow (The Famous Software Company)
 
 This source code is licensed under the BSD-License (simplified variant).
 Please see LICENSE for more info, or contact info@thefamoussoftwarecompany.com
 if you obtained this source without a LICENSE file.
 
 */

#define __UAXMLNODE_PRIVATE
#import "XMLNode.h"
#import "UAXMLNode.h"

@implementation UAXMLNode

- (id)initWithNode:(xml::XMLNode *)theNode {
    
    if (!theNode)
        return nil;
    
    if ((self = [super init])) {
        node = theNode;
        attributesCache = [[NSMutableDictionary alloc] init];
    }
    
    return self;
    
}

- (void)dealloc {

#if !defined(_ARC_ENABLED)
    [nodeName release];
    [attributesCache release];

    [super dealloc];
#endif
    
}

- (UAXMLNode*)parentNode {
    
    return [[[[self class] alloc] initWithNode:node->ParentNode()] arcSafeAutorelease];
    
}

- (BOOL)isDocumentNode {
    
    return node->IsDocumentNode();
    
}

- (NSString*)name {
    
    if (!nodeName)
        nodeName = [[NSString alloc] initWithCString:node->GetName() encoding:NSUTF8StringEncoding];
    
    return nodeName;
    
}

- (NSArray*)nodeListToArray:(xml::XMLNodeList*)nodeList {
    
    NSMutableArray* ret = [NSMutableArray arrayWithCapacity:nodeList->Count()];
    
    for (NSUInteger i = 0 ; i < nodeList->Count() ; i++)
        [ret addObject:[[[[self class] alloc] initWithNode:nodeList->NodeAtIndex(i)] arcSafeAutorelease]];
    
    return ret;
    
}

- (NSArray*)childNodes {
    
    return [self nodeListToArray:node->ChildNodes()];
    
}

- (NSArray*)nodesWithTagName:(NSString *)tagname {

    return [self nodeListToArray:node->GetNodesWithTagName([tagname cStringUsingEncoding:NSUTF8StringEncoding])];
    
}

- (NSString*)innerValue {
    
    if (node->GetInnerValue())
        return [NSString stringWithCString:node->GetInnerValue() encoding:NSUTF8StringEncoding];
    
    return nil;
    
}

- (NSString*)valueOfAttributeWithName:(NSString *)attributeName {
    
    NSString* ret = nil;
    if ((ret = [attributesCache objectForKey:attributeName]))
        return ret;
    
    const char* attributeValue = node->GetAttributeValue([attributeName cStringUsingEncoding:NSUTF8StringEncoding]);
    
    if (attributeValue) {
        ret = [NSString stringWithCString:attributeValue encoding:NSUTF8StringEncoding];
        [attributesCache setObject:ret forKey:attributeName];
    }
    
    return ret;
    
}

- (BOOL)isEqual:(id)object {
    
    if ([object isKindOfClass:[UAXMLNode class]])
        return (node == ((UAXMLNode*)object)->node);
    
    return [super isEqual:object];
    
}

@end
