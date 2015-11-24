/********************************************************************
创建于:		2015/03/05  14:57	
文件名: 	E:\Cocos3.3_Project\PlaneGame\Classes\Common\NewAudio.h 
作  者:		alex()
修改于：	2015/03/05  14:57	

目  的:		
*********************************************************************/
#ifndef __NewAudio_H__
#define __NewAudio_H__


#include "cocos2d.h"
#include "cocos-ext.h"
#include "../../cocos2d/cocos/audio/include/AudioEngine.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace experimental;

#define Music_Bg					"Music/Music_Bg.ogg"
#define Music_Bg_Play				"Music/Music_Bg_Play.ogg"
#define Music_BossIn				"Music/Music_BossIn.ogg"		//OK
#define Music_getDrops				"Music/Music_getDrops.ogg"		//OK
#define Music_getGolds				"Music/Music_getGolds.ogg"
#define Music_shootAmmo1			"Music/Music_shootAmmo1.ogg"	//OK
#define Music_shootAmmo2			"Music/Music_shootAmmo2.ogg"	//OK
#define Music_shootAmmo3			"Music/Music_shootAmmo3.ogg"	//OK
#define Music_MainPlDeadVoice1		"Music/Music_MainPlDeadVoice1.ogg"	//OK
#define Music_MainPlDeadVoice2		"Music/Music_MainPlDeadVoice2.ogg"	//OK
#define Music_MainPlDeadVoice3		"Music/Music_MainPlDeadVoice3.ogg"	//OK
#define Music_PlDead1				"Music/Music_PlDead1.ogg"		//OK
#define Music_PlDead2				"Music/Music_PlDead2.ogg"		//OK
#define Music_PlDead3				"Music/Music_PlDead3.ogg"		//OK
#define Music_PlDead4				"Music/Music_PlDead4.ogg"		//OK
#define Music_PlDead5				"Music/Music_PlDead5.ogg"		//OK
#define Music_Boss1Ready			"Music/Boss1Ready.ogg"			//OK
#define Music_Boss1Shoot1			"Music/Boss1Shoot1.ogg"			//OK
#define Music_Boss1Shoot2			"Music/Boss1Shoot2.ogg"			//OK
#define Music_BossKuiJiaDead		"Music/Music_BossKuiJiaDead.ogg"
#define Music_BossKuLouDead			"Music/Music_BossKuLouDead.ogg"
#define Music_BossNvQiangDead		"Music/Music_BossNvQiangDead.ogg"
#define Music_BossHaiShenDead		"Music/Music_BossHaiShenDead.ogg"
#define Music_BossYiZhiDead			"Music/Music_BossYiZhiDead.ogg"
#define Music_BossYanJingDead		"Music/Music_BossYanJingDead.ogg"
#define Music_BossShuangQiangDead	"Music/Music_BossShuangQiangDead.ogg"
#define Music_BossRenZheDead		"Music/Music_BossRenZheDead.ogg"
#define Music_SpeedOver				"Music/Music_SpeedOver.ogg"		//OK

#define Music_Menu					"Music/Music_Menu.ogg"

//特殊子弹
#define Music_Tiger					"Music/Music_Tiger.ogg"			//OK
#define Music_Dragon				"Music/Music_Dragon.ogg"		//OK
#define Music_Feibiao				"Music/Music_Feibiao.ogg"		//OK
#define Music_Flower				"Music/Music_Flower.ogg"		//OK
#define Music_IceMissile			"Music/Music_IceMissile.ogg"	//OK
#define Music_Laser					"Music/Music_Laser.ogg"			//OK
#define Music_Clow					"Music/Music_Clow.ogg"

// 战斗外
#define Music_GameStart				"Music/GameStart.ogg"
#define Music_ButtonBack			"Music/ButtonBack.ogg"
#define Music_UpgradeObject			"Music/UpgradeObject.ogg"
#define Music_ButtonStart			"Music/ButtonStart.ogg"
#define Music_Role1Unlock			"Music/Role1_Unlock.ogg"
#define Music_Role2Unlock			"Music/Role2_Unlock.ogg"
#define Music_Role3Unlock			"Music/Role3_Unlock.ogg"

#define Music_Role1Option			"Music/Role1_Option.ogg"
#define Music_Role2Option			"Music/Role2_Option.ogg"
#define Music_Role3Option			"Music/Role3_Option.ogg"

#define Music_BuySuccess			"Music/Music_BuySuccess.ogg"
#define Music_GameWin				"Music/Music_GameWin.ogg"

#define Music_RewardGoods			"Music/Music_FinishTask.ogg"
#define Music_ButtonClose			"Music/ButtonClose.ogg"
#define Music_UserCard				"Music/Music_UserCard.ogg"
#define Music_buyCard				"Music/Music_buyCard.ogg"

class NewAudio
{
public:
	static NewAudio* getInstance()
	{
		static NewAudio instance;
		return &instance;
	}


	void setSoundEffState(bool state);		//音效
	bool getSoundEffState( void );

	
	void setMusicState( bool state );//音乐
	bool getMusicState( void );

	//如果当前播放的音乐就是即将要播放的音乐，那么不再播放，否则强制停止当前背景音乐，播放新的背景音乐
	void playBgMusicForce(const std::string& file);

	void playEffect(const std::string& file, float intervel = 0.05f, unsigned int maxInatance = 5);


	void preloadAllFile();
private:
	NewAudio();

	bool		_mucicState;
	bool		_SoundEffState;

	std::string							m_CurBgMusicFile;

	std::map<std::string, unsigned int>			m_EffVec;
	std::map<std::string, int>			m_MusicVec;
};


#endif // __NewAudio_H__