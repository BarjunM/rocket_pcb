
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Servo.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>

#define SERVO_X_PIN 9
#define SERVO_Y_PIN 10
#define SD_CS_PIN 4
#define BUZZER_PIN 8
#define LED_PIN 13
#define PYRO_1_PIN 5
#define PYRO_2_PIN 6

#define SAMPLE_FREQ 200.0f
#define BETA_INIT 0.1f 

Adafruit_MPU6050 mpu;
Adafruit_BMP280 bmp;
Servo servoX;
Servo servoY;
File dataFile;

enum FlightState {
    STATE_POWER_ON,
    STATE_IDLE,
    STATE_ARMED,
    STATE_FLIGHT_ASCENT,
    STATE_FLIGHT_BURNOUT,
    STATE_COAST,
    STATE_APOGEE_DETECTED,
    STATE_DESCENT_DROGUE,
    STATE_DESCENT_MAIN,
    STATE_LANDED,
    STATE_ABORT
};

FlightState currentState = STATE_POWER_ON;

volatile float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
volatile float beta = BETA_INIT;

float roll, pitch, yaw;
float accX, accY, accZ, accMagnitude;
float gyroX, gyroY, gyroZ;

float initialAltitude = 0.0f;
float currentAltitude = 0.0f;
float maxAltitude = 0.0f;
float pressure, temperature;

float setpointPitch = 0.0f;
float setpointRoll = 0.0f;

double pid_pitch_p, pid_pitch_i, pid_pitch_d;
double pid_roll_p, pid_roll_i, pid_roll_d;

double Kp = 3.0;
double Ki = 0.05;
double Kd = 1.2;

float lastPitchError = 0.0f;
float lastRollError = 0.0f;
float integralPitch = 0.0f;
float integralRoll = 0.0f;

unsigned long loopTimer;
unsigned long launchTime = 0;
unsigned long burnoutTime = 0;
unsigned long apogeeTime = 0;
unsigned long lastStateChangeTime = 0;

const int servoX_center = 90;
const int servoY_center = 90;
const int servo_max_deflection = 20;

void madgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az);
void updateSensors();
void updateStateMachine();
void tvcControl();
void logData();
void setStatusLED(int r, int g, int b);
void buzz(int freq, int duration);

void setup() {
    Serial.begin(115200);
    Wire.begin();

    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(PYRO_1_PIN, OUTPUT);
    pinMode(PYRO_2_PIN, OUTPUT);
    digitalWrite(PYRO_1_PIN, LOW);
    digitalWrite(PYRO_2_PIN, LOW);

    if (!mpu.begin()) {
        Serial.println("MPU6050 connection failed");
        currentState = STATE_ABORT;
        while (1) { buzz(200, 500); delay(500); }
    }
    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);

    if (!bmp.begin()) {
        Serial.println("BMP280 connection failed");
        currentState = STATE_ABORT;
        while (1) { buzz(300, 500); delay(500); }
    }
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                    Adafruit_BMP280::SAMPLING_X2,
                    Adafruit_BMP280::SAMPLING_X16,
                    Adafruit_BMP280::FILTER_X16,
                    Adafruit_BMP280::STANDBY_MS_0_5);

    servoX.attach(SERVO_X_PIN);
    servoY.attach(SERVO_Y_PIN);
    servoX.write(servoX_center);
    servoY.write(servoY_center);

    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("SD card initialization failed");
    } else {
        dataFile = SD.open("datalog.csv", FILE_WRITE);
        if (dataFile) {
            dataFile.println("Time,State,Altitude,MaxAlt,Pressure,Temp,Roll,Pitch,Yaw,AccX,AccY,AccZ,GyroX,GyroY,GyroZ,ServoX,ServoY,q0,q1,q2,q3");
            dataFile.close();
        }
    }
    
    buzz(1000, 100); delay(100); buzz(1000, 100);
    
    float tempAlt = 0;
    for(int i=0; i<100; i++){
        tempAlt += bmp.readAltitude(1013.25);
        delay(10);
    }
    initialAltitude = tempAlt / 100.0;
    maxAltitude = initialAltitude;

    currentState = STATE_IDLE;
    lastStateChangeTime = millis();
    loopTimer = micros();
}

