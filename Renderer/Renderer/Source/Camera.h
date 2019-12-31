#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera 
{
	private:

		glm::vec3 Position;					// vec3 as coordinates of the camera
		glm::quat Orientation;				// unit quaternion for the orientation of the camera: rotates the ... into the world coordinates
		
	public:
	
		// Constructor from translation vector & unit quaternion that defaults to the origin and identity quaternion
		Camera(
			const glm::vec3& CamPosition = glm::vec3(),
			const glm::quat& CamOrientation	= glm::quat()
		) :
			Position(CamPosition),
			Orientation(glm::normalize(CamOrientation))
		{}


		// Camera in given position with identity rotation
		Camera(float x, float y, float z) :
			Position(x, y, z),
			Orientation()
		{}

		// Default copy, assignment, move and destructor are fine
		Camera(const Camera&)				= default;
		Camera(Camera&&)					= default;
		Camera& operator=(const Camera&)	= default;
		Camera& operator=(Camera&&)			= default;
		~Camera()							= default;
	

		// Getters
		inline const glm::vec3& position() const		{ return Position; }
		inline const glm::quat& orientation() const		{ return Orientation; }

		// Get directions of the camera orientation in world coordinates
		glm::vec3 forward() const 	{ return Orientation * glm::vec3(0.0f, 0.0f, -1.0f); }
		glm::vec3 up() const		{ return Orientation * glm::vec3(0.0f, 1.0f, 0.0f); }
		glm::vec3 right() const		{ return Orientation * glm::vec3(1.0, 0.0f, 0.0f); }
		
	
		///////////////////////
		// Moving & Rotating //
		///////////////////////

		// Move 
		void move_to(const glm::vec3& NewPosition)				{ Position = NewPosition; }									// Move cam to given position
		void move_by(const glm::vec3& Displacement)				{ Position += Displacement;	}								// Move cam by given (world) displacement
		void oriented_move_by(const glm::vec3& Displacement)	{ Position += Orientation * Displacement; }					// Move cam in cam coordinates (c.f. forward et cetera)
		
		// Move with respect to the camera axes
		void move_forward(float s) 	{ Position += Orientation * glm::vec3(0.0f, 0.0f, -s); }	// Move along camera axis: positive values forward, negative backwards
		void move_right(float s) 	{ Position += Orientation * glm::vec3(s, 0.0f, 0.0f); }		// Strafe: positive values move right, negative left
		void move_up(float s) 		{ Position += Orientation * glm::vec3(0.0f, s, 0.0f); }		// Move up: positive values up, negative values down
		
		// Rotate
		void rotate_to(const glm::quat& NewOrientation)			{ Orientation = glm::normalize(NewOrientation);	}									// Absolute rotation
		void rotate_by(float Angle, const glm::vec3& Axis)		{ Orientation = glm::normalize(glm::angleAxis(Angle, Axis) * Orientation); }		// Incremental rotation
		
		// Turn around y-axis
		void turn(float s)		{ Orientation = glm::normalize(glm::angleAxis(s, glm::vec3(0.0f, 1.0f, 0.0f)) * Orientation); }

		// Incremental Roll, Pitch & Yaw
		void roll(float s)		{ Orientation = glm::normalize(glm::angleAxis(s, Orientation * glm::vec3(0.0f, 0.0f, -1.0f)) * Orientation); }			//	Positive values roll right, negative left
		void pitch(float s)		{ Orientation = glm::normalize(glm::angleAxis(s, Orientation * glm::vec3(1.0f, 0.0f, 0.0f)) * Orientation); }			//	Positive values pitch up, negative down
		void yaw(float s)		{ Orientation = glm::normalize(glm::angleAxis(s, Orientation * glm::vec3(0.0f, 1.0f, 0.0f)) * Orientation); }			//	Positive input turns left, negative right	

		// Look at a given point with specified up direction 
		// Problems occur if a) the distance between target and camera is very small or b) when the Up direction is 
		void look_at(const glm::vec3& Target, const glm::vec3& Up = glm::vec3(0.0f, 1.0f, 0.0f))
		{
			glm::mat3 Result(glm::uninitialize);
			Result[2] = glm::normalize(Position - Target);
			Result[0] = glm::normalize(glm::cross(Up, Result[2]));
			Result[1] = glm::cross(Result[2], Result[0]);					
			Orientation = glm::quat_cast(Result);
		}


		// Look at a given point from a given position with a specified up direction 
		// Problems occur if a) the distance between target and camera is very small or b) when the Up direction is 
		void look_at_from(const glm::vec3& Eye, const glm::vec3& Target, const glm::vec3& Up = glm::vec3(0.0f, 1.0f, 0.0f))
		{
			Position = Eye;
			glm::mat3 Result(glm::uninitialize);
			Result[2] = glm::normalize(Position - Target);
			Result[0] = glm::normalize(glm::cross(Up, Result[2]));
			Result[1] = glm::cross(Result[2], Result[0]);					
			Orientation = glm::quat_cast(Result);
		}


		/////////////////////
		// Type conversion //
		/////////////////////
		
		// Type conversion to 4x4 matrix representation ("view matrix", World->Camera)
		explicit operator glm::mat4() const 
		{
			return glm::translate(glm::mat4_cast(glm::conjugate(Orientation)), -Position);
		}
		

		/////////////
		// Friends //
		/////////////

		// Action on vector
		friend glm::vec3 operator*(const Camera&, const glm::vec3&);		// Left-multiplication with vector: transform camera coordinates to world coordinates
		friend glm::vec3 operator*(const glm::vec3&, const Camera&);		// Right-multiplication with vector: transform world coordinates to camera coordinates

		
		// Interpolation
		friend Camera Slerp(const Camera&, const Camera&, const float);
		friend Camera Lerp(const Camera&, const Camera&, const float);
};



/////////////
// Friends //
/////////////

// Left-multiplication with vector: transform camera coordinates to world coordinates
inline glm::vec3 operator*(const Camera& lhs, const glm::vec3& rhs)
{
	return lhs.Orientation * rhs + lhs.Position;
}

// Right-multiplication with vector: transform world coordinates to camera coordinates
inline glm::vec3 operator*(const glm::vec3& lhs, const Camera& rhs)
{
	return glm::conjugate(rhs.Orientation) * (lhs - rhs.Position);
}



// Interpolate between transformations: for t in [0, 1] linearly interpolate in both, but do spherical interpolation for the quaternions
Camera Slerp(const Camera& A, const Camera& B, float t) 
{
	return Camera(A.Position + t * (B.Position - A.Position), glm::slerp(A.Orientation, B.Orientation, t));
}


// Interpolate between transformations: for t in [0, 1] linearly interpolate in both, but do spherical interpolation for the quaternions
Camera Lerp(const Camera& A, const Camera& B, float t)
{
	return Camera(A.Position + t * (B.Position - A.Position), glm::normalize(t * A.Orientation + (1.0f - t) * B.Orientation));
}