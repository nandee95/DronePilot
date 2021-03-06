#pragma once

#include <SFML/Graphics.hpp>

class Hud_Canvas : public sf::Drawable, public sf::Transformable
{
	sf::RectangleShape canvas;

	sf::Texture texture;
	sf::Shader bgra;
	bool active = false;
public:

	Hud_Canvas(const sf::Vector2i& resolution,const sf::Vector2i camresolution)
	{
		texture.create(camresolution.x, camresolution.y);

		float scale = 1.f;
		if ((float)resolution.x / resolution.y > (float)camresolution.x / camresolution.y)
		{
			scale = (float)resolution.y / camresolution.y;

			canvas.setPosition(sf::Vector2f(resolution.x / 2.f - camresolution.x*scale / 2.f,0.f));
		}
		else
		{
			scale = (float)resolution.x / camresolution.x;

			canvas.setPosition(sf::Vector2f(0.f,resolution.y / 2.f - camresolution.y*scale / 2.f));
		}
		canvas.setSize(sf::Vector2f(camresolution.x*scale, camresolution.y*scale));



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

