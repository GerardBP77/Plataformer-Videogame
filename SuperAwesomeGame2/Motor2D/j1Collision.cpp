#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"
#include <iostream>
#include "p2Log.h"

j1Collision::j1Collision() : j1Module()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[COLLIDER_FLOOR][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_PLATAFORM] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_WATER] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_START_WALL] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_FINISH_LEVEL] = false;


	matrix[COLLIDER_PLAYER][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_PLATAFORM] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_WATER] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_START_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_FINISH_LEVEL] = true;


	matrix[COLLIDER_PLATAFORM][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_PLATAFORM][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_PLATAFORM][COLLIDER_PLATAFORM] = false;
	matrix[COLLIDER_PLATAFORM][COLLIDER_WATER] = false;
	matrix[COLLIDER_PLATAFORM][COLLIDER_START_WALL] = false;
	matrix[COLLIDER_PLATAFORM][COLLIDER_FINISH_LEVEL] = false;


	matrix[COLLIDER_WATER][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_WATER][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WATER][COLLIDER_PLATAFORM] = false;
	matrix[COLLIDER_WATER][COLLIDER_WATER] = false;
	matrix[COLLIDER_WATER][COLLIDER_START_WALL] = false;
	matrix[COLLIDER_WATER][COLLIDER_FINISH_LEVEL] = false;


	matrix[COLLIDER_START_WALL][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_START_WALL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_START_WALL][COLLIDER_PLATAFORM] = false;
	matrix[COLLIDER_START_WALL][COLLIDER_WATER] = false;
	matrix[COLLIDER_START_WALL][COLLIDER_START_WALL] = false;
	matrix[COLLIDER_START_WALL][COLLIDER_FINISH_LEVEL] = false;
	

	matrix[COLLIDER_FINISH_LEVEL][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_FINISH_LEVEL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_FINISH_LEVEL][COLLIDER_PLATAFORM] = false;
	matrix[COLLIDER_FINISH_LEVEL][COLLIDER_WATER] = false;
	matrix[COLLIDER_FINISH_LEVEL][COLLIDER_START_WALL] = false;
	matrix[COLLIDER_FINISH_LEVEL][COLLIDER_FINISH_LEVEL] = false;
	
}

// Destructor
j1Collision::~j1Collision()
{}

bool j1Collision::PreUpdate()
{
	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	// Calculate collisions
	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (c1->CheckCollision(c2->rect) == true)
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					c1->callback->OnCollision(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					c2->callback->OnCollision(c2, c1);
			}
		}
	}

	return true;
}

// Called before render is available
bool j1Collision::Update(float dt)
{

	DebugDraw();

	return true;
}

void j1Collision::DebugDraw()
{
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if (debug == false)
		return;

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_NONE: // white
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_FLOOR: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break; 
		case COLLIDER_PLATAFORM: // yellow
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
			break;
		case COLLIDER_WATER: // red
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 255, alpha);
			break;
		case COLLIDER_START_WALL: // magenta
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 204, alpha);
			break;
		case COLLIDER_FINISH_LEVEL: // magenta
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 204, alpha);
			break;		
		}
	}
}

// Called before quitting
bool j1Collision::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Module* callback)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}

	return ret;
}


// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	if ((r.x + r.w <= rect.x) || (r.x >= rect.x + rect.w) || (r.y + r.h <= rect.y) || (r.y >= rect.y + rect.h)) return false;
	else return true;
}

COLLIDER_TYPE j1Collision::DefineType(int type_as_int)
{
	switch (type_as_int)
	{
	case 0:
		return COLLIDER_FLOOR;
	case 1:
		return COLLIDER_PLATAFORM;
	case 2:
		return COLLIDER_WATER;
	case 3:
		return COLLIDER_START_WALL;
	case 4:
		return COLLIDER_FINISH_LEVEL;
	case 5:
		return COLLIDER_PLAYER;
	default:
		return COLLIDER_NONE;
	}
}

