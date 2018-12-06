#pragma once
#include "Hud.hpp"
#include "ResourceManager.hpp"
#include <sstream>
#include <iomanip>

class Hud_TopBar : public Hud
{
private:
	sf::Text ping;
	sf::Text cellvoltage;
public:
	Hud_TopBar(const sf::Vector2i& resolution, const float hud_scale)
	{
		cellvoltage.setFont(*ResourceManager::LoadFont("fonts/arial.ttf"));
		ping.setFont(*ResourceManager::LoadFont("fonts/arial.ttf"));

		cellvoltage.setOutlineThickness(2.0);
		ping.setOutlineThickness(2.0);

		cellvoltage.setOutlineColor(sf::Color::Black);
		ping.setOutlineColor(sf::Color::Black);

		cellvoltage.setPosition(resolution.x / 2.0, 20);
		ping.setString("1000 ms");
		ping.setPosition(resolution.x - ping.getLocalBounds().width/2.0 - 20, 20);
		ping.setString("? ms");

		cellvoltage.setCharacterSize(15.f*hud_scale);
		ping.setCharacterSize(15.f*hud_scale);

		SetPing(0);
		SetCellVoltages(1.111111111, 2.222222222222, 3.3333333333333);
	}


	void SetPing(const float value)
	{
		std::stringstream ss;
		ss << std::setprecision(2) << value << "ms";
		ping.setString(ss.str());
	}

	void SetCellVoltages(const float c1, const float c2, const float c3)
	{
		std::stringstream ss;
		ss << std::setprecision(2);
		ss << std::setw(10) << "C1: " << c1 << "V";
		ss << std::setw(10) << "C2: " << c2 << "V";
		ss << std::setw(10) << "C3: " << c3 << "V";
		cellvoltage.setString(ss.str());
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		target.draw(ping,states);
		target.draw(cellvoltage,states);
	}

	virtual void Update()
	{
	}
};