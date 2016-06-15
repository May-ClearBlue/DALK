#import "GLView.h"

//#include <dxpad.h>

@implementation GLView

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

- (id) initWithFrame: (CGRect) frame
{
    @autoreleasepool {

		//Retina対応（というかそれ以前は基本サポートしない）
		float scale =[UIScreen mainScreen].scale;
		self.contentScaleFactor = [UIScreen mainScreen].scale;

		float _width = frame.size.width;
		frame.size.width  = frame.size.height;
		frame.size.height = _width;
		//[self setFrame:frame];

	//EAGLコンテキスト生成（）
    if( self = [super initWithFrame:frame] ) {
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*) super.layer;
        eaglLayer.opaque = YES;									//透過OFF
		eaglLayer.contentsScale = [UIScreen mainScreen].scale;	//Retina

        EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
        m_GLContext = [[EAGLContext alloc] initWithAPI:api];

        if (!m_GLContext || ![EAGLContext setCurrentContext : m_GLContext]) {
			NSLog(@"Failed:EAGLContext");
			//[self release];
            return nil;
        }

        NSLog(@"Initialized:OpenGL ES2.0");
#ifdef _DALK_IOS_LANDSCAPE_
		//座標軸も横に回転
		CGAffineTransform transform = CGAffineTransformMakeRotation(_PI * 90 / 180.0f);
		transform = CGAffineTransformTranslate(transform, (frame.size.width - frame.size.height)/ 2, (frame.size.width - frame.size.height)/ 2);
		self.transform = transform;
#endif

		COpenGL::PreInitialize();
        [m_GLContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];       
		COpenGL::AfterInitialize();

		m_AppMain.Init(CGRectGetWidth(frame) * [UIScreen mainScreen].scale, CGRectGetHeight(frame) * [UIScreen mainScreen].scale);

		//たいまー
        [self drawView: nil];
        m_CurrentTime = CACurrentMediaTime();

		//DisplayLink
        CADisplayLink* displayLink;
        displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawView:)];
        
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

//回転
#if 0
        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
        
        [[NSNotificationCenter defaultCenter]
            addObserver:self
            selector:@selector(didRotate:)
            name:UIDeviceOrientationDidChangeNotification
            object:nil
		];
#endif

        // マルチタッチイベントを検出する
        self.multipleTouchEnabled = YES;

        [self PADinit];
    }
    }
    return self;
}

- (void)viewDidLoad
{
	// Do any additional setup after loading the view, typically from a nib.

	// フルスクリーンにする
	[UIApplication sharedApplication].statusBarHidden	= YES;
	// マルチタッチイベントを検出する
	self.multipleTouchEnabled = YES;

#if 0
	// 傾きセンサ登録
    UIAccelerometer *pAccel = [UIAccelerometer sharedAccelerometer];
    pAccel.delegate = self;
    pAccel.updateInterval = 1.0f/60.0f;       
#endif
}

- (void) didRotate: (NSNotification*) notification
{
//回転通知は無視（横固定）
//    UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
//    m_renderingEngine->OnRotate((DeviceOrientation) orientation);
//    [self drawView: nil];
}

