// render - A C++ OpenGL library
// Copyright (C) 2022  Intrets
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// Obtained and slightly modified from
// http://www.opengl-tutorial.org/download/
// https://github.com/opengl-tutorials/ogl/tree/master/common

#include "loaders/ModelLoader.h"

#include <vector>
#include <numeric>

#include <misc/Num.h>

#include <cstdlib>

namespace render
{
	bool loadOBJ(
		std::unique_ptr<Buffer> const& buffer,
		std::vector<render::Vertex3>& out_vertices,
		std::vector<render::Vertex2>& out_uvs,
		std::vector<render::Vertex3>& out_normals
	) {
		std::stringstream data;
		data.write(buffer->getData<char>(), buffer->getSize<char>());

		std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_uvs;
		std::vector<glm::vec3> temp_normals;


		while (1) {
			std::string lineHeader;
			// read the first word of the line
			//int res = fscanf(file, "%s", lineHeader);
			data >> lineHeader;

			if (data.eof()) {
				break;
			}

			if (lineHeader == "v") {
				glm::vec3 vertex;
				data >> vertex.x;
				data >> vertex.y;
				data >> vertex.z;
				temp_vertices.push_back(vertex);
			}
			else if (lineHeader == "vt") {
				glm::vec2 uv;
				data >> uv.x;
				data >> uv.y;
				uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
				temp_uvs.push_back(uv);
			}
			else if (lineHeader == "vn") {
				glm::vec3 normal;
				data >> normal.x;
				data >> normal.y;
				data >> normal.z;
				temp_normals.push_back(normal);
			}
			else if (lineHeader == "f") {
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				for (size_t i = 0; i < 3; i++) {
					data >> vertexIndex[i];
					data.get();
					data >> uvIndex[i];
					data.get();
					data >> normalIndex[i];
				}

				if (!data.good()) {
					printf("File can't be read by our simple parser :-( Try exporting with other options\n");
					return false;
				}

				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
			else {
				// Probably a comment, eat up the rest of the line
				char sBuffer[1000];
				data.getline(sBuffer, 1000);
			}

		}

		// For each vertex of each triangle
		for (unsigned int i = 0; i < vertexIndices.size(); i++) {

			// Get the indices of its attributes
			unsigned int vertexIndex = vertexIndices[i];
			unsigned int uvIndex = uvIndices[i];
			unsigned int normalIndex = normalIndices[i];

			// Get the attributes thanks to the index
			glm::vec3 vertex = temp_vertices[vertexIndex - 1];
			glm::vec2 uv = temp_uvs[uvIndex - 1];
			glm::vec3 normal = temp_normals[normalIndex - 1];

			// Put the attributes in buffers
			out_vertices.push_back({ vertex });
			out_uvs.push_back({ uv });
			out_normals.push_back({ normal });

		}
		return true;
	}

	std::optional<IndexedModel> loadOBJ_indexed(std::unique_ptr<Buffer> const& buffer) {
		std::stringstream data;
		data.write(buffer->getData<char>(), buffer->getSize<char>());

		IndexedModel result{};

		while (1) {
			std::string lineHeader;
			// read the first word of the line
			//int res = fscanf(file, "%s", lineHeader);
			data >> lineHeader;

			if (data.eof()) {
				break;
			}

			if (lineHeader == "v") {
				glm::vec3 vertex;
				data >> vertex.x;
				data >> vertex.y;
				data >> vertex.z;
				result.vertices.data.push_back({ vertex });
			}
			else if (lineHeader == "vt") {
				glm::vec2 uv;
				data >> uv.x;
				data >> uv.y;
				uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
				result.uvs.data.push_back({ uv });
			}
			else if (lineHeader == "vn") {
				glm::vec3 normal;
				data >> normal.x;
				data >> normal.y;
				data >> normal.z;
				result.normals.data.push_back({ normal });
			}
			else if (lineHeader == "f") {
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				for (size_t i = 0; i < 3; i++) {
					data >> vertexIndex[i];
					data.get();
					data >> uvIndex[i];
					data.get();
					data >> normalIndex[i];
				}

				if (!data.good()) {
					printf("File can't be read by our simple parser :-( Try exporting with other options\n");
					return std::nullopt;
				}

				result.vertices.indices.push_back(vertexIndex[0] - 1);
				result.vertices.indices.push_back(vertexIndex[1] - 1);
				result.vertices.indices.push_back(vertexIndex[2] - 1);

				result.uvs.indices.push_back(uvIndex[0] - 1);
				result.uvs.indices.push_back(uvIndex[1] - 1);
				result.uvs.indices.push_back(uvIndex[2] - 1);
				result.normals.indices.push_back(normalIndex[0] - 1);
				result.normals.indices.push_back(normalIndex[1] - 1);
				result.normals.indices.push_back(normalIndex[2] - 1);
			}
			else {
				// Probably a comment, eat up the rest of the line
				char sBuffer[1000];
				data.getline(sBuffer, 1000);
			}

		}

		return result;
	}
}