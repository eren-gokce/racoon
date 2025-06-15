#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void kalkis();
void burnout();
void apogee();
void parasut();
void alcalma();
float avarage_in_given_time(unsigned long given_time /*as ms*/, float sensor_value);

#pragma region externs
//flag
extern volatile uint8_t flag;
// m_n lerin değerleri
extern float yukseklikraw; //m_n[0]
extern float yukseklik; // m_n[0] - yukseklikraw
extern float hiz; // m_n[1]
extern float ivme; // m_n[2]
//ypr değerleri
extern float yaw;
extern float pitch;
extern float row;
//ypr derece degerleri
extern float yawDeg;
extern float pitchDeg;
extern float rollDeg;
//avarage_calc icin millis
extern unsigned long millis_saved;
extern unsigned long millis_counter;

#pragma endregion

#endif