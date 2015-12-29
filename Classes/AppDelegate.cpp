#include "AppDelegate.h"
#include "GameScene.h"

#include "behaviac/agent/agent.h"
#include "hero.h"
#include "NPC.h"
#include "enemy.h"
#include "behaviac/base/file/filemanager.h"

USING_NS_CC;

AppDelegate::AppDelegate() {
			 
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	FileUtils::getInstance()->addSearchPath("../../res/");
#else
	FileUtils::getInstance()->addSearchPath("res/");
#endif

#if BEHAVIAC_COMPILER_ANDROID && (BEHAVIAC_COMPILER_ANDROID_VER > 8)
	behaviac::CFileManager::GetInstance()->SetAssetManager(cocos2d::FileUtilsAndroid::getAssetManager());
#endif

	std::string path = cocos2d::FileUtils::getInstance()->fullPathForFilename("behaviac/data.xml");
	path = path.substr(0, path.rfind('/'));
	behaviac::Config::SetSocketBlocking(false);

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	auto expPath = path;
	expPath.append("/data.xml");
	behaviac::Workspace::GetInstance()->ExportMetas(expPath.c_str());
#endif

	path = path.append("/exported");
	behaviac::Workspace::GetInstance()->SetFilePath(path.c_str());

	CCLOG("behaviac::Workspace::GetInstance()->GetFilePath:%s",behaviac::Workspace::GetInstance()->GetFilePath());

	behaviac::Agent::Register<NPC>();
	behaviac::Agent::Register<Hero>();
	behaviac::Agent::Register<Enemy>();
	//behaviac::TypeRegister::Register<NPC::refresh_delay>("NPC::refresh_delay");



	//if (!cl.IsProfiling())
	//{
	//    behaviac::Agent::SetIdMask(kIdMask_Wolrd | kIdMask_Opponent);
	//}

	behaviac::Config::SetLogging(true);

}

AppDelegate::~AppDelegate() 
{
	behaviac::Agent::UnRegister<NPC>();
	behaviac::Agent::UnRegister<Hero>();
	behaviac::Agent::UnRegister<Enemy>();
}

void AppDelegate::initGLContextAttrs()
{
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
		glview = GLViewImpl::createWithRect("AirBattleDemo", Rect(0, 0, 540, 960)); //设置PC端的设备分辨率，窗口名。在移动端这行代码无效。
        director->setOpenGLView(glview);
    }

	glview->setDesignResolutionSize(540, 960, ResolutionPolicy::SHOW_ALL); //设置游戏分辨率，尚未适配移动设备屏幕。

    director->setDisplayStats(true);

    director->setAnimationInterval(1.0 / 60);

	auto scene = GameScene::createScene();

	director->runWithScene(scene);

	//auto scene = Scene::create();
	//Label* label = Label::create("xxxx","xxxxxxxxx",40);
	//auto size = director->getWinSize();
	//label->setPosition(size.width / 2,size.height / 2);
	//scene->addChild(label);
	//director->runWithScene(scene);

    return true;
}

void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
