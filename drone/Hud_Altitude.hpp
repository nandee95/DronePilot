#pragma once

#include <SFML/Graphics.hpp>
#include "ResourceManager.hpp"
#include "Hud.hpp"

class Hud_Altitude : public Hud
{
	sf::VertexArray lines;
	sf::RectangleShape indicator;
	sf::Text altitude;
	float target=0.f;
	float alt=0.f;
	sf::Clock framerate;

public:
	Hud_Altitude(const sf::Vector2i& resolution,const float& scale)
	{
		lines.setPrimitiveType(sf::Lines);
		lines.append(sf::Vertex(sf::Vector2f(0, 0), sf::Color::White));
		lines.append(sf::Vertex(sf::Vector2f(0, -200), sf::Color::White));
		lines.append(sf::Vertex(sf::Vector2f(0, 0), sf::Color::White));
		lines.append(sf::Vertex(sf::Vector2f(15, 0), sf::Color::White));
		lines.append(sf::Vertex(sf::Vector2f(0, -50), sf::Color::White));
		lines.append(sf::Vertex(sf::Vector2f(10, -50), sf::Color::White));
		lines.append(sf::Vertex(sf::Vector2f(0, -100), sf::Color::White));
		lines.append(sf::Vertex(sf::Vector2f(15, -100), sf::Color::White));
		lines.append(sf::Vertex(sf::Vector2f(0, -150), sf::Color::White));
		lines.append(sf::Vertex(sf::Vector2f(10, -150), sf::Color::White));
		lines.append(sf::Vertex(sf::Vector2f(0, -200), sf::Color::White));
		lines.append(sf::Vertex(sf::Vector2f(15, -200), sf::Color::White));

		indicator.setOutlineColor(sf::Color::Red);
		indicator.setOutlineThickness(2);
		indicator.setSize(sf::Vector2f(20.0,0.0));

		altitude.setScale(0.5,0.5);
		altitude.setFillColor(sf::Color::White);
		altitude.setFont(*ResourceManager::LoadFont("fonts/arial.ttf"));
		altitude.setString("0 m");
		altitude.setOrigin(-55.0, 20.0);
		altitude.setOutlineColor(sf::Color::Black);
		altitude.setOutlineThickness(2.f);

		setPosition(sf::Vector2f(20.f,resolution.y - 20.f));
	}

	const void Set(float value)
	{
		target = std::fmax(std::fmin(value, 500), 0);
		framerate.restart();
	}

	virtual void Update()
	{
		if ((int)alt != (int)target)
		{
			float diff = target - alt;

			const float t = framerate.restart().asMicroseconds() / 1000000.0;
			alt += max(min(std::pow(std::abs(diff) + 1.0, 2.0)*t, std::abs(diff)), 0.003)*sign(diff);
			indicator.setOrigin(0, alt / 500.0*200.0);
			altitude.setString(std::to_string((int)alt) + " m");
			altitude.setOrigin(-55.0, alt / 500.0*400.0 + 20.0);
		}

	}
	static float sign(float value)
	{
		return value > 0 ? 1.0 : -1.0;
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform = getTransform();
		target.draw(lines, states);
		target.draw(indicator, states);
		const sf::Shader* old = states.shader;
		states.shader = nullptr;
		target.draw(altitude, states);
	}

};