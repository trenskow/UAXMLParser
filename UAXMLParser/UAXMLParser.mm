/*
 
 Created by Kristian Trenskow (The Famous Software Company)
 
 This source code is licensed under the BSD-License (simplified variant).
 Please see LICENSE for more info, or contact info@thefamoussoftwarecompany.com
 if you obtained this source without a LICENSE file.
 
 */

#import "XMLNode.h"
#import "XMLNodeList.h"

#define __UAXMLPARSER_PRIVATE
#import "UAXMLParser.h"

@implementation UAXMLParser

@synthesize currentNode, url, automaticallyPopsDelegatesOffStack;

- (id)initWithContentOfURL:(NSURL *)contentUrl {
    
    if ((self = [super init])) {
        
        //Used for debugging purposes
        //NSLog(@"%@", [contentUrl absoluteString]);
        NSData* data = [[NSData alloc] initWithContentsOfURL:contentUrl];
        if (data && xml::XMLNode::ValidateXML((const char*)[data bytes], [data length])) {
            url = [contentUrl copy];
            documentNode = new xml::XMLNode();
            documentNode->ParseData((const char*)[data bytes], [data length]);
            [data arcSafeRelease];
            delegates = [[NSMutableArray alloc] init];
            automaticallyPopsDelegatesOffStack = YES;
            
        } else
            return nil;
    }
    
    return self;
    
}

- (void)dealloc {
    
    delete documentNode;
#if !defined(_ARC_ENABLED)
    [wrapperDocumentNode release];
    [url release];
    [delegates release];
    
    [super dealloc];
#endif
    
}

- (void)internalParseNode:(xml::XMLNode*)node {
    
    for (NSUInteger i = 0 ; i < node->ChildNodes()->Count() ; i++) {
        
        UAXMLNode* oldCurrentNode = [currentNode arcSafeRetain];
        currentNode = [[UAXMLNode alloc] initWithNode:node->ChildNodes()->NodeAtIndex(i)];
        
        id<UAXMLParserDelegate> delegate = [delegates lastObject];
        if (delegate && [(id)delegate conformsToProtocol:@protocol(UAXMLParserDelegate)])
            [delegate parser:self didBeginNode:currentNode];
        
        BOOL pop = (automaticallyPopsDelegatesOffStack && [delegates lastObject] != delegate);
        
        if (!ignoreChildNodes)
            [self internalParseNode:node->ChildNodes()->NodeAtIndex(i)];
        ignoreChildNodes = NO;
        
        if (pop)
            [self popDelegate:[delegates lastObject]];
        
        delegate = [delegates lastObject];
        if (delegate && [(id)delegate conformsToProtocol:@protocol(UAXMLParserDelegate)] && [(id)delegate respondsToSelector:@selector(parser:didEndNode:)])
            [delegate parser:self didEndNode:currentNode];
        
        [currentNode arcSafeAutorelease];
        currentNode = oldCurrentNode;
        [currentNode arcSafeRelease];
        
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
    
    if (!wrapperDocumentNode)
        wrapperDocumentNode = [[UAXMLNode alloc] initWithNode:documentNode];
    
    return [[wrapperDocumentNode arcSafeRetain] arcSafeAutorelease];
    
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
