//
//  UAARCSafeObject.m
//  UAXMLParser
//
//  Created by Kristian Trenskow on 10/7/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "UAARCSafeObject.h"

@implementation NSObject (UAARCSafeAdditions)

- (id)arcSafeRetain {

#if defined(_ARC_ENABLED)
    return self;
#else
    return [self retain];
#endif
    
}

- (void)arcSafeRelease {
    
#if !defined(_ARC_ENABLED)
    [self release];
#endif
    
}

- (id)arcSafeAutorelease {
    
#if defined(_ARC_ENABLED)
    return self;
#else
    return [self autorelease];
#endif
    
}

@end
