#include "NewAudio.h"
#include "audio/include/SimpleAudioEngine.h"

using namespace std;
using namespace CocosDenshion;

#define UseOldAudio 0

NewAudio::NewAudio()
{
	_SoundEffState = false;
}

void NewAudio::preloadAllFile()
{
#if UseOldAudio == 1
	SimpleAudioEngine::getInstance()->preloadEffect(Music_BossIn				    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_getDrops				    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_getGolds				    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_shootAmmo1			    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_shootAmmo2			    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_shootAmmo3			    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_MainPlDeadVoice1		    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_MainPlDeadVoice2		    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_MainPlDeadVoice3		    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_PlDead1				    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_PlDead2				    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_PlDead3				    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_PlDead4				    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_PlDead5				    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Boss1Ready			    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Boss1Shoot1			    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Boss1Shoot2			    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_BossKuiJiaDead		    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_BossKuLouDead			    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_BossNvQiangDead		    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_BossHaiShenDead		    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_BossYiZhiDead			    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_BossYanJingDead		    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_BossShuangQiangDead	    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_BossRenZheDead		    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_SpeedOver				    );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Menu						);
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Tiger						);
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Dragon					);
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Feibiao					);
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Flower					);
	SimpleAudioEngine::getInstance()->preloadEffect(Music_IceMissile				);
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Laser						);
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Clow						);
	SimpleAudioEngine::getInstance()->preloadEffect(Music_GameStart		            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_ButtonBack	            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_UpgradeObject	            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_ButtonStart	            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Role1Unlock	            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Role2Unlock	            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Role3Unlock	            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Role1Option	            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Role2Option	            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_Role3Option	            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_BuySuccess	            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_GameWin		            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_RewardGoods	            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_ButtonClose	            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_UserCard		            );
	SimpleAudioEngine::getInstance()->preloadEffect(Music_buyCard		            );
#endif
}

void NewAudio::playBgMusicForce( const std::string& file)
{
	if (!_mucicState)
	{
		return;
	}
	if (file == m_CurBgMusicFile)
	{
		return;
	}
#if UseOldAudio == 1
	SimpleAudioEngine::getInstance()->playBackgroundMusic(file.c_str(),true);
	m_CurBgMusicFile = file;
	return;
#endif

	if (!_mucicState)
	{
		m_CurBgMusicFile = file;
		return;
	}

	auto it = m_MusicVec.begin();
	it = m_MusicVec.find(m_CurBgMusicFile);
	if (it != m_MusicVec.end())
	{
		auto id = (*it).second;
		AudioEngine::stop(id);		//先停止原先的背景音乐
		m_MusicVec.erase(it);
	}

	//播放新的背景音乐
	int id = AudioEngine::play2d(file,true);
	m_MusicVec.insert(pair<std::string, int >(file, id));
	m_CurBgMusicFile = file;
}

void NewAudio::playEffect(const std::string& file, float intervel,unsigned int maxInatance )
{
	return;
	if ( !_SoundEffState )
		return;

	unsigned int id = -1;

	if (Music_Role1Option == file || Music_Role2Option == file || Music_Role3Option == file ||
		Music_Role1Unlock == file || Music_Role2Unlock == file || Music_Role3Unlock == file)
	{
		map<std::string, unsigned int>::iterator	it;
		for (it = m_EffVec.begin(); it != m_EffVec.end(); ++it)
		{
			auto item = it->second;
#if UseOldAudio == 1
			SimpleAudioEngine::getInstance()->stopEffect(item);
#else
			AudioEngine::stop(item);
#endif
		}
	}

#if UseOldAudio == 1
	id = SimpleAudioEngine::getInstance()->playEffect(file.c_str());
#else
	AudioProfile profile;
	profile.minDelay = intervel;

	id = AudioEngine::play2d(file,false,1.0f,&profile);
#endif

	auto it = m_EffVec.find(file);
	if (it != m_EffVec.end())
	{
		//播放过,改变ID
		(*it).second = id;
	}
	else
	{
		//从没播放过
		m_EffVec.insert(pair<std::string, unsigned int>(file, id));
	}
}

void NewAudio::setMusicState( bool state )
{
	UserDefault::getInstance()->setBoolForKey("MusicState", state);

	_mucicState = state;
#ifdef WIN32 
	_mucicState = true;
#endif
	if (m_CurBgMusicFile.empty())
	{
		return;
	}
	if ( !_mucicState )
	{
#if UseOldAudio == 1
		SimpleAudioEngine::getInstance()->stopBackgroundMusic(false);
#else
		auto it = m_MusicVec.begin();
		it = m_MusicVec.find(m_CurBgMusicFile);
		if (it != m_MusicVec.end())
		{
			int id = (*it).second;

			AudioEngine::stop(id);		//停止背景音乐
		}
#endif
	}
	else
	{
#if UseOldAudio == 1
		SimpleAudioEngine::getInstance()->playBackgroundMusic(m_CurBgMusicFile.c_str(),true);
#else
		int id = AudioEngine::play2d(m_CurBgMusicFile,true);

		auto it = m_MusicVec.begin();
		it = m_MusicVec.find(m_CurBgMusicFile);
		if (it != m_MusicVec.end())
		{
			int ad = (*it).second = id;
		}
		else
		{
			m_MusicVec.insert(pair<std::string, int >(m_CurBgMusicFile, id));
		}
#endif
	}
}

bool NewAudio::getMusicState( void )
{
	bool mucicState = UserDefault::getInstance()->getBoolForKey("MusicState", true);

	return mucicState;
}

void NewAudio::setSoundEffState(bool state)
{
	_SoundEffState = state;
	if (!state)
	{
		m_EffVec.clear();
	}

	UserDefault::getInstance()->setBoolForKey("SoundEffState", state);
}

bool NewAudio::getSoundEffState(void)
{
	bool SoundState = UserDefault::getInstance()->getBoolForKey("SoundEffState", true);

	return SoundState;
}


