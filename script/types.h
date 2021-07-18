#pragma once

#include <Windows.h>

typedef int Entity;
typedef int Ped;
typedef int Vehicle;
typedef int Cam;
typedef int Group;
typedef int Pickup;
typedef int Object;
typedef int Weapon;
typedef int Blip;
typedef int Camera;
typedef int ScrHandle;
typedef int FireId;
typedef int Rope;
typedef int Interior;
typedef unsigned int Player;
typedef unsigned long Hash;
typedef unsigned long Void;
typedef unsigned long Any;

struct Request_s
{
	int index;
	int unk;
};

typedef struct Vector3
{
	float x, y, z;

	static Vector3 zero()
	{
		Vector3 newVec;

		newVec.x = 0.0f;
		newVec.y = 0.0f;
		newVec.z = 0.0f;
		
		return newVec;
	}

	bool operator==(Vector3 b)
	{
		return x == b.x && y == b.y && z == b.z;
	}

	bool operator!=(Vector3 b)
	{
		return x != b.x || y != b.y || z != b.z;
	}

	Vector3 operator+(Vector3 b)
	{
		Vector3 newVec;

		newVec.x = x + b.x;
		newVec.y = y + b.y;
		newVec.z = z + b.z;

		return newVec;
	}

	Vector3 operator-(Vector3 b)
	{
		Vector3 newVec;

		newVec.x = x - b.x;
		newVec.y = y - b.y;
		newVec.z = z - b.z;

		return newVec;
	}

	Vector3 operator*(float multiplier)
	{
		Vector3 newVec;

		newVec.x = x * multiplier;
		newVec.y = y * multiplier;
		newVec.z = z * multiplier;

		return newVec;
	}

	Vector3 normalize()
	{
		Vector3 newVec;

		float mag = (float) (sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)));
		newVec.x = x / mag;
		newVec.y = y / mag;
		newVec.z = z / mag;

		return newVec;
	}

	static Vector3 lerp(Vector3& A, Vector3& B, float t) {
		return A * t + B * (1.f - t);
	}

	bool IsZero()
	{
		return (x == 0 && y == 0 && z == 0);
	}
} Vector3;