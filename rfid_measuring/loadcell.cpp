
#include <Arduino.h>

#include "loadcell.h"

HX711 scale(3, 2); //HX711 scale(6, 5);

const uint16_t MAXWEIGHT = 500;
double getWeight()
{
	// debug serial
	Serial.print(F("Weight reading "));

	// 
	double units = scale.get_units(5);
	if (units < MAXWEIGHT)
	{
		if (units == 0 || units <= 0.5)
			units = 0.00;
	}
	else
		units = MAXWEIGHT;

	Serial.println(units);
	return units;
}
