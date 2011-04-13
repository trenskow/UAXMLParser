//
//  UAXMLParser.h
//  AU Find
//
//  Created by Kristian Trenskow on 3/16/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#ifdef __UAXMLPARSER_PRIVATE
#import "XMLNode.h"
#endif

#include "UAXMLNode.h"

@class UAXMLParser;

@protocol UAXMLParserDelegate

- (void)parser:(UAXMLParser*)parser didBeginNode:(UAXMLNode*)node;

@optional
- (void)parser:(UAXMLParser*)parser didEndNode:(UAXMLNode *)node;
- (BOOL)parserWillBeginParse:(UAXMLParser*)parser;
- (void)parserDidEndParse:(UAXMLParser*)parser;

@end

@interface UAXMLParser : NSObject
#ifdef __UAXMLPARSER_PRIVATE
{
    
    xml::XMLNode* documentNode;
    NSMutableArray* delegates;
    UAXMLNode* currentNode;
    NSURL* url;
    BOOL ignoreChildNodes;
    
}
#endif
@property (readonly) UAXMLNode* currentNode;
@property (readonly) NSURL* url;

- (id)initWithContentOfURL:(NSURL*)contentUrl;

- (void)parse;
- (void)ignoreChildNodes;

- (void)pushDelegate:(id<UAXMLParserDelegate>)delegate;
- (void)popDelegate:(id<UAXMLParserDelegate>)delegate;

@end
