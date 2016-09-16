/*
 
 Created by Kristian Trenskow (The Famous Software Company)
 
 This source code is licensed under the BSD-License (simplified variant).
 Please see LICENSE for more info, or contact info@thefamoussoftwarecompany.com
 if you obtained this source without a LICENSE file.
 
 */

#import <UIKit/UIKit.h>

//! Project version number for UAXMLParser.
FOUNDATION_EXPORT double UAXMLParserVersionNumber;

//! Project version string for UAXMLParser.
FOUNDATION_EXPORT const unsigned char UAXMLParserVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <UAXMLParser/PublicHeader.h>


#import <Foundation/Foundation.h>

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

@property (nonatomic,readonly) UAXMLNode* currentNode;
@property (nonatomic,readonly) NSURL* url;
@property (nonatomic) BOOL automaticallyPopsDelegatesOffStack;
@property (nonatomic,readonly) UAXMLNode* documentNode;

- (id)initWithData:(NSData*)data;
- (id)initWithContentOfURL:(NSURL*)contentUrl;

- (void)parse;
- (void)ignoreChildNodes;

- (void)pushDelegate:(id<UAXMLParserDelegate>)delegate;
- (void)popDelegate:(id<UAXMLParserDelegate>)delegate;

@end