- (void) drawView: (CADisplayLink*) displayLink
{
    if (displayLink != nil) {
        float elapsedSeconds = displayLink.timestamp - m_timestamp;
        if(elapsedSeconds >= 1.0f) {
            _DEBUGPRINTF("fps = %d", m_Fps);
            m_Fps = 0;
            m_CurrentTime = displayLink.timestamp;
        }
        else
            m_Fps++;
//        m_timestamp = displayLink.timestamp;
//        UpdateTime();  
    }
 
    m_AppMain.Draw();

    for(int a = 0; a < 20; a ++ )
        PADTouch[a] = 0;

    [m_GLContext presentRenderbuffer:GL_RENDERBUFFER];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    // シングルタッチの場合
    UITouch *touch = [touches anyObject];
    CGPoint location = [touch locationInView:self];
    NSLog(@"x:%f y:%f", location.x, location.y);

    location.x *= 2; location.y *= 2;
    
    if(touch.tapCount ==  1) {
        for(int a = 0; a < 4; a ++ ) {
            if( location.x >= PADRect[a].left && location.x <= PADRect[a].right &&
               location.y >= PADRect[a].top  && location.y <= PADRect[a].bottom) {
                PADTouch[a] = 1;
                return;
            }
        }
        if( location.x >= PADRect[4].left && location.x <= PADRect[4].right &&
           location.y >= PADRect[4].top  && location.y <= PADRect[4].bottom) {
            PADTouch[5] = 1;
            return;
        }
        else if( location.x >= PADRect[5].left && location.x <= PADRect[5].right &&
                location.y >= PADRect[5].top  && location.y <= PADRect[5].bottom) {
            _DEBUG_HEAP_INFO();
            return;
        }
 
        PADTouch[PADCUST_OK] = 1;
    }
    else {
        PADTouch[PADCUST_CANCEL] = 1;
    }
}

