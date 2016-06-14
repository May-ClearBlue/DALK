//
//  ViewController.h
//  AppTest
//
//  Created by 井筒 紗奈 on 2013/09/06.
//  Copyright (c) 2013年 井筒 紗奈. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

@interface ViewController : UIViewController<UIAccelerometerDelegate> {
@private
    EAGLContext* m_Context;
}

    - (BOOL) Init;
	- (void) drawView : (CADisplayLink*) displayLink;
	- (void) didRotate : (NSNotification*) notification;

@end
