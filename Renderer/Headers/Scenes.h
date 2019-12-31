#pragma once

// Defs for controls... TEMPORARY HACK
#define DOM_W 0x57
#define DOM_S 0x53
#define DOM_A 0x41

#define DOM_D 0x44
#define DOM_Q 0x51
#define DOM_E 0x45

#define DOM_0 0x30
#define DOM_1 0x31
#define DOM_2 0x32
#define DOM_3 0x33
#define DOM_4 0x34
#define DOM_5 0x35
#define DOM_6 0x36
#define DOM_7 0x37
#define DOM_8 0x38
#define DOM_9 0x39

#include "Time.h"


// Gravity Function
fizz::vec6 gravityFunction(const fizz::RigidMotion3d& Position, const fizz::vec6& v, const fizz::RigidBody3d& Body)
{
	const glm::vec3 gravity(0.0f, -2.0f, 0.0f);
	return fizz::vec6(Body.mass() * gravity, glm::vec3());
};


/////////////////
// Scene Class //
/////////////////

class Scene
{
public:
	virtual void draw(MainShader&, DepthMapShader&) const = 0;
	virtual void update() = 0;
	virtual void updateSim(const float) = 0;
};


class TestScene : public Scene
{
public:

	fizz::Simulation3d sim;

	std::vector<Sphere> spheres;
	std::vector<Cube> cubes;

	Clock clock;

	// Camera & Matrix for Projection and View
	Camera Cam;

	const float CamVelocity = 2.0f;
	const float CamAngularVelocity = 1.0f;

	double cooldown;

	// Light
	Renderer::Light Ambient, Point;
	float Exposure;
	float Contrast;
	glm::vec3 ColourFilter;

public:

	TestScene(const size_t SphereCount, const size_t CubeCount) :
		sim(SphereCount, CubeCount, gravityFunction),
		Cam(0.0f, 0.0f, 10.0f),
//		Point(Renderer::Light::Point(glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(1.0f), 10.0f)),
		Point(Renderer::Light::Spot(glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),  glm::vec3(1.0f), 10.f, 45.0f)),
