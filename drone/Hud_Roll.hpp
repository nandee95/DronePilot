#pragma once

#include <SFML/Graphics.hpp>
#include "Functions.hpp"
#include "Constants.hpp"

class Hud_Roll : public Hud
{
	sf::VertexArray s_circle, s_circle2, s_mark;
	float angle = 0.0f;
	float target = 0.0f;
	sf::VertexArray r_circle, r_circle2, r_mark;
	sf::Clock framerate;
public:
	Hud_Roll(const sf::Vector2i& resolution,const float& scale)
	{
		s_circle.setPrimitiveType(sf::TrianglesStrip);
		s_circle2.setPrimitiveType(sf::TrianglesStrip);
		for (float i = -0.43633 - 0.08726; i < 0.43633; i += 0.08726)//5 deg
		{
			const float next = i + 0.08726;

			sf::Vertex v1(sf::Vector2f(std::cosf(i)*250.0*scale, std::sinf(i)*250.0*scale), sf::Color::White); //Outer current
			sf::Vertex v2(sf::Vector2f(std::cosf(next)*248.0*scale, std::sinf(next)*248.0*scale), sf::Color::White); //Inner current
			sf::Vertex v3(sf::Vector2f(std::cosf(i)*250.0*scale, std::sinf(i)*250.0*scale), sf::Color::White); //Outer next
			sf::Vertex v4(sf::Vector2f(std::cosf(next)*248.0*scale, std::sinf(next)*248.0*scale), sf::Color::White); //Outer next
			
			s_circle.append(v2);
			s_circle.append(v1);
			s_circle.append(v4);
			s_circle.append(v3);

			v1 = sf::Vertex(sf::Vector2f(std::cosf(i + Constants::pi)*250.0*scale, std::sinf(i + Constants::pi)*250.0*scale), sf::Color::White); //Outer current
			v2 = sf::Vertex(sf::Vector2f(std::cosf(next + Constants::pi)*248.0*scale, std::sinf(next + Constants::pi)*248.0*scale), sf::Color::White); //Inner current
			v3 = sf::Vertex(sf::Vector2f(std::cosf(i + Constants::pi)*250.0*scale, std::sinf(i + Constants::pi)*250.0*scale), sf::Color::White); //Outer next
			v4 = sf::Vertex(sf::Vector2f(std::cosf(next + Constants::pi)*248.0*scale, std::sinf(next + Constants::pi)*248.0*scale), sf::Color::White); //Outer next

			s_circle2.append(v2);
			s_circle2.append(v1);
			s_circle2.append(v4);
			s_circle2.append(v3);
		}

		s_mark.setPrimitiveType(sf::Lines);
		bool state = true;
		for (float i = -0.43633; i <= 0.43633; i += 0.08726)//5 deg
		{
			s_mark.append(sf::Vertex(sf::Vector2f(
				std::cosf(i)*250.0*scale,
				std::sinf(i)*250.0*scale
			), sf::Color::White));
			s_mark.append(sf::Vertex(sf::Vector2f(
				std::cosf(i)*(state ? 245.0 : 240.0)*scale,
				std::sinf(i)*(state ? 245.0 : 240.0)*scale
			), sf::Color::White));
			s_mark.append(sf::Vertex(sf::Vector2f(
				std::cosf(i + Constants::pi)*250.0*scale,
				std::sinf(i + Constants::pi)*250.0*scale
			), sf::Color::White));
			s_mark.append(sf::Vertex(sf::Vector2f(
				std::cosf(i + Constants::pi)*(state ? 245.0 : 240.0)*scale,
				std::sinf(i + Constants::pi)*(state ? 245.0 : 240.0)*scale
			), sf::Color::White));
			state = !state;
		}

		r_circle.setPrimitiveType(sf::TrianglesStrip);
		r_circle2.setPrimitiveType(sf::TrianglesStrip);
		for (float i = -0.218165 - 0.08726; i < 0.218165; i += 0.08726)//5 deg
		{
			const float next = i + 0.08726;

			sf::Vertex v1(sf::Vector2f(std::cosf(i)*230.0*scale, std::sinf(i)*230.0*scale), sf::Color::White); //Outer current
			sf::Vertex v2(sf::Vector2f(std::cosf(next)*229.0*scale, std::sinf(next)*229.0*scale), sf::Color::White); //Inner current
			sf::Vertex v3(sf::Vector2f(std::cosf(i)*230.0*scale, std::sinf(i)*230.0*scale), sf::Color::White); //Outer next
			sf::Vertex v4(sf::Vector2f(std::cosf(next)*229.0*scale, std::sinf(next)*229.0*scale), sf::Color::White); //Outer next

			r_circle.append(v2);
			r_circle.append(v1);
			r_circle.append(v4);
			r_circle.append(v3);

			v1 = sf::Vertex(sf::Vector2f(std::cosf(i + Constants::pi)*230.0*scale, std::sinf(i + Constants::pi)*230.0*scale), sf::Color::White); //Outer current
			v2 = sf::Vertex(sf::Vector2f(std::cosf(next + Constants::pi)*229.0*scale, std::sinf(next + Constants::pi)*229.0*scale), sf::Color::White); //Inner current
			v3 = sf::Vertex(sf::Vector2f(std::cosf(i + Constants::pi)*230.0*scale, std::sinf(i + Constants::pi)*230.0*scale), sf::Color::White); //Outer next
			v4 = sf::Vertex(sf::Vector2f(std::cosf(next + Constants::pi)*229.0*scale, std::sinf(next + Constants::pi)*229.0*scale), sf::Color::White); //Outer next

			r_circle2.append(v2);
			r_circle2.append(v1);
			r_circle2.append(v4);
			r_circle2.append(v3);
		}

		r_mark.setPrimitiveType(sf::Lines);
		r_mark.append(sf::Vertex(sf::Vector2f(230.0*scale, 0.0), sf::Color::White));
		r_mark.append(sf::Vertex(sf::Vector2f(235.0*scale, 0.0), sf::Color::White));
		r_mark.append(sf::Vertex(sf::Vector2f(-230.0*scale, 0.0), sf::Color::White));
		r_mark.append(sf::Vertex(sf::Vector2f(-235.0*scale, 0.0), sf::Color::White));

		setPosition(sf::Vector2f(resolution.x / 2.0, resolution.y / 2.0));
	}

	static float sign(float value)
	{
		return value > 0 ? 1.0 : -1.0;
	}

	virtual void Update()
	{
		if ((int)angle != (int)target)
		{
			float diff = target - angle;
			if (diff < -180) diff += 360;
			else if (diff > 180) diff -= 360;

			const float t = framerate.restart().asMicroseconds() / 1000000.0;
			angle += Functions::Max<float>(Functions::Min<float>(std::pow(std::abs(diff) + 1.0, 2.0)*t, std::abs(diff)), 0.003)*sign(diff);
			if (angle < 0) angle += 360;
			else if (angle >= 360) angle -= 360;
		}
	}

	const void Set(float value)
	{
		target = std::fmod(value, 360.f) + (value < 0 ? 360.f : 0.f);
		framerate.restart();
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		states.transform = getTransform();
		target.draw(s_circle, states);
		target.draw(s_circle2, states);
		target.draw(s_mark, states);
		states.transform.rotate(angle);
		target.draw(r_circle, states);
		target.draw(r_circle2, states);
		target.draw(r_mark, states);

	}
};