#include "Character.h"

float moveXBack;
float moveYBack;

float totalMoveXBack = 0;


Character::Character(map<AnimationType, Movement> _animations, RectangleShape& _body, RectangleShape& _shadow, map<AnimationType, 
	vector<RectangleShape>> hitboxes_, map<AnimationType, vector<RectangleShape>> damage_hitboxes_, bool male, bool _hasSpecial) {
	srand(time(NULL));
	body = _body;
	shadow = _shadow;
	animation_in_process = AnimationType::IDLE;
	animations = _animations;
	hitboxes = hitboxes_;
	isMale = male;
	hasSpecial = _hasSpecial;
	damage_hitboxes = damage_hitboxes_;	

}

void Character::Update(float tiempo, bool secondPlayer) {

	if (looking_at == LookingAt::LEFT && player == 1) {
		backButton = forwButton_aux;
		forwButton = backButton_aux;
	}
	else if (looking_at == LookingAt::RIGHT && player == 1) {
		backButton = backButton_aux;
		forwButton = forwButton_aux;
	}
	else if (looking_at == LookingAt::LEFT && player == 2) {
		backButtonP2 = forwButton_auxP2;
		forwButtonP2 = backButton_auxP2;
	}
	else if (looking_at == LookingAt::RIGHT && player == 2) {
		backButtonP2 = backButton_auxP2;
		forwButtonP2 = forwButton_auxP2;
	}

	if (internalTimer >= updateTime) {
		specialDelay--;
		hitDelay--;

		internalTimer = 0.0f;
		/*global_position = body.getPosition();
		if (DEBUG_POSITION) {
			cout << "X: " << global_position.x << " - Y:" << global_position.y << endl;
			cout << "Origin: " << "(" << body.getOrigin().x << ", " << body.getOrigin().y << ")" << endl;
		}*/

		if (!freeze && !dying) {
			if (!secondPlayer)	CheckAnimation();	// Dependiendo de que ha pulsado el jugador hago una animaci�n u otra
			else CheckAnimationP2();
		}

		DoAnimation();		// Realizo el siguiente frame de la animaci�n

		if (player == 1) {

			global_position += p1PositionOffset;
			p1PositionOffset = Vector2f(0, 0);

			p1Position = getPosition();
		}
		else if (player == 2) {

			global_position += p2PositionOffset;
			p2PositionOffset = Vector2f(0, 0);

			p2Position = getPosition();
		}

		cout << "POS Y: " << global_position.y << endl;

		shadow.setPosition(Vector2f(global_position.x, screenFloorLimit));
		if (global_position.y == screenFloorLimit || (global_position.y < screenFloorLimit && !fallen)) {
			body.setPosition(global_position);
		}
		else if (global_position.y < screenFloorLimit && fallen) {
			body.setPosition(Vector2f(global_position.x, screenFloorLimit));
		}
		else {
			fullReset();
			EndAnimation();
			body.setPosition(Vector2f(global_position.x, screenFloorLimit));
		}
		
	
	}
	else {
		internalTimer += 0.025f;
	}
}


void Character::UpdateIA(float tiempo, Character opponent) {
	if (internalTimer >= updateTime) {
		difficulty_lvl = chosen_difficulty;
		internalTimer = 0.0f;
		specialDelay--;
		hitDelay--;

		
		if (!freeze && !dying) {
			CheckIAAnimation(opponent);
		}
		DoAnimation();

		global_position += p2PositionOffset;
		p2PositionOffset = Vector2f(0, 0);

		//cout << "OFFSET: " << totalMoveXBack << endl;

		shadow.setPosition(Vector2f(global_position.x, screenFloorLimit));
		if (global_position.y == screenFloorLimit || (global_position.y < screenFloorLimit && !fallen)) {
			body.setPosition(global_position);
		}
		else if (global_position.y < screenFloorLimit && fallen) {
			body.setPosition(Vector2f(global_position.x, screenFloorLimit));
		}
		else {
			fullReset();
			EndAnimation();
			body.setPosition(Vector2f(global_position.x, screenFloorLimit));
		}

		p2Position = getPosition();
	}
	else {
		internalTimer += 0.025f;
	}
}

