#pragma once

#include <SFML/Graphics.hpp>

class Hud_Canvas : public sf::Drawable, public sf::Transformable
{
	sf::RectangleShape canvas;

	sf::Texture texture;
	sf::Shader bgra;
	bool active = false;
public:

	Hud_Canvas()
	{
		texture.create(800, 600);
		canvas.setSize(sf::Vector2f(800, 600));
		canvas.setTexture(&texture);
		bgra.loadFromMemory(
			"void main() \
		{ \
			gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; \
			gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; \
			gl_FrontColor = gl_Color; \
		}",
			"uniform sampler2D texture; \
		void main() \
		{ \
			vec4 pixel = texture2D(texture, gl_TexCoord[0].xy); \
			gl_FragColor = vec4(pixel.b, pixel.g, pixel.r, 1); \
		}");
	}


	const void Update(const uint8_t* pixels)
	{
		active = true;
		texture.update(pixels, 800, 600, 0, 0);
	}

	const sf::Texture& GetTexture() const
	{
		return texture;
	}

	const void Reset()
	{
		active = false;
	}

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		if (!active) return;
		states.transform = getTransform();
		states.shader = &bgra;
		target.draw(canvas, states);
	}

};

