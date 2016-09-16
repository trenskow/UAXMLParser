/*
 
 Created by Kristian Trenskow (The Famous Software Company)
 
 This source code is licensed under the BSD-License (simplified variant).
 Please see LICENSE for more info, or contact info@thefamoussoftwarecompany.com
 if you obtained this source without a LICENSE file.
 
 */

#import "XMLNode.h"
#import "UAXMLNode+Internal.h"

@interface UAXMLNode ()

@property (nonatomic) xml::XMLNode* node;
@property (nonatomic) NSMutableDictionary* attributesCache;
@property (nonatomic,readwrite) NSString* name;
@property (nonatomic,readwrite) NSArray* childNodes;
@property (nonatomic,readwrite) NSString* innerValue;

@end

@implementation UAXMLNode

- (id)initWithNode:(xml::XMLNode *)node {
    
    if (!node)
        return nil;
    
    if (_node->GetCtx() != NULL) {
        return (__bridge UAXMLNode *)_node->GetCtx();
    }
    
    if ((self = [super init])) {
        _node = node;
        node->SetCtx((__bridge void *)self);
        _attributesCache = [@{} mutableCopy];
    }
    
    return self;
    
}

- (UAXMLNode*)parentNode {
    
    return [[[self class] alloc] initWithNode:self.node->ParentNode()];
    
}

- (BOOL)isDocumentNode {
    
    return self.node->IsDocumentNode();
    
}

- (NSString*)name {
    
    if (!_name) {
        _name = [[NSString alloc] initWithCString:self.node->GetName() encoding:NSUTF8StringEncoding];
    }
    
    return _name;
    
}

- (NSArray*)nodeListToArray:(xml::XMLNodeList*)nodeList {
    
    NSMutableArray* ret = [NSMutableArray arrayWithCapacity:nodeList->Count()];
    
    for (NSUInteger i = 0 ; i < nodeList->Count() ; i++)
        [ret addObject:[[[self class] alloc] initWithNode:nodeList->NodeAtIndex(i)]];
    
    return ret;
    
}

- (NSArray*)childNodes {
    
    return [self nodeListToArray:self.node->ChildNodes()];
    
}

- (NSArray*)nodesWithTagName:(NSString *)tagname {

    return [self nodeListToArray:self.node->GetNodesWithTagName([tagname cStringUsingEncoding:NSUTF8StringEncoding])];
    
}

- (NSString*)innerValue {
    
    if (self.node->GetInnerValue())
        return [NSString stringWithCString:self.node->GetInnerValue() encoding:NSUTF8StringEncoding];
    
    return nil;
    
}

- (NSString*)valueOfAttributeWithName:(NSString *)attributeName {
    
    NSString* ret = nil;
    if ((ret = [self.attributesCache objectForKey:attributeName]))
        return ret;
    
    const char* attributeValue = self.node->GetAttributeValue([attributeName cStringUsingEncoding:NSUTF8StringEncoding]);
    
    if (attributeValue) {
        ret = [NSString stringWithCString:attributeValue encoding:NSUTF8StringEncoding];
        [self.attributesCache setObject:ret forKey:attributeName];
    }
    
    return ret;
    
}

- (BOOL)isEqual:(id)object {
    
    if ([object isKindOfClass:[UAXMLNode class]])
        return (self.node == ((UAXMLNode*)object).node);
    
    return [super isEqual:object];
    
}

@end