void Character::CheckIAAnimation(Character opponent) {
	ChangeIAState(opponent);
	float distanceBetween = abs(GetXPosition() - opponent.GetXPosition());
	if (!falling && !dying) {
		if (on_air && estado == EstadoIA::MODO_ATAQUE && damageDelay < 0) {
			animations[animation_in_process].animation.ResetAnimation();
			int ataque = rand() % 2;
			sentFlag = false;
			switch (ataque) {
			case 0:
				animation_in_process = AnimationType::PUNCH_FROM_AIR;
				damageDelay = 2;
				music.hit6();
				break;

			case 1:
				animation_in_process == AnimationType::KICK_FROM_AIR;
				damageDelay = 2;
				music.hit7();
				break;
			}
			estado = EstadoIA::ALEJARSE;

		}
		else if (estado == EstadoIA::MODO_ATAQUE && crouching) {
			animations[animation_in_process].animation.ResetAnimation();
			crouching = false;	
			sentFlag = false;
			animation_in_process = AnimationType::KICK_LOW;
			music.hit7();
			estado = EstadoIA::ALEJARSE;
		}
		else if (animation_in_process == AnimationType::DOWN && estado != EstadoIA::PREPAR_AGACHADO && estado != EstadoIA::DEFENSA_SPECIAL) {
			if (!sentFlag) {
				animations[animation_in_process].animation.RecieveFlagEvent();
			}
			sentFlag = true;
		}
		else if ( (animation_in_process == AnimationType::BLOCK || animation_in_process == AnimationType::BLOCK_LOW) && estado != EstadoIA::MODO_DEFENSA ) {
			if (!sentFlag) {
				animations[animation_in_process].animation.RecieveFlagEvent();
				estado = EstadoIA::PREPAR_ATAQUE;
			}
			sentFlag = true;
		}
		else if (!doing_animation) {				
			sentFlag = false;
			blocking = false;
			crouching = false;
			AnimationType animOp = opponent.getAnimation();
			int probabilidad = rand() % 100;

			switch (estado) {
			case EstadoIA::IDLE:
				animation_in_process = AnimationType::IDLE;
				break;

			case EstadoIA::ACERCARSE:
				if (difficulty_lvl == DifficultyLevel::HARD) {
					if (probabilidad > 96) {
						speed = Vector2<float>(-500, 1000);
						animation_in_process = AnimationType::JUMP_AND_MOVE;
						on_air = true;
						characterJump01(isMale);
						music.doubleJump();
					}
					else if (probabilidad > 86 && hasSpecial && distanceBetween > 325 && specialDelay < 0) {
						animation_in_process = AnimationType::SPECIAL;
						specialDelay = 45;
					}
					else {
						animation_in_process = AnimationType::WALK_FORW;
					}
				} else if (difficulty_lvl == DifficultyLevel::MEDIUM) {
					if (probabilidad > 97) {
						speed = Vector2<float>(-500, 1000);
						animation_in_process = AnimationType::JUMP_AND_MOVE;
						on_air = true;
						characterJump01(isMale);
						music.doubleJump();
					}
					else if (probabilidad > 90 && hasSpecial && distanceBetween > 350 && specialDelay < 0) {
						animation_in_process = AnimationType::SPECIAL;
						specialDelay = 50;
					}
					else {
						animation_in_process = AnimationType::WALK_FORW;
					}
				} else if (difficulty_lvl == DifficultyLevel::EASY) {
					if (probabilidad > 98) {
						speed = Vector2<float>(-500, 1000);
						animation_in_process = AnimationType::JUMP_AND_MOVE;
						on_air = true;
						characterJump01(isMale);
						music.doubleJump();
					}
					else if (probabilidad > 94 && hasSpecial && distanceBetween > 350 && specialDelay < 0) {
						animation_in_process = AnimationType::SPECIAL;
						specialDelay = 60;
					}
					else {
						animation_in_process = AnimationType::WALK_FORW;
					}
				}
				break;

			case EstadoIA::ALEJARSE:
				if (probabilidad > 92) {
					speed = Vector2<float>(500, 1000);
					animation_in_process = AnimationType::JUMP_AND_MOVE;
					on_air = true;
					characterJump01(isMale);
					music.doubleJump();
				}
				else {
					animation_in_process = AnimationType::WALK_BACK;
				}
				break;

			case EstadoIA::MODO_ATAQUE:
				if (difficulty_lvl == DifficultyLevel::HARD) {
					if (hacerBarrido) {
						hacerBarrido = false;
						animation_in_process = AnimationType::KICK_LOW;
						music.hit7();

					}
					else if (animOp == AnimationType::BLOCK) {
						animation_in_process = AnimationType::KICK_LOW;
						music.hit7();
					}
					else
					{
						int ataque = rand() % 7;
						cout << ataque << endl;
						switch (ataque) {
						case 0:
							animation_in_process = AnimationType::PUNCH;
							music.hit6();
							break;

						case 1:
							animation_in_process = AnimationType::PUNCH_UPPER;
							music.hit6();
							break;

						case 2:
							animation_in_process = AnimationType::KICK;
							music.hit7();
							break;

						case 3:
							animation_in_process = AnimationType::KICK_LOW;
							music.hit7();
							break;
						
						case 4:
							animation_in_process = AnimationType::PUNCH_MULTIPLE;
							music.hit7();
							break;
						case 5:
							animation_in_process = AnimationType::KICK_UPPER;
							music.hit6();
							break;
						case 6:
							animation_in_process = AnimationType::PUNCH_CLOSE;
							music.hit6();
							break;
						}
						if (probabilidad > 80) {
							estado = EstadoIA::PREPAR_ATAQUE;
						}
						else {
							estado = EstadoIA::ALEJARSE;
						}
					}
				}
				else if (difficulty_lvl == DifficultyLevel::MEDIUM) {
					if (hacerBarrido && probabilidad > 80) {
						hacerBarrido = false;
						animation_in_process = AnimationType::KICK_LOW;
						music.hit7();
					}
					else if (animOp == AnimationType::BLOCK && probabilidad > 80) {
						animation_in_process = AnimationType::KICK_LOW;
						music.hit7();
					}
					else {
						int ataque = rand() % 7;
						cout << ataque << endl;
						switch (ataque) {
						case 0:
							animation_in_process = AnimationType::PUNCH;
							music.hit6();
							break;

						case 1:
							animation_in_process = AnimationType::PUNCH_UPPER;
							music.hit6();
							break;

						case 2:
							animation_in_process = AnimationType::KICK;
							music.hit7();
							break;

						case 3:
							animation_in_process = AnimationType::KICK_LOW;
							music.hit7();
							break;
						case 4:
							animation_in_process = AnimationType::PUNCH_MULTIPLE;
							music.hit7();
							break;
						case 5:
							animation_in_process = AnimationType::KICK_UPPER;
							music.hit6();
							break;
						case 6:
							animation_in_process = AnimationType::PUNCH_CLOSE;
							music.hit6();
							break;
						}
						if (probabilidad > 90) {
							estado = EstadoIA::PREPAR_ATAQUE;
						}
						else {
							estado = EstadoIA::ALEJARSE;
						}
					}
				}
				else if (difficulty_lvl == DifficultyLevel::EASY) {
					if (hacerBarrido && probabilidad > 95) {
						hacerBarrido = false;
						animation_in_process = AnimationType::KICK_LOW;
						music.hit7();
					}
					else {
						int ataque = rand() % 5;
						cout << ataque << endl;
						switch (ataque) {
						case 0:
							animation_in_process = AnimationType::PUNCH;
							music.hit6();
							break;

						case 1:
							animation_in_process = AnimationType::PUNCH_UPPER;
							music.hit6();
							break;

						case 2:
							animation_in_process = AnimationType::KICK;
							music.hit7();
							break;
						case 3:
							animation_in_process = AnimationType::KICK_UPPER;
							music.hit6();
							break;
						case 4:
							animation_in_process = AnimationType::PUNCH_CLOSE;
							music.hit6();
							break;
						}
						if (probabilidad > 96) {
							estado = EstadoIA::PREPAR_ATAQUE;
						}
						else {
							estado = EstadoIA::ALEJARSE;
						}
					}
				}
				break;

			case EstadoIA::SOBREPASAR_IZQ: // Estoy pegado a la dcha, tengo que sobrepasarle
				cout << "Sobrepaso izq" << endl;
				speed = Vector2<float>(-500, 1000);
				animation_in_process = AnimationType::JUMP_AND_MOVE;
				on_air = true;
				break;

			case EstadoIA::SOBREPASAR_DCHA: // Estoy pegado a la izq, tengo que sobrepasarle
				cout << "Sobrepaso dcha" << endl;
				speed = Vector2<float>(-500, 1000);
				animation_in_process = AnimationType::JUMP_AND_MOVE;
				on_air = true;
				break;
			case EstadoIA::MODO_DEFENSA:
				if (Difficulty[difficulty_lvl] > probabilidad) {
					blocking = true;
					if (isLowHit(animOp) && animation_in_process == AnimationType::DOWN) {
						animation_in_process = AnimationType::BLOCK_LOW;
					}
					else if (isLowHit(animOp)) {
						animation_in_process = AnimationType::JUMP;
						on_air = true;
						speed = Vector2<float>(0, 1000);
						if (Difficulty[difficulty_lvl] > probabilidad) {
							hacerBarrido = true;
						}
					}
					else {
						animation_in_process = AnimationType::BLOCK;
						if (Difficulty[difficulty_lvl] > probabilidad) {
							if (Difficulty[difficulty_lvl] > probabilidad - 25) {
								estado = EstadoIA::MODO_ATAQUE;
							}
							else {
								estado = EstadoIA::PREPAR_ATAQUE;
							}
						}
					}
				}
				else {
					animation_in_process = AnimationType::IDLE;
				}
				break;
			case EstadoIA::PREPAR_AGACHADO:
				if (Difficulty[difficulty_lvl] > probabilidad) {
					crouching = true;
					animation_in_process = AnimationType::DOWN;
				}
				break;
			case EstadoIA::DEFENSA_SPECIAL:
				if (Difficulty[difficulty_lvl] > probabilidad) {
					crouching = true;
					animation_in_process = AnimationType::DOWN;
				}
				break;
			}
					
		}
	}
}

