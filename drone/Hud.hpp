#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

class Hud : public sf::Drawable, public sf::Transformable
{
public:
	virtual void Update() = 0;
};