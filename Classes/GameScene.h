#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

const int row = 8;
const int col = 8;

struct BLOCK
{
	cocos2d::CCSprite *sprite;
	int color;
};

class Game : public cocos2d::CCLayer
{
public:
	virtual bool init();
	static cocos2d::CCScene * scene();
	void menuPlayCallback(CCObject* pSender);
	virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	int floodfill(int x, int y);
	void removeAndUpdate();
	void updateShow();
	void scheduleFunc(float dt);
	void autoCreate();
	CREATE_FUNC(Game);
private:
	BLOCK blocks[row][col];
	int colorCnt;
	bool played, created;
	int score, timeValue, preTime;
	int blockSize;
	bool vis[row][col];
	cocos2d::CCSize winSize;
	cocos2d::CCLabelTTF *scoreLabel, *timeLabel;
};

#endif // __HELLOWORLD_SCENE_H__