void Character::ChangeIAState(Character opponent) {

	// Cambiamos de estado
	if (estado == EstadoIA::PREPAR_ATAQUE) {
		estado = EstadoIA::MODO_ATAQUE;
	}
	else if (estado == EstadoIA::DEFENSA_SPECIAL) {
		ia_special_counter--;
		if (ia_special_counter == 0) {
			estado = EstadoIA::ACERCARSE;
		}
	}
	else {
		int probabilidad = rand() % 100; // entre 0 y 99 (inclusive)
		int probabilidadIdle = rand() % 100; // entre 0 y 99 (inclusive)

		float distancia = abs(GetXPosition() - opponent.GetXPosition());
		AnimationType anim = opponent.getAnimation();
		bool siendoAtacado = opponent.isAttaking();

		//cout << "Posicion X: "  << global_position.x << endl;
		//cout << "Right Limit: " << screenRightLimit << " - Left Limit: " << screenLeftLimit << endl;
		//cout << "Hard Right Limit: " << screenRightHardLimit << " - Hard Left Limit: " << screenLeftHardLimit <<  endl;
		//cout << "totalMoveXBack: " << totalMoveXBack << endl;
		//cout << endl;

		if (global_position.x + 30 > screenRightLimit && totalMoveXBack + 30 > screenRightHardLimit) {
			estado = EstadoIA::SOBREPASAR_DCHA;
		}
		else if (global_position.x - 30 < screenLeftLimit && totalMoveXBack - 30 < screenLeftHardLimit) {
			estado = EstadoIA::SOBREPASAR_IZQ;
		}
		else if (siendoAtacado && difficulty_lvl != DifficultyLevel::EASY && distancia < 190) {
			ia_crouch_counter = 7;
			if (Difficulty[difficulty_lvl] > probabilidad) {
				estado = EstadoIA::MODO_DEFENSA;
			}
			else {
				estado = EstadoIA::IDLE;
			}
		} else if (anim == AnimationType::SPECIAL) {
			if (difficulty_lvl == DifficultyLevel::EASY && distancia > 300 && Difficulty[difficulty_lvl] > probabilidad) {
				estado = EstadoIA::DEFENSA_SPECIAL;
				ia_special_counter = 21;
			} else if (Difficulty[difficulty_lvl] > probabilidad) {
				estado = EstadoIA::DEFENSA_SPECIAL;
				ia_special_counter = 21;
			}
		}
		else if (difficulty_lvl != DifficultyLevel::EASY && anim == AnimationType::DOWN && distancia < 170) {
			estado = EstadoIA::PREPAR_AGACHADO;
			ia_crouch_counter--;
			if (ia_crouch_counter == 0) {
				ia_crouch_counter = 7;
				estado = EstadoIA::MODO_ATAQUE;
			}
		}
		else {
			ia_crouch_counter = 7;
			if (distancia < 170) {
				if (Difficulty[difficulty_lvl] > probabilidad) {
					estado = EstadoIA::MODO_ATAQUE;
				}
				else {
					estado = EstadoIA::ALEJARSE;
				}
			}
			else if (distancia > 400) {
				//cout << "\n" << "Entro en modo me acerco" << endl;
				estado = EstadoIA::ACERCARSE;
			}
			else if ( distancia > 170 && distancia < 350 && (estado == EstadoIA::PREPAR_AGACHADO || estado == EstadoIA::MODO_DEFENSA || estado == EstadoIA::IDLE) ) {
				estado = EstadoIA::ALEJARSE;
			}
		}
	}	
}

