// Fichier : PorteAutomatique.h

#pragma once


#include <AccelStepper.h>


enum EtatPorte {

	FERMEE,

	OUVERTE,

	EN_FERMETURE,

	EN_OUVERTURE

};


class PorteAutomatique {

public:

	// Constructeur

	// p1 � p4 : broches IN1 � IN4 du ULN2003

	// distancePtr : pointeur vers la variable distance partag�e

	PorteAutomatique(int p1, int p2, int p3, int p4, float& distancePtr);


	// Doit �tre appel�e continuellement dans loop()

	void update();


	// R�gle l'angle maximal d'ouverture de la porte

	void setAngleOuvert(float angle);


	// R�gle l'angle de fermeture (habituellement 0�)

	void setAngleFerme(float angle);


	// D�finit le nombre de pas par tour du moteur

	void setPasParTour(int steps);


	// D�finit la distance de d�clenchement pour l'ouverture

	void setDistanceOuverture(float distance);


	// D�finit la distance de d�clenchement pour la fermeture

	void setDistanceFermeture(float distance);


	// Retourne l'�tat actuel de la porte sous forme de texte

	const char* getEtatTexte() const;


	// Retourne l'angle actuel de la porte

	float getAngle() const;


private:

	// Moteur AccelStepper

	AccelStepper _stepper;


	// Angle de position

	float _angleOuvert = 90.0;

	float _angleFerme = 0.0;


	// Conversion : pas par tour

	int _stepsPerRev = 2048; // par d�faut pour 28BYJ-48


	// Distance mesur�e

	float _distance;


	// Distance de seuils

	float _distanceOuverture = 20.0;

	float _distanceFermeture = 30.0;


	// �tat actuel

	EtatPorte _etat = FERMEE;


	// M�thodes d'�tat

	void _ouvertState();

	void _fermeState();

	void _ouvertureState();

	void _fermetureState();


	// actions

	void _ouvrir();

	void _fermer();


	void _mettreAJourEtat();


	// Aide

	long _angleEnSteps(float angle) const;

};