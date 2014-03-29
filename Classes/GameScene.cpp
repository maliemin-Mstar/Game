#include "GameScene.h"

USING_NS_CC;
char colorName[][100] = {"blue.jpg", "green.jpg", "purple.jpg", "red.jpg", "yellow.jpg"};
int dx[] = {-1, 0, 1, 0}, dy[] = {0, 1, 0, -1};

CCScene* Game::scene()
{
	CCScene *scene = CCScene::create();
	Game *layer = Game::create();
	scene->addChild(layer);
	return scene;
}

bool Game::init()
{
	bool bRet = 0;
	do
	{
		CC_BREAK_IF(!CCLayer::init());
		this->setTouchEnabled(true);
		played = 0;
		created = 0;
		colorCnt = 5;
		blockSize = 25;
		timeValue = 60;
		winSize = CCDirector::sharedDirector()->getWinSize();
		CCMenuItemImage *pPlayItem = CCMenuItemImage::create(
			"btn-play-normal.png",
			"btn-play-selected.png",
			this,
			menu_selector(Game::menuPlayCallback));
		pPlayItem->setPosition(ccp(winSize.width / 2, winSize.height * 0.9));

		CCMenu* pMenu = CCMenu::create(pPlayItem, NULL);
		pMenu->setPosition(CCPointZero);
		this->addChild(pMenu, 1);

		CCLabelTTF* scoreStrLabel = CCLabelTTF::create("Score : ", "Arial", 24);
		scoreStrLabel->setPosition(ccp(50, winSize.height / 2));
		this->addChild(scoreStrLabel, 1);

		CCLabelTTF* timeStrLabel = CCLabelTTF::create("Time : ", "Arial", 24);
		timeStrLabel->setPosition(ccp(55, winSize.height / 2 + 50));
		this->addChild(timeStrLabel, 1);

		scoreLabel = CCLabelTTF::create("the string", "Arial", 24);
		this->addChild(scoreLabel);
		scoreLabel->setPosition(ccp(120, winSize.height / 2));
		scoreLabel->setString("0");

		timeLabel = CCLabelTTF::create("the string", "Arial", 24);
		this->addChild(timeLabel);
		timeLabel->setPosition(ccp(120, winSize.height / 2 + 50));
		timeLabel->setString("60");
		bRet = 1;
	}while(0);

	return bRet;
}

void Game::autoCreate()
{
	int x, y, c;
	x = rand() % row;
	y = rand() % col;
	c = rand() % colorCnt;
	int thisC = blocks[x][y].color;
	for (int i = 0; i < 4; ++i)
	{
		int tx = x + dx[i];
		int ty = y + dy[i];
		if (tx >= 0 && tx < row && ty >= 0 && ty < col)
		{
			blocks[tx][ty].color = thisC;
		}
	}
	updateShow();
}

void Game::scheduleFunc(float dt)
{
	--timeValue;
	if (timeValue < 0)
	{
		unschedule(schedule_selector(Game::scheduleFunc));
		return;
	}
	if (preTime - timeValue > 4)
	{
		preTime = timeValue;
		autoCreate();
	}
	timeLabel->setString(CCString::createWithFormat("%d", timeValue)->getCString());
}

void Game::menuPlayCallback(CCObject* pSender)
{
	score = 0;
	preTime = timeValue = 60;
	scoreLabel->setString("0");
	timeLabel->setString("60");
	schedule(schedule_selector(Game::scheduleFunc), 1);
	if (!played)
	{
		played = 1;
		created = 1;
		srand(time(NULL));
		for (int i = 0; i < row; ++i)
		{
			for (int j = 0; j < col; ++j)
			{
				int randNum = rand() % colorCnt;
				blocks[i][j].sprite = CCSprite::create(colorName[randNum]);
				blocks[i][j].sprite->setTag(randNum);
				blocks[i][j].color = randNum;

				blocks[i][j].sprite->setScale(blockSize / blocks[i][j].sprite->getContentSize().width);
				blocks[i][j].sprite->setPosition(ccp(200 + j * blockSize, 225 - i * blockSize));
				this->addChild(blocks[i][j].sprite);
			}
		}
	}
}

int Game::floodfill(int x, int y)
{
	int cnt = 0;
	for (int i = 0; i < 4; ++i)
	{
		int tx = x + dx[i];
		int ty = y + dy[i];
		if (tx >= 0 && tx < row && ty >= 0 && \
			ty < col && !vis[tx][ty] && \
			blocks[tx][ty].color == blocks[x][y].color)
		{
			++cnt;
			vis[tx][ty] = 1;
			cnt += floodfill(tx, ty);
		}
	}
	return cnt;
}

void Game::updateShow()
{
	int i, j;
	for (i = 0; i < row; ++i)
	{
		for (j = 0; j < col; ++j)
		{
			int c = blocks[i][j].color;
			if (blocks[i][j].sprite->getTag() != c)
			{
				CCTexture2D *texture = \
					CCTextureCache::sharedTextureCache()->addImage(colorName[c]);
				blocks[i][j].sprite->setTexture(texture);
				blocks[i][j].sprite->setTag(c);
			}
		}
	}
}

void Game::removeAndUpdate()
{
	int i, j, k, l;
	int lastLine = row - 1;
	int lack[col];
	for (i = 0; i < col; ++i)
	{
		lack[i] = 0;
		for (j = lastLine; j >= 0; --j)
		{
			if (vis[j][i])
			{
				k = j;
				lack[i] = 1;
				for (l = j - 1; l >= 0; --l)
				{
					if (vis[l][i])
						++lack[i];
					else
					{
						blocks[k][i].color = blocks[l][i].color;
						--k;
					}
				}
				break;          //O(n^2)...
			}
		}
	}
	for (i = 0; i < col; ++i)
	{
		for (j = 0; j < lack[i]; ++j)
		{
			int randNum = rand() % colorCnt;
			blocks[j][i].color = randNum;
		}
	}
	Sleep(200);
	updateShow();
}

void Game::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	if (created && timeValue >= 0)
	{
		preTime = timeValue;
		CCTouch *touch = (CCTouch*)pTouches->anyObject();
		CCPoint location = touch->getLocation();
		int i, j;
		int cnt = 0;
		bool found = 0;
		for (i = 0; i < row; ++i)
		{
			if (found)
				break;
			for (j = 0; j < col; ++j)
			{
				CCRect rect= blocks[i][j].sprite->boundingBox();
				if(rect.containsPoint(location))
				{
					memset(vis, 0, sizeof(vis));
					vis[i][j] = 1;
					cnt += 1;
					cnt += floodfill(i, j);
					found = 1;
					break;
				}
			}
		}
		if (cnt >= 3)
		{
			score += 5 * cnt;
			scoreLabel->setString(CCString::createWithFormat("%d", score)->getCString());
			removeAndUpdate();
		}
	}
}