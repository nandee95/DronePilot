#pragma once

#include <SFML/Graphics.hpp>
#include "ResourceManager.hpp"

struct Debug_Data
{
	float BatteryCellVoltages[3];
};

class Hud_Debug : public sf::Drawable, public sf::Transformable
{
private:
	bool state=false;
	sf::Text text;
public:

	Debug_Data data;

	Hud_Debug()
	{
		text.setFont(*ResourceManager::LoadFont("fonts/arial.ttf"));
	}

	const void Update()
	{
		text.setString("Batery Cell Voltages:\n \
			Cell 1: "+std::to_string(data.BatteryCellVoltages[0])+" V\n \
			Cell 2: "+std::to_string(data.BatteryCellVoltages[1])+" V\n \
			Cell 3: "+std::to_string(data.BatteryCellVoltages[2])+" V\n \
		");
	}

	const void Toggle()
	{
		state = !state;
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		if (!state) return;
		states.transform = getTransform();
		target.draw(text);
	}
};