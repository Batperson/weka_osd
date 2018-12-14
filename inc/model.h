/*
 * model.h
 * Aircraft state and flight data
 *
 */

#ifndef MODEL_H_
#define MODEL_H_

typedef struct
{
	float pitch;
	float roll;
	float heading;
	float homeVector;
} ATTITUDE;

typedef struct
{
	u32 latitude;
	u32 longitude;
	float altitude;
} LOCATION;

typedef struct
{
	float voltage;
	float current;
	float milliampHours;
} ELECTRONICS;

typedef struct
{
	float horizontal;
	float vertical;
} VELOCITY;

typedef struct
{
	ATTITUDE 	att;
	LOCATION 	loc;
	VELOCITY 	vel;
	ELECTRONICS elec;
} MODEL, *PMODEL;

extern MODEL model;

#define DEREFERENCE_OFFSET_FLOAT(ofs) *((float*)(((void*)&model)+ofs))
#define DEREFERENCE_OFFSET_INT(ofs) *((int*)(((void*)&model)+ofs))

void initModel();
void demoModelUpdate();

#endif /* MODEL_H_ */
