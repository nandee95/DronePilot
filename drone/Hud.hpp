#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

class Hud : public sf::Drawable, public sf::Transformable
{
public:
	virtual void Update() = 0;
};

class Hud_InverseShader
{
public:
	sf::RenderStates states;
	sf::Shader shader;
	Hud_InverseShader(const sf::Vector2i& resolution,const sf::Vector2i& camResolution)
	{
		//Precalculated values
		float ratio = (float)resolution.x / resolution.y;
		float camratio = (float)camResolution.x / camResolution.y;

		std::string code = "", code2 = "";
		float scaleratio = 1;
		sf::Vector2f correction;
		if (ratio > camratio)
		{
			scaleratio = (float)resolution.y / camResolution.y;
			float zone = resolution.x / 2.f - camResolution.x*scaleratio / 2.f;
			code += "gl_FragCoord.x < " + std::to_string(zone) + " || gl_FragCoord.x > " + std::to_string(resolution.x - zone);
			code2 = "screenpos.x-=" + std::to_string(zone) + ";\n";
			correction = sf::Vector2f(camResolution.x*scaleratio, resolution.y);
		}
		else
		{
			scaleratio = (float)resolution.x / camResolution.x;
			float zone = resolution.y / 2.f - camResolution.y*scaleratio / 2.f;
			code += "gl_FragCoord.y < " + std::to_string(zone) + " || gl_FragCoord.y > " + std::to_string(resolution.y - zone);
			code2 = "screenpos.y-=" + std::to_string(zone) + ";\n";
			correction = sf::Vector2f(resolution.x, camResolution.y*scaleratio);
		}


		shader.loadFromMemory(
		"#version 120 \n \
			void main() \
			{ \
				gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; \
				gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0; \
				gl_FrontColor = gl_Color; \
			}",
		"#version 120 \n \
			uniform sampler2D texture; \n \
			uniform bool camera; \n \
			vec3 filter(vec3 color) \n \
			{\n \
				float d = (color.r + color.g + color.b) / 3.0 < 0.5 ? 1 : 0; \n \
				return vec3(d, d, d); \n \
			}\n \
			void main() \n \
			{ \n \
				if(!camera || " + code + ") { \
					gl_FragColor = vec4(1,1,1,1); \
					return; \
				} \
				vec2 screenpos = gl_FragCoord.xy; \n \
				" + code2 + " \
				vec2 pos = screenpos/vec2(" + std::to_string(correction.x) + "," + std::to_string(correction.y) + "); \n \
				vec4 pixel = texture2D(texture, vec2(pos.x,1-pos.y)); \n \
				gl_FragColor= vec4(filter(pixel.rgb),1.0); \n \
			}");

		states.shader = &shader;
	}
};