void Character::initPosition(Vector2<float> initPos) {
	global_position = initPos;
}

void Character::CheckAnimation() {
	if (fightKeyPressed) {
		if (!isAnyFightKeyPressed(player)) {
			fightKeyPressed = false;
		}
	}
	if (!falling && !dying) {
		if (on_air && air_attack_permitted && damageDelay < 0) {		// El personaje est� en el aire
			if (Keyboard::isKeyPressed(punchButton) && hitDelay < 0) {
				EndAndResetAnimation();
				hitDelay = 20;
				animation_in_process = AnimationType::PUNCH_FROM_AIR;
				music.hit6();
				air_attack_permitted = false;
			}
			else if (Keyboard::isKeyPressed(kickButton) && hitDelay < 0) {
				EndAndResetAnimation();
				hitDelay = 20;
				animation_in_process = AnimationType::KICK_FROM_AIR;
				music.hit8();
				air_attack_permitted = false;
			}
			// else -> nothing
			
		}
		else if (blocking) {
			if (!Keyboard::isKeyPressed(blockButton)) {
				blocking = false;
				animations[animation_in_process].animation.RecieveFlagEvent();
			}
		}
		else if (crouching) {			// El personaje est� agachado
			if (!fightKeyPressed && Keyboard::isKeyPressed(punchButton) && animation_in_process != AnimationType::PUNCH_FROM_DOWN && hitDelay < 0) {
				animations[animation_in_process].animation.ResetAnimation();
				hitDelay = 20;
				animation_in_process = AnimationType::PUNCH_FROM_DOWN;
				music.hit7();
				fightKeyPressed = true;
			}
			else if (!fightKeyPressed && Keyboard::isKeyPressed(kickButton) && hitDelay < 0) {
				hitDelay = 20;
				animation_in_process = AnimationType::KICK_FROM_DOWN;
				music.hit6();
				fightKeyPressed = true;
			}
			else if (Keyboard::isKeyPressed(blockButton) && !noBlockMode) {												//H.Kick en parado
				animation_in_process = AnimationType::BLOCK_LOW;
				blocking = true;
			}
			else if (!Keyboard::isKeyPressed(downButton)) {
				crouching = false;
				animations[animation_in_process].animation.RecieveFlagEvent();
			}
			else {
				if (!doing_animation) {
					animation_in_process = AnimationType::DOWN;
				}
			}
		}
		else if (!doing_animation) {	// El personaje no est� haciendo nada
			if (Keyboard::isKeyPressed(downButton)) {													//Agacharse

				if (!fightKeyPressed && Keyboard::isKeyPressed(kickButton)) {
					animation_in_process = AnimationType::KICK_LOW;
					music.hit7();
					fightKeyPressed = true;
				}
				else {
					crouching = true;
					animation_in_process = AnimationType::DOWN;
				}
			}
			else if (Keyboard::isKeyPressed(forwButton)) {												//Moverse derecha
				if (Keyboard::isKeyPressed(jumpButton)) {												//Salto hacia delante
					animation_in_process = AnimationType::JUMP_AND_MOVE;
					characterJump01(isMale);
					music.doubleJump();
					speed = Vector2<float>(-500, 1000);
					on_air = true;
				}
				else if (hasSpecial && Keyboard::isKeyPressed(backButton) && Keyboard::isKeyPressed(punchButton) && specialDelay < 0) {		//L.Kick, M.Kick en parado
					specialDelay = 30;
					animation_in_process = AnimationType::SPECIAL;
				}
				else if (!fightKeyPressed && Keyboard::isKeyPressed(punchButton)) {											//L.Punch hacia delante
					if (Keyboard::isKeyPressed(punchButton2)) {
						animation_in_process = AnimationType::PUNCH_UPPER_MULTIPLE;
						music.hit8();
					}
					else {
						animation_in_process = AnimationType::PUNCH_UPPER;
						music.hit6();
					}

					fightKeyPressed = true;
					punching = true;
				}
				else if (!fightKeyPressed && Keyboard::isKeyPressed(kickButton)) {											//M.Punch hacia delante
					animation_in_process = AnimationType::KICK_UPPER;
					music.hit6();
					fightKeyPressed = true;
				}
				else {																					//Solo moverse
					animation_in_process = AnimationType::WALK_FORW;
				}
			}
			else if (Keyboard::isKeyPressed(backButton)) {												//Moverse izquierda

				if (Keyboard::isKeyPressed(jumpButton)) {												//Salto hacia atras
					animation_in_process = AnimationType::JUMP_AND_MOVE;
					characterJump01(isMale);
					music.doubleJump();
					speed = Vector2<float>(500, 1000);
					on_air = true;
				}
				else if (!fightKeyPressed && Keyboard::isKeyPressed(punchButton)) {												//Salto hacia delante
					if (Keyboard::isKeyPressed(punchButton2)) {
						animation_in_process = AnimationType::PUNCH_MULTIPLE;
						music.hit8();
					}
					else {
						animation_in_process = AnimationType::PUNCH_CLOSE;
						music.hit7();
					}

					punching = true;
					fightKeyPressed = true;
				}
				else if (!fightKeyPressed && Keyboard::isKeyPressed(kickButton)) {											//L.Punch hacia delante
					animation_in_process = AnimationType::KICK;
					music.hit6();
					fightKeyPressed = true;
				}
				else {																					//Solo moverse
					animation_in_process = AnimationType::WALK_BACK;
				}

			}
			else if (Keyboard::isKeyPressed(jumpButton)) {												//Salto en parado
				animation_in_process = AnimationType::JUMP;
				characterJump01(isMale);
				speed = Vector2<float>(0, 1000);
				on_air = true;
			}
			else if (!fightKeyPressed && Keyboard::isKeyPressed(kickButton)) {		//M.Punch, H.Punch en parado
				animation_in_process = AnimationType::KICK;
				music.hit7();
				fightKeyPressed = true;
			}
			else if (!fightKeyPressed && Keyboard::isKeyPressed(punchButton)) {												//L.Punch en parado
				if (Keyboard::isKeyPressed(punchButton2)) {
					animation_in_process = AnimationType::PUNCH_MULTIPLE;
					music.hit8();
				}
				else {
					animation_in_process = AnimationType::PUNCH;
					music.hit7();
				}

				punching = true;
				fightKeyPressed = true;
			}
			else if (Keyboard::isKeyPressed(blockButton) && !noBlockMode) {												//H.Kick en parado
				animation_in_process = AnimationType::BLOCK;
				blocking = true;
			}
			else if (Keyboard::isKeyPressed(grabButton)) {
				animation_in_process = AnimationType::CATCH;
			}
		}
	}
	
}