- (void) PADinit {
    m_PadCount = 0;

    @autoreleasepool {
/*
     // シングルタップ
     // ダブルタップ
     UITapGestureRecognizer* tapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleTapGesture:)];
     UITapGestureRecognizer* doubleTapGesture = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(handleDoubleTapGesture:)];
  
     [tapGesture requireGestureRecognizerToFail:doubleTapGesture];
     doubleTapGesture.numberOfTapsRequired = 2;


     [self addGestureRecognizer:tapGesture];
     [self addGestureRecognizer:doubleTapGesture];
 */
     //	ピンチ
     UIPinchGestureRecognizer* pinchGesture = [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(handlePinchGesture:)];
     [self addGestureRecognizer:pinchGesture];
  /*
     //	パン（ドラッグ）
     
     UIPanGestureRecognizer* panGesture = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(handlePanGesture:)];
     // ジェスチャーを認識する指の最大数
     panGesture.maximumNumberOfTouches = 3;
     // ジェスチャーを認識する指の最小数
     panGesture.minimumNumberOfTouches = 1;
     [self.view addGestureRecognizer:panGesture];
     [panGesture release];
*/
    // 左へスワイプ
    UISwipeGestureRecognizer* swipeLeftGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeLeftGesture:)];
    swipeLeftGesture.direction = UISwipeGestureRecognizerDirectionLeft;
    [self addGestureRecognizer:swipeLeftGesture];

    // 右へスワイプ
    UISwipeGestureRecognizer* swipeRightGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeRightGesture:)];
    swipeRightGesture.direction = UISwipeGestureRecognizerDirectionRight;
    [self addGestureRecognizer:swipeRightGesture];
    // 上へスワイプ
    UISwipeGestureRecognizer* swipeUpGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeUpGesture:)];
    swipeUpGesture.direction = UISwipeGestureRecognizerDirectionUp;
    [self addGestureRecognizer:swipeUpGesture];
    // 下へスワイプ
    UISwipeGestureRecognizer* swipeDownGesture = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(handleSwipeDownGesture:)];
    swipeDownGesture.direction = UISwipeGestureRecognizerDirectionDown;
    [self addGestureRecognizer:swipeDownGesture];

    /*
     //	ローテイト
     UIRotationGestureRecognizer* rotationGesture = [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(handleRotationGesture:)];
     [self.view addGestureRecognizer:rotationGesture];
     [rotationGesture release];
     */
    //	ロングプレス
    UILongPressGestureRecognizer* longPressGesture = [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(handleLongPressGesture:)];
    [self addGestureRecognizer:longPressGesture];

    }
}
/*
 //-----------------------------------------------------------------------------------------------------
 //	タップ
 - (void) handleTapGesture:(UITapGestureRecognizer*)sender {
     if(sender.numberOfTouches == 1) {
         CGPoint location = [sender locationInView:self];
         for(int a = 0; a < 4; a ++ ) {
             if( location.x >= PADRect[a].left && location.x <= PADRect[a].right &&
                location.y >= PADRect[a].top  && location.y <= PADRect[a].bottom) {
                 PADTouch[a] = 1;
                 return;
             }
         }
         if( location.x >= PADRect[4].left && location.x <= PADRect[4].right &&
            location.y >= PADRect[4].top  && location.y <= PADRect[4].bottom) {
             PADTouch[5] = 1;
             return;
         }
         else if( location.x >= PADRect[5].left && location.x <= PADRect[5].right &&
                 location.y >= PADRect[5].top  && location.y <= PADRect[5].bottom) {
             _DEBUG_HEAP_INFO();
             return;
         }
         PADTouch[PADCUST_OK] = 1;
     }
     else
         PADTouch[PADCUST_CANCEL] = 1;
 }
 - (void) handleDoubleTapGesture:(UITapGestureRecognizer*)sender
 {
     PADTouch[PADCUST_CANCEL] = 1;
 }
*/
 //----------------------------------------------------------------------------------------------------
 //	ピンチ
 - (void) handlePinchGesture:(UIPinchGestureRecognizer*) sender
 {
/*
 UIPinchGestureRecognizer* pinch = (UIPinchGestureRecognizer*)sender;
 sprintf(buf, "Pinch scale=%f, velocity=%f", pinch.scale, pinch.velocity);
 PADaddLog(buf);
*/
     PADTouch[PADCUST_STD_Y] = 1;
//	NSLog(@"pinch scale=%f, velocity=%f", pinch.scale, pinch.velocity);
 }
 
 /*
 //-----------------------------------------------------------------------------------------------------
 //	パン（ドラッグ）
 - (void) handlePanGesture:(UIPanGestureRecognizer*) sender
 {
 UINT8	buf[256];
 
 UIPanGestureRecognizer* pan = (UIPanGestureRecognizer*) sender;
 CGPoint location = [pan translationInView:self.view];
 sprintf(buf, "Pan x=%f, y=%f", location.x, location.y);
 PADaddLog(buf);
 //	NSLog(@"pan x=%f, y=%f", location.x, location.y);
 }
 //-----------------------------------------------------------------------------------------------------
 //	ローテイト
 - (void) handleRotationGesture:(UIRotationGestureRecognizer*) sender
 {
 UINT8	buf[256];
 
 UIRotationGestureRecognizer* rotation = (UIRotationGestureRecognizer*) sender;
 sprintf(buf, "Rotation rad=%f, velocity=%f", rotation.rotation, rotation.velocity);
 PADaddLog(buf);
 //	NSLog(@"rotation rad=%f, velocity=%f", rotation.rotation, rotation.velocity);
 }
 */
//-----------------------------------------------------------------------------------------------------
//	スワイプ
- (void) handleSwipeLeftGesture:(UISwipeGestureRecognizer *)sender {
    PADTouch[PADCUST_FORCESKIP] = 1;

}
- (void) handleSwipeRightGesture:(UISwipeGestureRecognizer *)sender {
    PADTouch[PADCUST_STD_X] = 1;
             //SKIPMODE] = 1;
}
- (void) handleSwipeUpGesture:(UISwipeGestureRecognizer *)sender {
    PADTouch[PADCUST_STD_B] = 1;
        //  PADTouch[PADCUST_HIDEWIN] = 1;
}
- (void) handleSwipeDownGesture:(UISwipeGestureRecognizer *)sender {
    PADTouch[PADCUST_BACKLOG] = 1;
}
//-----------------------------------------------------------------------------------------------------
//	ロングプレス
- (void) handleLongPressGesture:(UILongPressGestureRecognizer*) sender {
    //押された時のみ検出
    if([sender state] == UIGestureRecognizerStateBegan)
        PADTouch[PADCUST_AUTOMODE] = 1;
}

@end
