//#import "IRenderingEngine.hpp"

#import <QuartzCore/QuartzCore.h>
#include "IRenderingEngine.hpp"
#include <Dalk.h>

#define _DALK_IOS_LANDSCAPE_

@interface GLView : UIView {
@private
    EAGLContext*    m_GLContext;
    CDalk			m_Dalk;

    float           m_CurentTime;
	bool			m_IsPortrait;
    int             m_PadCount;
    int	            m_Fps;
}

- (void) drawView : (CADisplayLink*) displayLink;
- (void) didRotate : (NSNotification*) notification;

@end