//		Point(Renderer::Light::Directional(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f), 3.0f)),
		Exposure(2.0f),
		Contrast(1.0f),
		ColourFilter(1.0f)
	{
		spheres.reserve(SphereCount);
		cubes.reserve(CubeCount);

		cooldown = 0.0;

		cubes.emplace_back(64, glm::vec3(0.0f, -4.0f, 0.0f), 2.0f);
		cubes.back().PhysicsIndex = sim.attach_static(fizz::Trafo3d::Translation(cubes.back().Position),/* cubes.back().HitBox,*/ fizz::Material::Wood, fizz::State::active());
	}

	void update()
	{
		// Update clock, cooldown and view matrix (from clock)
		clock.update();
		cooldown += clock.deltaTime();

		// Update the render matrices of the spheres from the transformation of the physics component
		for (Sphere& sphere : spheres) 
		{
			sphere.updateMatrices(sim.dynamic_transformation(sphere.PhysicsIndex));
		}

		// Update the render matrices of the cubes from the transformation of the physics component
		for (Cube& cube : cubes) 
		{
			cube.updateMatrices(sim.static_transformation(cube.PhysicsIndex));
		}


		//////////////
		// Controls //
		//////////////

		// LEFT / RIGHT: turn left / right
		if (((GetKeyState(VK_LEFT) >> 15)&(short)1) == (short)1) {
			Cam.yaw(static_cast<float>(CamAngularVelocity * clock.deltaTime()));
//			Cam.turn(CamAngularVelocity * clock.deltaTime());
		}

		if (((GetKeyState(VK_RIGHT) >> 15)&(short)1) == (short)1) {
			Cam.yaw(static_cast<float>(-CamAngularVelocity * clock.deltaTime()));
//			Cam.turn(-CamAngularVelocity * clock.deltaTime());
		}

		// UP / DOWN: pitch up / down
		if (((GetKeyState(VK_UP) >> 15)&(short)1) == (short)1)	{
			Cam.pitch(static_cast<float>(CamAngularVelocity * clock.deltaTime()));
		}

		if (((GetKeyState(VK_DOWN) >> 15)&(short)1) == (short)1) {
			Cam.pitch(static_cast<float>(-CamAngularVelocity * clock.deltaTime()));
		}

		// W & S: forward / backward
		if (((GetKeyState(DOM_S) >> 15)&(short)1) == (short)1) {
			Cam.move_forward(static_cast<float>(-CamVelocity * clock.deltaTime()));
		}

		if (((GetKeyState(DOM_W) >> 15)&(short)1) == (short)1) {
			Cam.move_forward(static_cast<float>(CamVelocity * clock.deltaTime()));
		}

		// A & D: left / right
		if (((GetKeyState(DOM_A) >> 15)&(short)1) == (short)1) {
			Cam.move_right(static_cast<float>(-CamVelocity * clock.deltaTime()));
		}

		if (((GetKeyState(DOM_D) >> 15)&(short)1) == (short)1) {
			Cam.move_right(static_cast<float>(CamVelocity * clock.deltaTime()));
		}

		// Q & E: roll left / right
		if (((GetKeyState(DOM_Q) >> 15)&(short)1) == (short)1) {
			Cam.roll(static_cast<float>(-CamAngularVelocity * clock.deltaTime()));
		}

		if (((GetKeyState(DOM_E) >> 15)&(short)1) == (short)1) {
			Cam.roll(static_cast<float>(CamAngularVelocity * clock.deltaTime()));
		}


		// Shift & Control: up / down
		if (((GetKeyState(VK_SHIFT) >> 15)&(short)1) == (short)1) {
			Cam.move_by(glm::vec3(0.0f, static_cast<float>(CamVelocity * clock.deltaTime()), 0.0f));
		}
		
		if (((GetKeyState(VK_CONTROL) >> 15)&(short)1) == (short)1) {
			Cam.move_by(glm::vec3(0.0f, static_cast<float>(-CamVelocity * clock.deltaTime()), 0.0f));
		}

		// Look at center of the scene
		if (((GetKeyState(VK_SPACE) >> 15)&(short)1) == (short)1) {
			Cam.look_at(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}


		//////////////
		// GRAPHICS //
		//////////////

		// Increase f-stop exposure
		if (((GetKeyState(VK_NUMPAD6) >> 15)&(short)1) == (short)1) {
			Exposure += 0.05f;
		}

		// Decrease f-stop exposure
		if (((GetKeyState(VK_NUMPAD4) >> 15)&(short)1) == (short)1) {
			Exposure -= 0.05f;
		}

		// Increase contrast
		if (((GetKeyState(VK_NUMPAD8) >> 15)&(short)1) == (short)1)	{
			Contrast += 0.01f;
		}

		// Decrease contrast
		if (((GetKeyState(VK_NUMPAD2) >> 15)&(short)1) == (short)1)	{
			Contrast -= 0.01f;
		}

		// Reset contrast & exposure
		if (((GetKeyState(VK_NUMPAD5) >> 15)&(short)1) == (short)1)	{
			Contrast = 1.0f;
			Exposure = 2.0f;
		}

		// Clamp contrast and exposure to some sensible values
		Exposure = clamp(Exposure, -16.0f, 16.0f);
		Contrast = clamp(Contrast, 0.0f, 10.0f);

		///////////
		// Mouse //
		///////////

		if (((GetKeyState(VK_LBUTTON) >> 15)&(short)1) == (short)1 && (cooldown > 0.5) && sim.can_attach_dynamic()) 
		{
			cooldown = 0.0;
			spheres.emplace_back(25, Cam * glm::vec3(0.0f, 0.0f, -5.0f), 1.0f);
			spheres.back().PhysicsIndex = sim.attach_dynamic(fizz::Trafo3d::Translation(spheres.back().Position),/* spheres.back().HitBox,*/ fizz::Material::Wood, fizz::State::active());
		}

		if (((GetKeyState(VK_RBUTTON) >> 15)&(short)1) == (short)1 && (cooldown > 0.5) && sim.can_attach_static()) 
		{
			cooldown = 0.0;
			cubes.emplace_back(2, Cam * glm::vec3(0.0f, 0.0f, -5.0f), 1.0f);
			cubes.back().PhysicsIndex = sim.attach_static(fizz::Trafo3d::Translation(cubes.back().Position),/*cubes.back().HitBox,*/ fizz::Material::Wood, fizz::State::active());
		}
	}

	void draw(MainShader& main, DepthMapShader& shadow) const
	{
		Renderer::PrepareDepthMapPass();
		generateDepthMap(shadow);
		Renderer::PrepareDrawPass(static_cast<glm::mat4>(Cam));
		drawdraw(main);
		Renderer::PostProcessPass(Exposure, 2.2f, Contrast, ColourFilter);
	}

	void updateSim(const float dt)
	{
		sim.update(dt);
	}


	// Generate depth map
	void generateDepthMap(DepthMapShader& shader) const
	{
		shader.setLightPosition(Point.Position);

		for (const Cube& cube : cubes) 
		{
			cube.drawShadow(shader);
		}

		for (const Sphere& sphere : spheres) 
		{
			sphere.drawShadow(shader);
		}
	}


	// Draw the scene
	void drawdraw(MainShader& shader) const
	{
		shader.setLight(Point);
		shader.setCameraPosition(Cam.position());

		for (const Sphere& sphere : spheres) 
		{
			sphere.draw(shader);
		}

		for (const Cube& cube : cubes) 
		{
			cube.draw(shader);
		}
	}
};