void loop() {
    updateSensors();
    madgwickAHRSupdateIMU(gyroX, gyroY, gyroZ, accX, accY, accZ);
    updateStateMachine();
    logData();
    
    while(micros() - loopTimer < (1000000.0f / SAMPLE_FREQ));
    loopTimer = micros();
}

void updateSensors() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    accX = a.acceleration.x;
    accY = a.acceleration.y;
    accZ = a.acceleration.z;
    accMagnitude = sqrt(accX*accX + accY*accY + accZ*accZ);

    gyroX = g.gyro.x;
    gyroY = g.gyro.y;
    gyroZ = g.gyro.z;

    pressure = bmp.readPressure();
    temperature = bmp.readTemperature();
    currentAltitude = bmp.readAltitude(1013.25) - initialAltitude;

    yaw   = atan2(2.0f * (q1 * q2 + q0 * q3), q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.29578f;
    pitch = -asin(2.0f * (q1 * q3 - q0 * q2)) * 57.29578f;
    roll  = atan2(2.0f * (q0 * q1 + q2 * q3), q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3) * 57.29578f;
}

void updateStateMachine() {
    unsigned long now = millis();

    switch (currentState) {
        case STATE_IDLE:
            if (accMagnitude > 30.0f) { 
                currentState = STATE_ARMED;
            }
            servoX.write(servoX_center);
            servoY.write(servoY_center);
            break;

        case STATE_ARMED:
            if (accMagnitude > 40.0f && currentAltitude > 20.0f) {
                currentState = STATE_FLIGHT_ASCENT;
                launchTime = now;
            }
            break;

        case STATE_FLIGHT_ASCENT:
            tvcControl();
            if (accMagnitude < 8.0f && (now - launchTime > 1500)) {
                currentState = STATE_FLIGHT_BURNOUT;
                burnoutTime = now;
            }
            if (currentAltitude > maxAltitude) maxAltitude = currentAltitude;
            break;

        case STATE_FLIGHT_BURNOUT:
            tvcControl();
            if (currentAltitude < maxAltitude - 10.0f) {
                currentState = STATE_APOGEE_DETECTED;
                apogeeTime = now;
            }
            if (currentAltitude > maxAltitude) maxAltitude = currentAltitude;
            break;

        case STATE_COAST:
            servoX.write(servoX_center);
            servoY.write(servoY_center);
            if (currentAltitude < maxAltitude - 10.0f) {
                currentState = STATE_APOGEE_DETECTED;
                apogeeTime = now;
            }
            if (currentAltitude > maxAltitude) maxAltitude = currentAltitude;
            break;

        case STATE_APOGEE_DETECTED:
            servoX.write(servoX_center);
            servoY.write(servoY_center);
            digitalWrite(PYRO_1_PIN, HIGH); 
            buzz(2000, 500);
            currentState = STATE_DESCENT_DROGUE;
            break;

        case STATE_DESCENT_DROGUE:
            if (currentAltitude < 400.0f) {
                digitalWrite(PYRO_2_PIN, HIGH);
                buzz(2000, 500);
                currentState = STATE_DESCENT_MAIN;
            }
            break;

        case STATE_DESCENT_MAIN:
            if (abs(currentAltitude - initialAltitude) < 5.0f && accMagnitude < 2.0f) {
                currentState = STATE_LANDED;
            }
            break;

        case STATE_LANDED:
            digitalWrite(PYRO_1_PIN, LOW);
            digitalWrite(PYRO_2_PIN, LOW);
            buzz(1500, 2000);
            break;
        
        case STATE_ABORT:
            break;
    }
}

