// 手に持った機器本体の向き（ここではUIDeviceOrientationと同一）
enum DeviceOrientation {
    DeviceOrientationUnknown,
    DeviceOrientationPortrait,
    DeviceOrientationPortraitUpsideDown,
    DeviceOrientationLandscapeLeft,
    DeviceOrientationLandscapeRight,
    DeviceOrientationFaceUp,
    DeviceOrientationFaceDown,
};

// レンダリングエンジンのインスタンスを作成し、種々のOpenGLの状態を設定する
struct IRenderingEngine* CreateRenderer1();
struct IRenderingEngine* CreateRenderer2();

// OpenGL ESレンダリングエンジンのインタフェース；GLViewで使用される
struct IRenderingEngine {
    virtual void Initialize(int width, int height) = 0;    
    virtual void Render() const = 0;
    virtual void UpdateAnimation(float timeStep) = 0;
    virtual void OnRotate(DeviceOrientation newOrientation) = 0;
    virtual ~IRenderingEngine() {}
};
