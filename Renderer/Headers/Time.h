#pragma once

#include <chrono>

class Clock 
{
	
	private:
		 
		/////////////////		
		// Data Fields //
		/////////////////
		
		std::chrono::high_resolution_clock::time_point t0;
		double dT;
		double T;
		
	public:
	
		//////////////////
		// Constructors //
		//////////////////
		
		Clock() :
			t0(std::chrono::high_resolution_clock::now()),
			dT(0.0),
			T(0.0)
		{}
		
		///////////////////
		// Other members //
		///////////////////
		
		// Tick
		inline void update(){
			dT = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(std::chrono::high_resolution_clock::now() - t0).count();
			T += dT;
			t0 = std::chrono::high_resolution_clock::now();
		}
		
		// Return the time that passed since the last update of the clock (or the initialization)
		inline double deltaTime() const { return dT; }
		
		// Return the total time passed since the last update
		inline double time() const {return T; }
};



class Timer 
{
	
	private:
	
		std::chrono::high_resolution_clock::time_point t0;
		double dT;
		
	public:
	
		// Constructor
		Timer() :
			t0(std::chrono::high_resolution_clock::now()),
			dT(0.0)
		{}
		
		// Tick
		inline void update()
		{
			dT = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(std::chrono::high_resolution_clock::now() - t0).count();
			t0 = std::chrono::high_resolution_clock::now();
		}
		
		// Return the time that passed since the last update of the clock (or the initialization)
		inline double deltaTime() const { return dT; }
};