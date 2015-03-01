//
//  myConstant.h
//  HungerGame_v2
//
//  Created by Hong Li on 2/24/15.
//  Copyright (c) 2015 Hong. All rights reserved.
//

#ifndef HungerGame_v2_myConstant_h
#define HungerGame_v2_myConstant_h
#include <math.h>

#define DEBUG_MODE
#define calcDegree(a,b) atan( ((b).z - (a).z) / ((b).x/(a).x))  
#define calcDistance(a,b) sqrt(pow(((b).z - (a).z),2) / pow(((b).x/(a).x),2))
// TIMER
#define HUNTER_WALK_STAMP 0
#define DEFAULT_STAMP     1  
#define ZOOM_STAMP        2

#define H 0
#define C 1
#define W 2
#define R 3
#define E 4

#define H_ARM_L           2
#define H_HEAD            1
#define H_LEG_L           3
#define H_ARM_R           4
#define H_LEG_R           0
#define C_WING_L          5
#define C_WING_R          6
#define C_LEG_L           7
#define C_LEG_R           8

#define scale(a, b, c) ((TIME - myTimer[a] - b ) / c)

bool isMoving 			=0;
bool isShoting 			=0;
// SPEED
//
#define HUNTER_SPEED 5
#define WOLF_SPEED  10
#define CHICKEN_SPEED 0.5 
#endif
