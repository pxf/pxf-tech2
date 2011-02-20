//
//  iPhoneTestAppDelegate.m
//  iPhoneTest
//
//  Created by work on 2/17/11.
//  Copyright __MyCompanyName__ 2011. All rights reserved.
//

#import "iPhoneTestAppDelegate.h"
#import "EAGLView.h"

@implementation iPhoneTestAppDelegate

@synthesize window;
@synthesize glView;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [glView startAnimation];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    [glView stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    [glView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [glView stopAnimation];
}

- (void)dealloc
{
    [window release];
    [glView release];

    [super dealloc];
}

@end
