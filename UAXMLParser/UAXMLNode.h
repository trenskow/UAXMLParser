/*
 
 Created by Kristian Trenskow (The Famous Software Company)
 
 This source code is licensed under the BSD-License (simplified variant).
 Please see LICENSE for more info, or contact info@thefamoussoftwarecompany.com
 if you obtained this source without a LICENSE file.
 
 */

#import <Foundation/Foundation.h>

@interface UAXMLNode : NSObject

@property (nonatomic,readonly,weak) UAXMLNode* parentNode;
@property (nonatomic,readonly) BOOL isDocumentNode;
@property (nonatomic,readonly) NSString* name;
@property (nonatomic,readonly) NSArray* childNodes;
@property (nonatomic,readonly) NSString* innerValue;

- (NSArray*)nodesWithTagName:(NSString*)tagname;
- (NSString*)valueOfAttributeWithName:(NSString*)attributeName;

@end
