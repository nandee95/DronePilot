#pragma once

#include <SFML/Graphics.hpp>
#include "ResourceManager.hpp"
#include "Hud.hpp"

class Hud_Altitude : public Hud
{
	sf::VertexArray lines;
	sf::VertexArray base;
	sf::RectangleShape indicator;
	sf::Text altitude;
	float target=0.f;
	float alt=0.f;
	sf::Clock framerate;
	float hud_scale;
	float cfg_alt_max = 500;
	float cfg_alt_step = 500;
	float vertical_center;
public:
	Hud_Altitude(const sf::Vector2i& resolution,const float& scale,
		const float alt_max,const float alt_step) : hud_scale(scale) ,cfg_alt_max(alt_max),cfg_alt_step(alt_step)
	{
		lines.setPrimitiveType(sf::Lines);
		base.setPrimitiveType(sf::TrianglesStrip);

		altitude.setCharacterSize(30.f* scale);
		vertical_center = 15.f* scale;

		base.append(sf::Vertex(sf::Vector2f(2, 0), sf::Color::White));
		base.append(sf::Vertex(sf::Vector2f(0, 0), sf::Color::White));
		base.append(sf::Vertex(sf::Vector2f(2, -200)*scale, sf::Color::White));
		base.append(sf::Vertex(sf::Vector2f(0, -200)*scale, sf::Color::White));

		int steps = cfg_alt_max / cfg_alt_step;
		if (cfg_alt_max == (steps)*cfg_alt_step) steps++;

		const float scaledstep = cfg_alt_step / cfg_alt_max * 200.0f;
		for (int i = 0; i < steps; i++)
		{
			lines.append(sf::Vertex(sf::Vector2f(0, -scaledstep * i)*scale, sf::Color::White));
			lines.append(sf::Vertex(sf::Vector2f(i % 2 == 1 ? 10 : 20, -scaledstep * i)*scale, sf::Color::White));
		}

		indicator.setOutlineColor(sf::Color::Red);
		indicator.setOutlineThickness(2);
		indicator.setSize(sf::Vector2f(20.0,0.0)*scale);

		altitude.setScale(0.5,0.5);
		altitude.setFillColor(sf::Color::White);
		altitude.setFont(*ResourceManager::LoadFont("fonts/arial.ttf"));
		altitude.setOutlineColor(sf::Color::Black);
		altitude.setOutlineThickness(2.f);

		setPosition(sf::Vector2f(20.f,resolution.y - 20.f));
		Set(0);
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
			alt += max(min(std::pow(std::abs(diff) + 1.0, 2.0)*t, std::abs(diff)), 0.003)*Functions::Sign(diff);
			indicator.setOrigin(0, alt / cfg_alt_max*200.0 * hud_scale);
			altitude.setString(std::to_string((int)alt) + " m");
			altitude.setOrigin(-55.0 * hud_scale, alt / cfg_alt_max * 200.0 * hud_scale * 2   + vertical_center);
		}

	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform = getTransform();
		target.draw(base, states);
		target.draw(lines, states);
		//const sf::Shader* old = states.shader;
		states.shader = nullptr;
		target.draw(altitude, states);
		target.draw(indicator, states);
	}

};