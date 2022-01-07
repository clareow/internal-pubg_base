#pragma once

class Config {
public:
	bool Skelcfg = true;
	bool Playercfg = true;
	bool Aimbot = true;
	bool Itemcfg = true;
	bool SpeedHack = false;
	bool Radarcfg = true;
	bool DebugMode = false;
	bool MagicBulletcfg = true;
	bool RandomHit = false;
	float SkeletonThick = 1.0f;
	float Fov = 0.f;
	float AimSmooth = 0.18f;
	std::string License = "";
	std::string remainingtime = "";
};

static Config* cfg = new Config;