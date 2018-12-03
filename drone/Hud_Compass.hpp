#pragma once

#include <SFML/Graphics.hpp>
#include "ResourceManager.hpp"
#include "Functions.hpp"
#include "Constants.hpp"
#include <iostream>
#include "Hud.hpp"

class Hud_Compass : public Hud
{
	sf::RectangleShape needle;
	sf::Texture t_needle;
	sf::VertexArray face;
	sf::VertexArray lines;
	sf::Text quarter[4];
	sf::Text degree;
	float angle = 0.f;
	float target = 0.f;
	sf::Clock framerate;
	float hud_scale;
public:
	Hud_Compass(const sf::Vector2i& resolution, const float& scale) : hud_scale(scale)
	{
		setPosition(60, 70);
		
		
		lines.setPrimitiveType(sf::Lines);
		float step = Constants::pi / 8.0f;
		uint8_t cnt = 0;
		for (float i = 0; i < Constants::double_pi; i += step)
		{
			lines.append(sf::Vertex(sf::Vector2f(
				std::cosf(i)*50.f*scale,
				std::sinf(i)*50.f*scale
			), sf::Color::White));
			if (cnt++ % 4 != 0)
			{
				lines.append(sf::Vertex(sf::Vector2f(
					std::cosf(i)*45.f*scale,
					std::sinf(i)*45.f*scale
				), sf::Color::White));
			}
			else
			{
				lines.append(sf::Vertex(sf::Vector2f(
					std::cosf(i)*40.f*scale,
					std::sinf(i)*40.f*scale
				), sf::Color::White));
			}
		}

		face.setPrimitiveType(sf::TrianglesStrip);
		step = Constants::pi / 32.0f;
		for (float i = 0; i < Constants::double_pi; i += step)
		{
			face.append(sf::Vertex(sf::Vector2f(
				std::cosf(i)*48.f*scale,
				std::sinf(i)*48.f*scale
			), sf::Color::White));
			face.append(sf::Vertex(sf::Vector2f(
				std::cosf(i)*50.f*scale,
				std::sinf(i)*50.f*scale
			), sf::Color::White));
		}


		t_needle.loadFromFile("sprites/compass_needle.png");
		needle.setTexture(&t_needle);

		needle.setSize(sf::Vector2f(16 * scale, 16 * scale));
		needle.setOrigin(8.0*scale, 58.0*scale);


		const char dirs[4] = {'E','S','W','N'};
		for (int i = 0; i < 4; i++)
		{
			quarter[i].setFillColor(i==3?sf::Color::Red:sf::Color::White);

			quarter[i].setFont(*ResourceManager::LoadFont("fonts/arial.ttf"));
			quarter[i].setString(dirs[i]);
			quarter[i].setScale(0.5,0.5);
			const sf::FloatRect bounds = quarter[i].getLocalBounds();
			quarter[i].setOrigin(sf::Vector2f(bounds.width,bounds.height+14.f)/2.f);
			quarter[i].setOutlineColor(sf::Color::Black);
			quarter[i].setOutlineThickness(2.f);
		}
		degree.setFillColor(sf::Color::White);
		degree.setOutlineColor(sf::Color::Black);
		degree.setOutlineThickness(2.f);
		degree.setFont(*ResourceManager::LoadFont("fonts/arial.ttf"));
		degree.setString("0°");
		degree.setScale(0.5, 0.5);
		sf::FloatRect bounds = degree.getLocalBounds();
		degree.setOrigin(sf::Vector2f(bounds.width, bounds.height+14.f) / 2.f);

		setPosition(sf::Vector2f(resolution)-(sf::Vector2f(100.0, 100.0)*scale)- sf::Vector2f(20, 20));
	}


	virtual void Update()
	{
		if ((int)angle != (int)target)
		{
			const float t = framerate.restart().asMicroseconds() / 1000000.0f;
			if (t > 0.1) return;

			float diff = target - angle;
			if (diff < -180) diff += 360;
			else if (diff > 180) diff -= 360;

			angle += std::pow(std::abs(diff)+2, 2)*t*Functions::Sign(diff);
			if (angle < 0) angle += 360;
			else if (angle >= 360) angle -= 360;
			//face.setRotation(angle);
			degree.setString(std::to_string((int)angle) + "°");
			sf::FloatRect bounds = degree.getLocalBounds();
			degree.setOrigin(sf::Vector2f(bounds.width, bounds.height + 14.f) / 2.f);
		}
	}

	const void Set(float angle)
	{
		target = std::fmod(angle, 360.f) + (angle < 0 ? 360.f : 0.f);
		framerate.restart();
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform = getTransform();
		states.transform.translate(50*hud_scale, 50* hud_scale);
		target.draw(face, states);
		states.transform.rotate(-angle);
		target.draw(lines, states);
		states.transform.rotate(angle);
		const sf::Shader* old = states.shader;
		states.shader = nullptr;
		target.draw(needle, states);
		for (int i = 0; i < 4; i++)
		{
			const float a = (-angle *Constants::deg_to_rad) + Constants::half_pi * static_cast<float>(i);
			const sf::Vector2f pos=sf::Vector2f(std::cosf(a), std::sinf(a))*30.f;
			states.transform.translate(pos);
			target.draw(quarter[i], states);
			states.transform.translate(-pos);
		}
		target.draw(degree, states);
	}

};