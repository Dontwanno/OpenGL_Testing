//
// Created by twang on 05/12/2022.
//

#ifndef GLITTER_MASS_PROPS_HPP
#define GLITTER_MASS_PROPS_HPP

// Desc:
// Header file that contrains methods use to calculate
// mass and inertia tensor from density

//include dependancies
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

//fills massProperties with tensor followed by mass
void computeMassProperties(ifstream &meshFile,
                           double density, double *massProperties);

#endif //GLITTER_MASS_PROPS_HPP
