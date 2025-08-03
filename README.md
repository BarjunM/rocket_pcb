# Elytra: TVC Hobby Rocket Flight Computer

## Short Description

Elytra is a half-kilogram solid-fuel TVC hobby rocket flight computer. To develop a custom computer, the goal is to start as simple as possible and iterate through more and more detailed developments. The current version thus far incorporates working components including an ESP32, BMP280, and MPU6050 dev board and joins them all onto 1 single pcb.

## Why Make This Project?

The purpose of Elytra is to eventually explore self-landing rocket technology. In order to develop an autonomous rocket, the computer needs to be able to read input data regarding yaw, pitch, roll, acceleration, and elevation in order to actively adjust the direction of thrust and self-correct direction. Hence, the purpose of the flight computer is to actively control a thrust vectoring system via servos (TVC) based on input data from various telemetry sensors, including an accelerometer (mpu6050) and barometer (bmp280)

## Project Images

![cad_image](/assets/sch_rocket.png)
![cad_image](/assets/livecomputer.png)
![image](/assets/3dpcb.png)
![image](/assets/wiredpcb.png)
![image](/assets/cad1.png)



## BOM

| No. | Qty | Component / Comment                  | Supplier Part | Supplier   | Unit Cost | Total Cost (+shipping) | Link |
|-----|-----|--------------------------------------|----------------|------------|-----------|-------------------------|------|
| 1   | 5   | RS606-10N011CG1B-3A                  | C2885789       | LCSC       | $0.67*    | $3.35*                  | [JLCPCB](https://jlcpcb.com/partdetail/HCTL-RS606101N011CRB/C2885788) |
| 2   | 1   | MPU6050 Breakout (Accelerometer)     | -              | AliExpress | -         | Already Owned           | -    |
| 3   | 1   | BMP280 Breakout (Barometer)          | -              | AliExpress | -         | Already Owned           | -    |
| 4   | 1   | ESP32 WROOM (Microcontroller)        | -              | AliExpress | -         | Already Owned           | -    |
| 5   | 1   | Micro SD Card Reader (Data Storage)  | -              | AliExpress | -         | Already Owned           | -    |
| 6   | 1   | Servos (Motor)                       | -              | AliExpress | -         | Already Owned           | -    |
| 7   | 1   | 3S LiPo (Battery)                    | -              | AliExpress | -         | Already Owned           | -    |
| 8   | 5   | 3S XT30 to 5V 5A (Buck Converter)    | -              | SpeedyFPV  | -         | Already Owned           | -    |
| 9   | 5   | Custom PCB                           | -              | JLCPCB     | $2.92     | $14.60 + $27.72 shipping| -    |

**Total Cost: $45.67** (excluding already owned components)