void Character::CheckAnimationP2() {

	//CheckDebugAnimations();

	if (fightKeyPressed) {
		if (!isAnyFightKeyPressed(player)) {
			fightKeyPressed = false;
		}
	}
	else if (!falling && !dying ) {
		if (on_air && air_attack_permitted && damageDelay < 0) {					// El personaje est� en el aire
			if (Keyboard::isKeyPressed(punchButton2P2) && hitDelay < 0) {
				EndAndResetAnimation();
				hitDelay = 20;
				animation_in_process = AnimationType::PUNCH_FROM_AIR;
				music.hit6();
				air_attack_permitted = false;
			}
			else if (Keyboard::isKeyPressed(kickButtonP2) && hitDelay < 0) {
				EndAndResetAnimation();
				hitDelay = 20;
				animation_in_process = AnimationType::KICK_FROM_AIR;
				music.hit7();
				air_attack_permitted = false;
			}
			// else -> nothing
		}
		else if (blocking) {
			if (!Keyboard::isKeyPressed(blockButtonP2)) {
				blocking = false;
				animations[animation_in_process].animation.RecieveFlagEvent();
			}
		}
		else if (crouching) {			// El personaje est� agachado
			if (!fightKeyPressed && Keyboard::isKeyPressed(punchButtonP2) && animation_in_process != AnimationType::PUNCH_FROM_DOWN && hitDelay < 0) {
				animations[animation_in_process].animation.ResetAnimation();
				hitDelay = 20;
				animation_in_process = AnimationType::PUNCH_FROM_DOWN;
				music.hit8();
				fightKeyPressed = true;
			}
			else if (!fightKeyPressed && Keyboard::isKeyPressed(kickButtonP2) && hitDelay < 0) {
				hitDelay = 20;
				animation_in_process = AnimationType::KICK_FROM_DOWN;
				music.hit7();
				fightKeyPressed = true;
			}
			else if (Keyboard::isKeyPressed(blockButtonP2) && !noBlockMode) {												//H.Kick en parado
				animation_in_process = AnimationType::BLOCK_LOW;
				blocking = true;
			}
			else if (!Keyboard::isKeyPressed(downButtonP2)) {
				crouching = false;
				animations[animation_in_process].animation.RecieveFlagEvent();
			}
			else {
				if (!doing_animation) {
					animation_in_process = AnimationType::DOWN;
				}
			}
		}
		else if (!doing_animation) {	// El personaje no est� haciendo nada
			if (Keyboard::isKeyPressed(downButtonP2)) {													//Agacharse

				if (!fightKeyPressed && Keyboard::isKeyPressed(kickButtonP2)) {
					animation_in_process = AnimationType::KICK_LOW;
					music.hit7();
					fightKeyPressed = true;
				}
				else {
					crouching = true;
					animation_in_process = AnimationType::DOWN;
				}
			}
			else if (Keyboard::isKeyPressed(forwButtonP2)) {												//Moverse derecha
				if (Keyboard::isKeyPressed(jumpButtonP2)) {												//Salto hacia delante
					animation_in_process = AnimationType::JUMP_AND_MOVE;
					characterJump01(isMale);
					music.doubleJump();
					speed = Vector2<float>(-500, 1000);
					on_air = true;
				} else if (hasSpecial && Keyboard::isKeyPressed(backButtonP2) && Keyboard::isKeyPressed(punchButtonP2) && specialDelay < 0) {		//L.Kick, M.Kick en parado
					specialDelay = 30;
					animation_in_process = AnimationType::SPECIAL;
				}
				else if (!fightKeyPressed && Keyboard::isKeyPressed(punchButtonP2)) {											//L.Punch hacia delante
					if (Keyboard::isKeyPressed(punchButton2P2)) {
						animation_in_process = AnimationType::PUNCH_UPPER_MULTIPLE;
						music.hit8();
					}
					else {
						animation_in_process = AnimationType::PUNCH_UPPER;
						music.hit6();
					}

					fightKeyPressed = true;
					punching = true;
				}
				else if (!fightKeyPressed && Keyboard::isKeyPressed(kickButtonP2)) {											//M.Punch hacia delante
					animation_in_process = AnimationType::KICK_UPPER;
					music.hit7();
					fightKeyPressed = true;
				}
				else {																					//Solo moverse
					animation_in_process = AnimationType::WALK_FORW;
				}
			}
			else if (Keyboard::isKeyPressed(backButtonP2)) {												//Moverse izquierda

				if (Keyboard::isKeyPressed(jumpButtonP2)) {												//Salto hacia atras
					animation_in_process = AnimationType::JUMP_AND_MOVE;
					characterJump01(isMale);
					music.doubleJump();
					speed = Vector2<float>(500, 1000);
					on_air = true;
				}
				else if (!fightKeyPressed && Keyboard::isKeyPressed(punchButtonP2)) {												//Salto hacia delante
					if (Keyboard::isKeyPressed(punchButton2P2)) {
						music.hit8();
						animation_in_process = AnimationType::PUNCH_MULTIPLE;
					}
					else {
						animation_in_process = AnimationType::PUNCH_CLOSE;
						music.hit6();
					}

					punching = true;
					fightKeyPressed = true;
				}
				else if (!fightKeyPressed && Keyboard::isKeyPressed(kickButtonP2)) {											//L.Punch hacia delante
					animation_in_process = AnimationType::KICK;
					music.hit8();
					fightKeyPressed = true;
				}
				else {																					//Solo moverse
					animation_in_process = AnimationType::WALK_BACK;
				}

			}
			else if (Keyboard::isKeyPressed(jumpButtonP2)) {												//Salto en parado
				animation_in_process = AnimationType::JUMP;
				characterJump01(isMale);
				speed = Vector2<float>(0, 1000);
				on_air = true;
			}
			else if (!fightKeyPressed && Keyboard::isKeyPressed(kickButtonP2)) {		//M.Punch, H.Punch en parado
				animation_in_process = AnimationType::KICK;
				music.hit7();
				fightKeyPressed = true;
			}
			else if (!fightKeyPressed && Keyboard::isKeyPressed(punchButtonP2)) {												//L.Punch en parado
				if (Keyboard::isKeyPressed(punchButton2P2)) {
					animation_in_process = AnimationType::PUNCH_MULTIPLE;
					music.hit8();
				}
				else {
					animation_in_process = AnimationType::PUNCH;
					music.hit6();
				}

				punching = true;
				fightKeyPressed = true;
			}
			else if (Keyboard::isKeyPressed(blockButtonP2) && !noBlockMode) {												//H.Kick en parado
				animation_in_process = AnimationType::BLOCK;
				blocking = true;
			}
			else if (Keyboard::isKeyPressed(grabButtonP2)) {
				animation_in_process = AnimationType::CATCH;
			}
		}
		// else -> Nothing
	}	
}

