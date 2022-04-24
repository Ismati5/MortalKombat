#ifndef MUSICMANAGER_H
#define MUSICMANAGER_H

#include "SFML/Graphics.hpp"
#include <SFML/Audio.hpp>
#include <iostream>

using namespace sf;
using namespace std;

class MusicManager {
public:
	MusicManager();
	~MusicManager() = default;

	void presentsIntro();
	void startIntro();
	void endIntro();
	void selectorIntro();

	void mainTheme();
	void selectorTheme();
	void historyTheme();

	void skipHistoryTheme();

	void courtyardTheme();
	void thepitTheme();
	void warriorshrineTheme();

	void excellente();
	void fatality();
	void fight();
	void finishHer();
	void finishHim();
	void JhonnyCage();
	void JhonnyCageWins();
	void Kano();
	void KanoWins();
	void laugh();
	void LiuKang();
	void LiuKangWins();
	void Raiden();
	void RaidenWins();
	void Scorpion();
	void ScorpionWins();
	void SonyaBlade();
	void SonyaBladeWins();
	void SubZero();
	void SubZeroWins();

	void moveMain();
	void moveOptions();
	void moveSelector();
	void skipIntro();

	void claps();
	void loosePublic();
	void victoryPublic();

	void comeHere();
	void getOverHere();

	void liuKangPunch01();
	void liuKangKick01();

	void reduceVolumeMusic() { volumeMusic -= 20.f;};
	void addVolumeMusic() { volumeMusic += 20.f;};

	void reduceVolumeEffects() { volumeEffects -= 20.f; };
	void addVolumeEffects() { volumeEffects += 20.f; };

	void updateMusicVolume() {
		for (int i = 0; i < music.size(); i++) {
			music[i].setVolume(volumeMusic);
		}
	}

	void stopIntros();
	void stopMusic();

private:
	vector<Sound> intro;
	vector<Sound> music;
	vector<Sound> announcers;
	vector<Sound> UI;
	vector<Sound> arenas;
	vector<Sound> scorpion;
	vector<Sound> liukang;

	vector<SoundBuffer> bufferIntro;
	vector<SoundBuffer> bufferMusic;
	vector<SoundBuffer> bufferAnnouncers;
	vector<SoundBuffer> bufferUI;
	vector<SoundBuffer> bufferArenas;
	vector<SoundBuffer> bufferScorpion;
	vector<SoundBuffer> bufferLiuKang;

	float volumeMusic = 80.f;
	float volumeEffects = 80.f;

};

#endif
