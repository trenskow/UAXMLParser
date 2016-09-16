/*
 
 Created by Kristian Trenskow (The Famous Software Company)
 
 This source code is licensed under the BSD-License (simplified variant).
 Please see LICENSE for more info, or contact info@thefamoussoftwarecompany.com
 if you obtained this source without a LICENSE file.
 
 */

#import "XMLNode.h"
#import "XMLNodeList.h"

#import "UAXMLNode+Internal.h"

#import "UAXMLParser.h"

@interface UAXMLParser () {
    
    xml::XMLNode* documentNode;
    NSMutableArray* delegates;
    BOOL ignoreChildNodes;
    
}

@property (nonatomic,readwrite) UAXMLNode* currentNode;
@property (nonatomic,readwrite) NSURL* url;

@end

@implementation UAXMLParser

- (id)initWithData:(NSData*)data {
    
    if ((self = [super init])) {
        
        if (data && xml::XMLNode::ValidateXML((const char*)[data bytes], [data length])) {
            documentNode = new xml::XMLNode();
            documentNode->ParseData((const char*)[data bytes], [data length]);
            delegates = [@{} mutableCopy];
            _automaticallyPopsDelegatesOffStack = YES;
            
        } else
            return nil;
    }
    
    return self;
    
}

- (id)initWithContentOfURL:(NSURL *)contentUrl {

    if ((self = [self initWithData:[[NSData alloc] initWithContentsOfURL:contentUrl]])) {
        self.url = [contentUrl copy];
    }
    
    return self;

    
}

- (void)dealloc {
    
    delete documentNode;
    
}

- (void)internalParseNode:(xml::XMLNode*)node {
    
    for (NSUInteger i = 0 ; i < node->ChildNodes()->Count() ; i++) {
        
        UAXMLNode* oldCurrentNode = self.currentNode;
        self.currentNode = [[UAXMLNode alloc] initWithNode:node->ChildNodes()->NodeAtIndex(i)];
        
        id<UAXMLParserDelegate> delegate = [delegates lastObject];
        if (delegate && [(id)delegate conformsToProtocol:@protocol(UAXMLParserDelegate)])
            [delegate parser:self didBeginNode:self.currentNode];
        
        BOOL pop = (self.automaticallyPopsDelegatesOffStack && [delegates lastObject] != delegate);
        
        if (!ignoreChildNodes)
            [self internalParseNode:node->ChildNodes()->NodeAtIndex(i)];
        ignoreChildNodes = NO;
        
        if (pop)
            [self popDelegate:[delegates lastObject]];
        
        delegate = [delegates lastObject];
        if (delegate && [(id)delegate conformsToProtocol:@protocol(UAXMLParserDelegate)] && [(id)delegate respondsToSelector:@selector(parser:didEndNode:)])
            [delegate parser:self didEndNode:self.currentNode];
        
        self.currentNode = oldCurrentNode;
        
    }
    
}

- (void)parse {
    
    BOOL shouldBegin = YES;
    if ([delegates lastObject] && [[delegates lastObject] respondsToSelector:@selector(parserWillBeginParse:)])
        shouldBegin = [(id<UAXMLParserDelegate>)[delegates lastObject] parserWillBeginParse:self];

    if (shouldBegin) {

        [self internalParseNode:documentNode];

        if ([delegates lastObject] && [[delegates lastObject] respondsToSelector:@selector(parserDidEndParse:)])
            [(id<UAXMLParserDelegate>)[delegates lastObject] parserDidEndParse:self];
        
    }
    
}

- (UAXMLNode*)documentNode {
    
    return [[UAXMLNode alloc] initWithNode:documentNode];
    
}

- (void)ignoreChildNodes {
    
    ignoreChildNodes = YES;
    
}

- (void)pushDelegate:(id)delegate {
    
    if ([delegates lastObject] != delegate)
        [delegates addObject:delegate];
    
}

- (void)popDelegate:(id)delegate {
    
    [delegates removeObject:delegate];
    
}

@end
