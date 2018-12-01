#pragma once

#include <SFML/Graphics.hpp>
#include "ResourceManager.hpp"

class Hud_Status : public sf::Drawable, public sf::Transformable
{
protected:
	const sf::Texture* t_green, *t_red,*t_yellow;
	bool status = false;
	sf::RectangleShape icon;
	sf::Text text;
public:
	Hud_Status() :
		t_green(ResourceManager::LoadTexture("sprites/status_green.png")),
		t_red(ResourceManager::LoadTexture("sprites/status_red.png")),
		t_yellow(ResourceManager::LoadTexture("sprites/status_yellow.png"))
	{
		text.setFont(*ResourceManager::LoadFont("fonts/arial.ttf"));
		text.setScale(0.5,0.5);
		text.setOutlineColor(sf::Color::Black);
		text.setOutlineThickness(2.f);
		text.setFillColor(sf::Color::White);
		text.setOrigin(-40.0, 2.0);
		icon.setSize(sf::Vector2f(16,16));
		icon.setTexture(t_red);
	}

	virtual const void SetString(sf::String title)
	{
		text.setString(title);
	}

	const void SetStatus(uint8_t status)
	{
		icon.setTexture(status==0 ? t_red : (status == 1 ? t_green : t_yellow));
	}


	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform = getTransform();
		target.draw(icon, states);
		target.draw(text, states);
	}
};


class Hud_RF : public Hud_Status
{
public:
	Hud_RF()
	{
		SetString("?");
	}
	virtual const void SetString(sf::String title)
	{
		text.setString("RF: "+title);
	}
};

class Hud_CAM : public Hud_Status
{
public:
	Hud_CAM()
	{
		SetString("Off");
	}
	virtual const void SetString(sf::String title)
	{
		text.setString("CAM: " + title);
	}

};

