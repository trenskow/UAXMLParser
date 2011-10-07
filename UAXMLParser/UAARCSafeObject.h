//
//  UAARCSafeObject.h
//  UAXMLParser
//
//  Created by Kristian Trenskow on 10/7/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#if defined(__has_feature) && __has_feature(objc_arc)
#define _ARC_ENABLED
#endif

@interface NSObject (UAARCSafeAdditions)

- (id)arcSafeRetain;
- (void)arcSafeRelease;
- (id)arcSafeAutorelease;

@end
