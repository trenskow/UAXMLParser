//
//  UAXMLNode.mm
//  AU Find
//
//  Created by Kristian Trenskow on 3/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

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
    [nodeName release];
    [attributesCache release];
    [super dealloc];
}

- (UAXMLNode*)parentNode {
    
    return [[[[self class] alloc] initWithNode:node->ParentNode()] autorelease];
    
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
        [ret addObject:[[[[self class] alloc] initWithNode:nodeList->NodeAtIndex(i)] autorelease]];
    
    return ret;
    
}

- (NSArray*)childNodes {
    
    return [self nodeListToArray:node->ChildNodes()];
    
}

- (NSArray*)nodesWithTagName:(NSString *)tagname {

    return [self nodeListToArray:node->GetNodesWithTagName([tagname cStringUsingEncoding:NSUTF8StringEncoding])];
    
}

- (NSString*)innerValue {
    
    return [NSString stringWithCString:node->GetInnerValue() encoding:NSUTF8StringEncoding];
    
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

@end
