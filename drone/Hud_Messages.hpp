#pragma once

#include <SFML/Graphics.hpp>
#include <array>

class Hud_Messages : public sf::Drawable
{
private:
	struct Line
	{
		sf::Text text;
		sf::Clock clock;
		bool visible;
	};
	std::array <Line, 5> lines;
public:

	Hud_Messages()
	{
		for (auto& l : lines)
		{
			l.text.setFont(*ResourceManager::LoadFont("fonts/arial.ttf"));
		}
	}


	virtual void Update()
	{
		for (auto& l : lines)
		{
			if (!l.visible) continue;
			const auto elapsed = l.clock.getElapsedTime().asMilliseconds();
			if (elapsed > 2000)
			{
				auto color = l.text.getFillColor();
				const float mul = (elapsed - 2000.f) / 1000.f;
				if (mul > 1.0f) {
					l.visible = false;
					continue;
				}
				color.a = 255 - mul * 255.f;
				l.text.setFillColor(color);
			}
		}
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		for (const auto& l : lines)
		{
			if (!l.visible) continue;
			target.draw(l.text, states);
		}
	}

	void UpdateResolution(sf::Vector2i resolution,float scale)
	{
		int32_t shift = 0;
		for (auto& l : lines)
		{
			bool empty = l.text.getString().isEmpty();
			if(empty) l.text.setString("W");
			l.text.setPosition({ resolution.x / 2 - resolution.x / 4.f, resolution.y  - 100.f - shift });
			shift += l.text.getLocalBounds().height + 10;
			if (empty) l.text.setString("");
		}
	}

	virtual void AddText(std::string string,sf::Color color)
	{
		for (int i = 4; i >=1; i--)
		{
			if (!lines[i - 1].visible)
			{
				lines[i].visible = false;
				continue;
			}
			lines[i].text.setFillColor(lines[i - 1].text.getFillColor());
			lines[i].text.setString(lines[i - 1].text.getString());
			lines[i].clock = lines[i-1].clock;
			lines[i].visible = true;
		}
		lines[0].text.setString(string);
		lines[0].text.setFillColor(color);
		lines[0].clock.restart();
		lines[0].visible = true;
	}
};