void Character::CheckDebugAnimations() {
	if (Keyboard::isKeyPressed(Keyboard::Numpad0)) { // DEBUG
		animation_in_process = AnimationType::TURN_RIGHT;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Numpad1)) {
		animation_in_process = AnimationType::GET_CAUGHT;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Numpad2)) {
		animation_in_process = AnimationType::FATALITY;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Numpad3)) {
		animations[animation_in_process].animation.ResetAnimation();
		if (!mirrored) {
			speed = Vector2<float>(100, 200);
		}
		else {
			speed = Vector2<float>(-100, 200);
		}
		animation_in_process = AnimationType::FALL;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Numpad4)) {
		animation_in_process = AnimationType::FALL_UPPERCUT;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Numpad5)) {
		animation_in_process = AnimationType::WIN;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Numpad6)) {
		animation_in_process = AnimationType::NUTS;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Numpad7)) {
		dying = true;
		animation_in_process = AnimationType::DYING;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Numpad8)) {
		animation_in_process = AnimationType::FALL_BACK;
	}
	else if (Keyboard::isKeyPressed(Keyboard::Numpad9)) {
		animation_in_process = AnimationType::TURN_LEFT;
	}
}

bool Character::CheckScreenCollisions(float movement) {

	bool collision = false;

	Vector2f opponentPos;
	if (player == 1) {
		opponentPos = p2Position;
	}
	else if (player == 2) {
		opponentPos = p1Position;
	}

	if (global_position.x - movement <= screenLeftLimit) {

		if ((opponentPos.x + movement >= screenRightLimit) || (totalMoveXBack <= screenLeftHardLimit)) {
			collision = true;
		}
		else {
			moveXBack += movement;
			totalMoveXBack -= moveXBack;
			if (player == 1) {
				p2PositionOffset.x += movement;
			}
			else if (player == 2) {
				p1PositionOffset.x += movement;
			}
			collision = true;
		}

	} else if (global_position.x - movement >= screenRightLimit) {
		if ((opponentPos.x + movement <= screenLeftLimit) || (totalMoveXBack >= screenRightHardLimit)) {
			collision = true;
		} else {
			moveXBack += movement;
			totalMoveXBack -= moveXBack;
			if (player == 1) {
				p2PositionOffset.x += movement;
			}
			else if (player == 2) {
				p1PositionOffset.x += movement;
			}
			collision = true;
		}
	}

	if (global_position.y > screenFloorLimit) {
		damageDelay = 2;
		if (animation_in_process == AnimationType::FALL || animation_in_process == AnimationType::FALL_BACK
			|| animation_in_process == AnimationType::FALL_UPPERCUT) wantsShake = true;
		on_air = false;
		air_attack_permitted = true;
		global_position.y = screenFloorLimit;
		speed = Vector2<float>(0, 0);
		if (!isBlockingMovement(animation_in_process)) {
			animations[animation_in_process].animation.RecieveFlagEvent();
		}
		collision = true;
	}

	return collision;
}



