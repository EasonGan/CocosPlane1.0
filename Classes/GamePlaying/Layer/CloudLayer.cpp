#include "CloudLayer.h"
#include "../../GameCommon/GameData.h"

CloudLayer::CloudLayer()
{
	this->move_speed_y = 750;
	this->move_speed_x = 15;
	this->next_size = 1600;
	this->fly_size = 0;
}

void CloudLayer::onEnter()
{
	Layer::onEnter();
	this->m_ws = Director::getInstance()->getWinSize();
	this->addOneCloud();
	this->scheduleUpdate();
}

void CloudLayer::setIsSpeedUp()
{
	this->move_speed_y *= 6;
	this->cur_move_speed_y = this->move_speed_y * random(0.9,1.1);
}

void CloudLayer::addOneCloud()
{
	this->cur_next_size = this->next_size * random(0.8,1.2);
	int randId = random(1,5);
	auto file = String::createWithFormat("Bg/cloud/cloud_%d.png", randId)->getCString();
	auto cloud = Sprite::create(file);
	cloud->setPosition(this->m_ws.width* (0.5 + rand_minus1_1() * 0.3), this->m_ws.height * 2.0);
	this->addChild(cloud);
	this->cloud_vector.pushBack(cloud);
	this->cur_move_speed_y = this->move_speed_y * random(0.8,1.1);
	this->move_speed_x *= rand_0_1() * 2 -1;
	int Zorder = random(1, 3);
	cloud->setOpacity(100 + Zorder * 50);
	cloud->setLocalZOrder(Zorder);
	cloud->setScale(1.0f + rand_0_1() * 6.0f);
}

void CloudLayer::update(float dt)
{
	float size_x = this->move_speed_x * dt;
	float size_y = this->cur_move_speed_y * dt;

	this->fly_size += size_y;
	if(this->fly_size >= this->cur_next_size) {
		this->fly_size = 0;
		this->addOneCloud();
	}

	for(Vector<Node*>::iterator it = this->cloud_vector.begin();it != this->cloud_vector.end();it ++) {
		Node* cloud = *it;
		if(cloud->getPositionY() < -900) {
			this->remove_cloud_vector.pushBack(cloud);
		} else {
			cloud->setPosition(cloud->getPosition() - Vec2(size_x,size_y));
		}
	}

	for(Vector<Node*>::iterator it = this->remove_cloud_vector.begin();it != this->remove_cloud_vector.end();it ++) {
		Node* node = *it;
		node->removeFromParent();
		this->cloud_vector.eraseObject(node);
	}

	if(this->remove_cloud_vector.size() > 0) {
		this->remove_cloud_vector.clear();
	}
}

