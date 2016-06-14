//
//  ViewController.m
//  AppTest
//
//  Created by 井筒 紗奈 on 2013/09/06.
//  Copyright (c) 2013年 井筒 紗奈. All rights reserved.
//

#import "ViewController.h"

@interface ViewController ()

@end

@implementation ViewController

//定型
+ (Class) layerClass {
	return [CAEAGLLayer class];
}

- (BOOL) Init {
		CGRect frame = [[UIScreen mainScreen] bounds];

//    if (self = [super initWithFrame:frame]) {
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*) super.view.layer;
//透過不要
        eaglLayer.opaque = YES;
		eaglLayer.drawableProperties =	[NSDictionary dictionaryWithObjectsAndKeys:
												[NSNumber numberWithBool:FALSE],
												kEAGLDrawablePropertyRetainedBacking, 
												kEAGLColorFormatRGBA8,
												kEAGLDrawablePropertyColorFormat,
												nil
										];

        EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
        m_Context = [[EAGLContext alloc] initWithAPI:api];

        if (!m_Context) {
			NSLog(@"Failed:= [[EAGLContext alloc] initWithAPI:api]");
			return FALSE;
		}
		
		if(![EAGLContext setCurrentContext:m_Context]) {
			NSLog(@"Failed:[EAGLContext setCurrentContext:m_Context])");
            //   [self release];
            return FALSE;
        }

        [m_Context renderbufferStorage:GL_RENDERBUFFER fromDrawable: eaglLayer];
        
//      m_renderingEngine->Initialize(CGRectGetWidth(frame), CGRectGetHeight(frame));
      
        [self drawView: nil];
  
//        m_timestamp = CACurrentMediaTime();

        CADisplayLink* displayLink;
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
        
        [	
			[NSNotificationCenter defaultCenter]
				addObserver:self
				selector:@selector(didRotate:)
				name:UIDeviceOrientationDidChangeNotification
			    object:nil
		];
    //}
    return TRUE;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.

	[self Init];

// フルスクリーンにする
	[UIApplication sharedApplication].statusBarHidden	= YES;
// マルチタッチイベントを検出する
	self.view.multipleTouchEnabled = YES;

// 傾きセンサ登録
    UIAccelerometer *pAccel = [UIAccelerometer sharedAccelerometer];
    pAccel.delegate = self;
    pAccel.updateInterval = 1.0f/60.0f;       
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void) drawView: (CADisplayLink*) displayLink
{
#if 0
    if (displayLink != nil) {
        float elapsedSeconds = displayLink.timestamp - m_timestamp;
        m_timestamp = displayLink.timestamp;
        m_renderingEngine->UpdateAnimation(elapsedSeconds);
    }

    m_renderingEngine->Render();
#endif
    [m_Context presentRenderbuffer:GL_RENDERBUFFER];
}
//iOS5以前
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
	return (interfaceOrientation == UIInterfaceOrientationLandscapeRight);
}

//iOS6以降
- (BOOL)shouldAutorotate {
	return YES;
}

- (NSUInteger)supportedInterfaceOrientations {
	return UIInterfaceOrientationMaskAll;
}

- (void) didRotate: (NSNotification*) notification
{
    UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
    //m_renderingEngine->OnRotate((DeviceOrientation) orientation);
    [self drawView: nil];
}

@end