/*
	Comprueba que tecla est� presionando el usuario y realiza una animaci�n dependiendo de eso
*/
void Character::DoAnimation() {
	doing_animation = true;
	bool finished = false;

	if (on_air) {
		damageDelay--;
	}

	if (animation_in_process == AnimationType::WIN) {
		speed = { 0, 0 };
		global_position.y = screenFloorLimit;
	}

	if (!fallen) {
		finished = animations[animation_in_process].animation.DoAnimation(
			body,
			shadow,
			hitbox,
			mirrored,
			(player == 2),
			animations[animation_in_process].hitbox_positions_X,
			animations[animation_in_process].hitbox_positions_Y,
			global_position,
			hitboxes[animation_in_process],
			damage_hitbox,
			animations[animation_in_process].damage_hitbox_positions_X,
			animations[animation_in_process].damage_hitbox_positions_Y,
			damage_hitboxes[animation_in_process]
		);
	}
	
	if (finished) {
		if (animation_in_process == AnimationType::FALL || animation_in_process == AnimationType::FALL_UPPERCUT ) {
			finished = false;
			if (life == 0) {
				fallen = true;
			}
			else {
				animation_in_process = AnimationType::RECOVER;
			}			
		}
		else if (animation_in_process == AnimationType::FALL_BACK) {
			finished = false;
			fallen = true;
		}
		else if (animation_in_process == AnimationType::DYING) {
			animations[animation_in_process].animation.ResetAnimation();
			finished = false;
		}
		else if (animation_in_process == AnimationType::RECOVER) {
			falling = false;
		}
		punching = false;
	}
	

	if (isFixedMovement(animation_in_process) && !isBlockingMovement(animation_in_process) && !dying) { // Sigue un desplazamiento fijado
		Vector2<float> mov = animations[animation_in_process].traslation;
		mov.x = mirrored ? -mov.x : mov.x;
		if (mov.x != 0) { 
			if (!CheckScreenCollisions(-mov.x)) {
				global_position += mov;
			}
		}
	} else { // Sigue las f�sicas del mundo (gravedad)
		Vector2<float> mov;

		mov.y = speed.y * updateTime;
		global_position.y -= mov.y;

		CheckScreenCollisions(0);

		mov.x = speed.x * updateTime;
		mov.x = mirrored ? -mov.x : mov.x;
		if (mov.x != 0) {
			if (!CheckScreenCollisions(mov.x)) {
				global_position.x -= mov.x;
			}
		}

		Vector2<float> pre_speed = speed;
		speed.y = speed.y - gravedad * updateTime;
		
		// Pasamos de subir a bajar
		if (pre_speed.y >= 0 && speed.y < 0) {
			if (!isBlockingMovement(animation_in_process)) {
				animations[animation_in_process].animation.RecieveFlagEvent();
			}
		}
	}

	if (finished && !dying) {
		AnimationType anim_pre = animation_in_process;

		if (!hasFlag(animation_in_process)) {
			EndAnimation();
		}
		else {
			EndAndResetAnimation();
		}
	
		if (anim_pre != AnimationType::IDLE && mirrorOnEnd) {
			mirrorOnEnd = false;
			mirroring = true;
		}

		if (mirrorOnEnd) {			
			animation_in_process = AnimationType::TURN_LEFT;			

			mirroring = true;
			mirrorOnEnd = false;

		}
		else if (mirroring) {
			mirroring = false;
			if (player == 1) {
				Keyboard::Key aux = backButton;
				backButton = forwButton;
				forwButton = aux;
			}
			else {
				Keyboard::Key aux = backButtonP2;
				backButtonP2 = forwButtonP2;
				forwButtonP2 = aux;
			}
			mirrored = !mirrored;
		}
	}
}

