// Fichier : Alarm.h

#pragma once


enum AlarmState {

	OFF,

	WATCHING,

	ON,

	TESTING

};


class Alarm {

public:



	Alarm(int rPin, int gPin, int bPin, int buzzerPin, float& distancePtr);




	void update();



	void setColourA(int r, int g, int b);

	void setColourB(int r, int g, int b);

	void setVariationTiming(unsigned long ms);


	void setDistance(float d);


	void setTimeout(unsigned long ms);


	void turnOff();

	void turnOn();



	void test();


	AlarmState getState() const;


private:

	// --- Broches mat�rielles ---

	int _rPin, _gPin, _bPin, _buzzerPin;


	// --- Couleurs du gyrophare ---

	int _colA[3]; // Couleur A

	int _colB[3]; // Couleur B

	bool _currentColor = true; // false = couleur A, true = couleur B


	// --- D�tection ---

	float* _distance; // Pointeur vers la distance externe

	float _distanceTrigger = 10.0; // Distance minimale pour d�clenchement


	// --- Temporisation ---

	unsigned long _variationRate = 500; // Intervalle entre deux couleurs

	unsigned long _timeoutDelay = 3000; // D�lai avant arr�t

	unsigned long _lastUpdate = 0; // Pour le gyrophare

	unsigned long _lastDetectedTime = 0; // Derni�re fois qu'on a vu l'objet

	unsigned long _testStartTime = 0; // Pour TESTING


	// --- �tat interne ---

	AlarmState _state = OFF;


	// --- Indicateurs de transition ---

	bool _turnOnFlag = false; // Pour transition vers ON

	bool _turnOffFlag = false; // Pour transition vers OFF


	// --- M�thodes internes ---

	void _setRGB(int r, int g, int b);

	void _turnOff(); // �teint DEL et buzzer


	// Gestion des �tats

	void _offState();

	void _watchState();

	void _onState();

	void _testingState();

};