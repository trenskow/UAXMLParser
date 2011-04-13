//
//  UAXMLParser.mm
//  AU Find
//
//  Created by Kristian Trenskow on 3/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "XMLNode.h"
#import "XMLNodeList.h"

#define __UAXMLPARSER_PRIVATE
#import "UAXMLParser.h"

@implementation UAXMLParser

@synthesize currentNode, url;

- (id)initWithContentOfURL:(NSURL *)contentUrl {
    
    if ((self = [super init])) {
        
        //Used for debugging purposes
        //NSLog(@"%@", [contentUrl absoluteString]);
        NSData* data = [[NSData alloc] initWithContentsOfURL:contentUrl];
        if (data && xml::XMLNode::ValidateXML((const char*)[data bytes], [data length])) {
            url = [contentUrl copy];
            documentNode = new xml::XMLNode();
            documentNode->ParseData((const char*)[data bytes], [data length]);
            [data release];
            
            delegates = [[NSMutableArray alloc] init];
            
        } else
            return nil;
    }
    
    return self;
    
}

- (void)dealloc {
    
    delete documentNode;
    [url release];
    [delegates release];
    
    [super dealloc];
}

- (void)internalParseNode:(xml::XMLNode*)node {
    
    for (NSUInteger i = 0 ; i < node->ChildNodes()->Count() ; i++) {
        
        UAXMLNode* oldCurrentNode = [currentNode retain];
        currentNode = [[UAXMLNode alloc] initWithNode:node->ChildNodes()->NodeAtIndex(i)];
        
        id<UAXMLParserDelegate> delegate = [delegates lastObject];
        if (delegate && [(id)delegate conformsToProtocol:@protocol(UAXMLParserDelegate)])
            [delegate parser:self didBeginNode:currentNode];
        
        if (!ignoreChildNodes)
            [self internalParseNode:node->ChildNodes()->NodeAtIndex(i)];
        ignoreChildNodes = NO;
        
        delegate = [delegates lastObject];
        if (delegate && [(id)delegate conformsToProtocol:@protocol(UAXMLParserDelegate)] && [(id)delegate respondsToSelector:@selector(parser:didEndNode:)])
            [delegate parser:self didEndNode:currentNode];
        
        [currentNode autorelease];
        currentNode = oldCurrentNode;
        [oldCurrentNode release];
        
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