void Character::EndAnimation() {
	doing_animation = false;
	animation_in_process = AnimationType::IDLE;
}

void Character::EndAndResetAnimation() {
	animations[animation_in_process].animation.ResetAnimation();
	EndAnimation();
}

void Character::GetHit() {
	if(!crouching)	EndAndResetAnimation();
	doing_animation = true;
	if (on_air) {
		falling = true;
	}
}

void Character::debugDraw(RenderWindow& window) {
	//window.draw(hitbox);	//Para debug
	//window.draw(damage_hitbox);	//Para debug
	window.draw(shadow);
	window.draw(body);
}

void Character::RestartMirror(bool playerTwo) {

	forwButton = forwButton_aux;
	backButton = backButton_aux;

	forwButtonP2 = forwButton_auxP2;
	backButtonP2 = backButton_auxP2;

	mirroring = false;
	mirrorOnEnd = false;

}

void Character::Mirror() {
	looking_at = (looking_at == LookingAt::LEFT) ? LookingAt::RIGHT : LookingAt::LEFT;
	mirrorOnEnd = !mirrorOnEnd;
}

float Character::GetLife() {
	return life;
}

bool Character::isAttaking() {
	return isAnyFightKeyPressed(player);
}

void Character::fullReset() {
	
	animations[animation_in_process].animation.ResetAnimation();
	looking_at = LookingAt::RIGHT;

	if (mirrored && player == 1) {
			Keyboard::Key aux = backButton;
			backButton = forwButton;
			forwButton = aux;		
	}
	else if (mirrored && player == 2) {
		Keyboard::Key aux = backButtonP2;
		backButtonP2 = forwButtonP2;
		forwButtonP2 = aux;
	}

	mirrored = false;	
	doing_animation = false;
	on_air = false;
	punching = false;
	crouching = false;
	blocking = false;
	fallen = false;
	falling = false;
	dying = false;
	wait_air = false;
	fightKeyPressed = false;
	removePunchNext = false;
	freeze = true;
	mirrorOnEnd = false;
	mirroring = false;

	for (const auto& keyVal : animations) {
		animations[keyVal.first].animation.ResetAnimation();
	}

	animation_in_process = AnimationType::IDLE;
	
	speed = { 0, 0 };
	estado = EstadoIA::IDLE;
	duracionEstadoActual = 0;
	life = 100;

}


void Character::setDying(bool die) {
	if (!dying && die) {
		dying = true;
		speed = { 0, 0 };
		animation_in_process = AnimationType::DYING;
		global_position.y = screenFloorLimit;
	}
}

void Character::setDifficulty() {
	difficulty_lvl = chosen_difficulty;
}