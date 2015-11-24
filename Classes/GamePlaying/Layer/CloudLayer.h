#ifndef CloudLayer_h__
#define CloudLayer_h__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class CloudLayer : public Layer
{
public:

	CloudLayer();
	CREATE_FUNC(CloudLayer);

	virtual void onEnter();
	void update(float dt);
	void addOneCloud();
	Vector<Node*> cloud_vector;
	Vector<Node*> remove_cloud_vector;
	float next_size;
	float cur_next_size;
	float fly_size;
	float move_speed_y;
	float cur_move_speed_y;
	float move_speed_x;
	void setIsSpeedUp();
private:
	Size		m_ws;
};
#endif // CloudLayer_h__