#ifndef __GUIDE_LAYER_H__
#define __GUIDE_LAYER_H__

#include "cocos2d.h"
#include "ui/UIText.h"

USING_NS_CC;
using namespace std;

class GuideLayer :public Layer
{
public:
	GuideLayer(){};
	~GuideLayer(){};
	CREATE_FUNC(GuideLayer);

	virtual bool init() override;
	static GuideLayer * getInstance();

	void setStencilPos(Vec2 pos);
	void setStencilSize(Size size);

	void setFingerPos(Vec2 pos);
	void setDescContent(std::string fileName, float px, float py);

	void show(float time, int returnId, int nextId, std::string fingerName = "hand");
	void hide();
	void showById(int id);
	void timeOverHander(float at);
private:
	static GuideLayer * _instance;
	ClippingNode * _clipper;
	LayerColor * _content;
	DrawNode * _stencil;
	Sprite * _fingerImg;
	Label * _descLabel;
	Node * _dialogLayer;
	ui::Text * _dialogText;
	vector<string> _dialogVec;
	int _dialogIndex = 0;
	int _id;
	int _returnId;
	int _nextId;
	int _guideEnd = 0;
	EventListenerTouchOneByOne * _listener;

	virtual bool onTouchBegan(Touch* touch, Event* event) override;

	Point getLocatePoint(string locateType, string locateId);
};

#endif