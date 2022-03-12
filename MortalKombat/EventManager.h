#ifndef	EventManager_H
#define EventManager_H

#include "SFML/Graphics.hpp"
#include <iostream>
#include "StartManager.h"
#include "MenuManager.h"
#include "TransitionManager.h"
#include "OptionsManager.h"
#include "PlayerSelector.h"

using namespace sf;
using namespace std;

const int NUMERO_CANCIONES = 10;

class EventManager {

public:
	EventManager(Texture[], Font);
	~EventManager() = default;

	void Actualizar(Event);								//Actualiza el menu actual dada una tecla pulsada
	void draw(RenderWindow&);							//Dibuja el menu actual
	void setClock(Clock);								//Funcion de gestion de reloj
	void drawPlayerSelectorChoose(RenderWindow&);		//Dibuja el jugador seleccionado

private:

	void drawTransitionManager(RenderWindow& window);	//Dibuja la transicion entre pantallas

	int estado;											//0 = StartManager, 1 = MenuManager, 2 = PlayerSelectorHistoria, 3 = PlayerSelectorDuelo, 4 = OptionsManager, 5 = Torre, 6 = Batalla
	int personaje1;										//Personaje elegido por el jugador 1
	int personaje2;										//Personaje elegido por el jugador 2
	bool cambiadoEstado;								//True cuando se va a cambiar el estado en el siguiente frame, false en cualquier otro momento
	MenuManager MenuManager;							//Objeto del menu principal
	Music musica[NUMERO_CANCIONES];						//Vector de sonidos
	StartManager StartManager;							//Objeto de las pantallas iniciales
	OptionsManager OptionsManager;						//Objeto del menu de opciones
	TransitionManager TransitionManager;				//Objeto de la transicion
	Clock clock;										//Reloj para gestionar actualizacion de animaciones
	PlayerSelector PlayerSelector_hist;					//Objeto de selector de personajes de historia
	PlayerSelector PlayerSelector_duel;					//Objeto de selector de personajes de duelo

};

#endif