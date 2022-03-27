#include "Courtyard.h"

void Courtyard::loadTextures() {

	float ratio = 4.0/3.0;

	sizeVector = 8;
	courtyardTexture->loadFromFile("resources/Sprites/Escenarios/Courtyard.png");

	float rectWidth = 1037.0f;
	float rectHeight = 281.0f;

	float rectWidth_monks = 1037.0f;
	float rectHeight_monks = 128.0f;

	Vector2f sizeBackground = Vector2f(rectWidth*(600.0/281.0), 600.0f);
	Vector2f sizeBackground_monks = Vector2f(rectWidth_monks * (600.0 / 281.0), rectHeight_monks*(600.0/281.0));

	IntRect uvRect;
	RectangleShape tile;
	uvRect.width = rectWidth;
	uvRect.height = rectHeight;

	tile.setTexture(courtyardTexture);
	tile.setSize(sizeBackground);
	tile.setPosition(0.0f, 0.0f);
	tile.setScale(1.0f, 1.0f);

	//Fondo con piedras 0
	uvRect.left = 8.0f;
	uvRect.top = 8.0f;

	tile.setTextureRect(uvRect);
	backgroundVector.push_back(tile);

	//Piezas metalicas 1
	uvRect.top = 297.0f;

	tile.setTextureRect(uvRect);
	backgroundVector.push_back(tile);

	//Templo 2
	uvRect.top = 586.0f;

	tile.setTextureRect(uvRect);
	backgroundVector.push_back(tile);

	//Barandillas y dragones 3
	uvRect.top = 875.0f;

	tile.setTextureRect(uvRect);
	backgroundVector.push_back(tile);

	//Monjes 4
	uvRect.top = 32.0f;
	uvRect.left = 1121.0f;
	uvRect.height = rectHeight_monks;
	tile.setSize(sizeBackground_monks);
	tile.setPosition(0.0f, (281-128)*(600.0 / 281.0));

	tile.setTextureRect(uvRect);
	backgroundVector.push_back(tile);

	//Guardias 5
	uvRect.left = 8.0f;
	uvRect.top = 1256.0f;
	uvRect.height = rectHeight;
	tile.setSize(sizeBackground);
	tile.setPosition(0.0f, 0.0f);

	tile.setTextureRect(uvRect);
	backgroundVector.push_back(tile);

	//Suelo 6
	uvRect.top = 1545.0f;

	tile.setTextureRect(uvRect);
	backgroundVector.push_back(tile);
}