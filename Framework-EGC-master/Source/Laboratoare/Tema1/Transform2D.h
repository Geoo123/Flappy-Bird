#pragma once

#include <include/glm.h>

namespace Transform2D
{
	// Translate matrix
	inline glm::mat3 Translate(float tx, float ty)
	{
		// TODO implement translate matrix

		//return glm::mat3(1);
		return glm::transpose(
			glm::mat3(1, 0, tx,
				0, 1, ty,
				0, 0, 1)
		);
	}

	// Scale matrix
	inline glm::mat3 Scale(float scaleX, float scaleY)
	{
		// TODO implement scale matrix
		//return glm::mat3(1);
		return glm::transpose(
			glm::mat3(scaleX, 0, 0,
				0, scaleY, 0,
				0, 0, 1)
		);
	}

	// Rotate matrix
	inline glm::mat3 Rotate(float radians)
	{
		// TODO implement rotate matrix
		//return glm::mat3(1);
		return glm::transpose(
			glm::mat3(cos(radians), -sin(radians), 0,
				sin(radians), cos(radians), 0,
				0, 0, 1)
		);
	}

	//Oglindire OY
	inline glm::mat3 OglindireOY()
	{
		return glm::mat3(
			-1, 0, 0,
			0, 1, 0,
			0, 0, 1);
	}
}