void tvcControl() {
    float pitchError = setpointPitch - pitch;
    float rollError = setpointRoll - roll;

    integralPitch += Ki * pitchError;
    integralRoll += Ki * rollError;
    integralPitch = constrain(integralPitch, -100, 100);
    integralRoll = constrain(integralRoll, -100, 100);

    float pitchDerivative = pitchError - lastPitchError;
    float rollDerivative = rollError - lastRollError;
    lastPitchError = pitchError;
    lastRollError = rollError;

    float pitchOutput = Kp * pitchError + integralPitch + Kd * pitchDerivative;
    float rollOutput = Kp * rollError + integralRoll + Kd * rollDerivative;

    int servoX_angle = servoX_center + constrain(rollOutput, -servo_max_deflection, servo_max_deflection);
    int servoY_angle = servoY_center + constrain(pitchOutput, -servo_max_deflection, servo_max_deflection);

    servoX.write(servoX_angle);
    servoY.write(servoY_angle);
}

void logData() {
    dataFile = SD.open("datalog.csv", FILE_WRITE);
    if (dataFile) {
        dataFile.print(millis()); dataFile.print(",");
        dataFile.print(currentState); dataFile.print(",");
        dataFile.print(currentAltitude, 2); dataFile.print(",");
        dataFile.print(maxAltitude, 2); dataFile.print(",");
        dataFile.print(pressure, 2); dataFile.print(",");
        dataFile.print(temperature, 2); dataFile.print(",");
        dataFile.print(roll, 2); dataFile.print(",");
        dataFile.print(pitch, 2); dataFile.print(",");
        dataFile.print(yaw, 2); dataFile.print(",");
        dataFile.print(accX, 4); dataFile.print(",");
        dataFile.print(accY, 4); dataFile.print(",");
        dataFile.print(accZ, 4); dataFile.print(",");
        dataFile.print(gyroX, 4); dataFile.print(",");
        dataFile.print(gyroY, 4); dataFile.print(",");
        dataFile.print(gyroZ, 4); dataFile.print(",");
        dataFile.print(servoX.read()); dataFile.print(",");
        dataFile.print(servoY.read()); dataFile.print(",");
        dataFile.print(q0, 4); dataFile.print(",");
        dataFile.print(q1, 4); dataFile.print(",");
        dataFile.print(q2, 4); dataFile.print(",");
        dataFile.println(q3, 4);
        dataFile.close();
    }
}

void buzz(int freq, int duration) {
    tone(BUZZER_PIN, freq, duration);
}

void madgwickAHRSupdateIMU(float gx, float gy, float gz, float ax, float ay, float az) {
	float recipNorm;
	float s0, s1, s2, s3;
	float qDot1, qDot2, qDot3, qDot4;
	float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

	float dt = (float)(micros() - loopTimer) / 1000000.0f;
    if(dt == 0) return;

	qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
	qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
	qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
	qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))) {
		recipNorm = 1.0f / sqrt(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		_2q0 = 2.0f * q0;
		_2q1 = 2.0f * q1;
		_2q2 = 2.0f * q2;
		_2q3 = 2.0f * q3;
		_4q0 = 4.0f * q0;
		_4q1 = 4.0f * q1;
		_4q2 = 4.0f * q2;
		_8q1 = 8.0f * q1;
		_8q2 = 8.0f * q2;
		q0q0 = q0 * q0;
		q1q1 = q1 * q1;
		q2q2 = q2 * q2;
		q3q3 = q3 * q3;

		s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
		s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
		s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
		s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
		recipNorm = 1.0f / sqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3);
		s0 *= recipNorm;
		s1 *= recipNorm;
		s2 *= recipNorm;
		s3 *= recipNorm;

		qDot1 -= beta * s0;
		qDot2 -= beta * s1;
		qDot3 -= beta * s2;
		qDot4 -= beta * s3;
	}

	q0 += qDot1 * dt;
	q1 += qDot2 * dt;
	q2 += qDot3 * dt;
	q3 += qDot4 * dt;

	recipNorm = 1.0f / sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	q0 *= recipNorm;
	q1 *= recipNorm;
	q2 *= recipNorm;
	q3 *= recipNorm;
}
