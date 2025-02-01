#include "Player.h"

void Player::initialize(glm::vec2 aabb_size)
{
	position = glm::vec2(0, 0);
	velocity = glm::vec2(0, 0);

	aabb.initialize(position, aabb_size);

	flip_x = false;

	jump_sound = LoadSound(RESOURCES_PATH "sounds/jump.wav");
}

void Player::get_input(GLFWwindow* window, float delta_time)
{
	int input = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) ? 1 : (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) ? -1 : 0;

	if (aabb.normal.y == 1)
	{
		auto lerp = [](float v0, float v1, float t)
		{
			return (1 - t) * v0 + t * v1;
		};

		velocity.x = lerp(velocity.x, input * PLAYER_SPEED, 0.2f);
	}

	velocity.y -= PLAYER_GRAVITY * delta_time;

	if (velocity.y >= 500)
	{
		velocity.y = 500;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (aabb.normal.y == 1 && velocity.y > -200)
		{
			if (velocity.x >  PLAYER_SPEED / 10) velocity.x =  PLAYER_SPEED;
			if (velocity.x < -PLAYER_SPEED / 10) velocity.x = -PLAYER_SPEED;

			velocity.y = PLAYER_JUMP_FORCE;
			aabb.normal = glm::vec2(0, 0);
			PlaySound(jump_sound);
		}
		else if (aabb.normal.x != 0)
		{
			velocity.x = aabb.normal.x * PLAYER_SPEED;
			velocity.y = PLAYER_JUMP_FORCE;
			aabb.normal = glm::vec2(0, 0);
			PlaySound(jump_sound);
		}
	}
	
	if (velocity.x > 0) flip_x = false; else if (velocity.x < 0) flip_x = true;
}

void Player::update_velocity(float delta_time)
{
	position += velocity * delta_time;
	aabb.attach(position);
}