#include "OpenGL.h"
#include "MainShader.h"
#include "Texture.h"


struct Vertex 
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};


class Mesh
{
	public:
		
		std::vector<Vertex> Vertices;
		std::vector<GLuint> Indices;
		std::vector<std::shared_ptr<Texture>> Textures;
		
		GLuint VAO, VBO, EBO;
	

		/////////////////
		// Constructor //
		/////////////////
		
		Mesh(const vector<Vertex>& vertices, const vector<GLuint>& indices) :
			Vertices(vertices),
			Indices(indices)
		{
			setup_mesh();
		}
	
	
	    void setup_mesh()
		{
			// Generate 
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			
			// (Ab)use alignment of the vertex struct that decays to a
			glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);  

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);


			//////////////////////////////////////
			// Enable vertex attribute pointers //
			//////////////////////////////////////
			
			// Vertex Positions
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			
			// Vertex Normals
			glEnableVertexAttribArray(1);	
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
			
			// Vertex Texture Coordinates
			glEnableVertexAttribArray(2);	
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
			
			/* Additional possible components of a vertex: 
			
			// Vertex tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    
			// vertex bitangent
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

			*/
			
			// Unbind
			glBindVertexArray(0);
		}
			
			
		/////////////
		// Drawing //
		/////////////
		
		void draw(MainShader& shader)
		{        
			// Indexed drawing 
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

			// Unbind buffer and texture after drawing
			glBindVertexArray(0);
			glActiveTexture(GL_TEXTURE0);
		}	
};