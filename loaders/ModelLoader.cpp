#include "loaders/ModelLoader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#include "loaders/vboindexer.h"


namespace render
{
	bool loadOBJ(
		std::string& path,
		std::vector<glm::vec3>& out_vertices,
		std::vector<glm::vec2>& out_uvs,
		std::vector<glm::vec3>& out_normals
	) {
		printf("Loading OBJ file %s...\n", path.c_str());

		std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
		std::vector<glm::vec3> temp_vertices;
		std::vector<glm::vec2> temp_uvs;
		std::vector<glm::vec3> temp_normals;


		FILE* file; //= fopen(path.c_str(), "r");
		if (fopen_s(&file, path.c_str(), "r") != 0) {
			printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
			getchar();
			return false;
		}

		while (1) {
			char lineHeader[128];
			// read the first word of the line
			//int res = fscanf(file, "%s", lineHeader);
			int res = fscanf_s(file, "%s", lineHeader, 128);
			if (res == EOF)
				break; // EOF = End Of File. Quit the loop.

					   // else : parse lineHeader

			if (strcmp(lineHeader, "v") == 0) {
				glm::vec3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0) {
				glm::vec2 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0) {
				glm::vec3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0) {
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9) {
					printf("File can't be read by our simple parser :-( Try exporting with other options\n");
					fclose(file);
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
				fgets(sBuffer, 1000, file);
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
			out_vertices.push_back(vertex);
			out_uvs.push_back(uv);
			out_normals.push_back(normal);

		}
		fclose(file);
		return true;
	}

	bool loadModel(std::string& path,
				   GLuint& vertexbuffer,
				   GLuint& uvbuffer,
				   GLuint& normalbuffer,
				   GLuint& indexbuffer,
				   int32_t& indexsize) {
		std::vector< glm::vec3 > vertices;
		std::vector< glm::vec2 > uvs;
		std::vector< glm::vec3 > normals; // Won't be used at the moment.

		loadOBJ(path, vertices, uvs, normals);

		std::vector< unsigned short> out_indices;
		std::vector< glm::vec3 > out_vertices;
		std::vector< glm::vec2 > out_uvs;
		std::vector< glm::vec3 > out_normals; // Won't be used at the moment.

		indexVBO(vertices, uvs, normals, out_indices, out_vertices, out_uvs, out_normals);

		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, out_vertices.size() * sizeof(glm::vec3), &out_vertices[0], GL_STATIC_DRAW);

		glGenBuffers(1, &uvbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glBufferData(GL_ARRAY_BUFFER, out_uvs.size() * sizeof(glm::vec2), &out_uvs[0], GL_STATIC_DRAW);

		glGenBuffers(1, &normalbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glBufferData(GL_ARRAY_BUFFER, out_normals.size() * sizeof(glm::vec3), &out_normals[0], GL_STATIC_DRAW);

		indexsize = static_cast<int>(out_indices.size());

		glGenBuffers(1, &indexbuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexsize * sizeof(unsigned short), &out_indices[0], GL_STATIC_DRAW);

		return true;
	}
}