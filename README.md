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



## BOM

| No. | Qty | Component / Comment               | Supplier Part | Supplier     | Unit Cost (CAD) | Total Cost (CAD) | Link |
|-----|-----|-----------------------------------|---------------|--------------|------------------|-------------------|------|
| 1   | 3   | RS606-10N011CG1B-3A               | C2885789      | LCSC         | $0.60*           | $1.80*            | -    |
| 2   | 1   | MPU5060 Breakout (Accelerometer)  | -             | AliExpress   | -                | Already Owned     | -    |
| 3   | 1   | BMP280 Breakout (Barometer)       | -             | AliExpress   | -                | Already Owned     | -    |
| 4   | 1   | ESP32 WROOM (Microcontroller)     | -             | AliExpress   | -                | Already Owned     | -    |
| 5   | 1   | Micro SD Card Reader (Data Storage) | -           | AliExpress   | -                | Already Owned     | -    |
| 6   | 1   | Servos (Motor)                    | -             | AliExpress   | $11.31           | Already Owned             | -    |
| 7   | 1   | 3S LiPo (Battery)                 | -             | AliExpress   | $19.30           | $19.30            | [Link](https://www.aliexpress.com/item/1005008729299721.html?spm=a2g0o.productlist.main.1.2385780bOu6CPQ&algo_pvid=c7b5bdea-aded-4a12-b92b-e16a9b05e28c&algo_exp_id=c7b5bdea-aded-4a12-b92b-e16a9b05e28c-0&pdp_ext_f=%7B%22order%22%3A%2271%22%2C%22eval%22%3A%221%22%7D&pdp_npi=4%40dis%21CAD%2148.39%2120.00%21%21%2134.66%2114.33%21%4021030ea417534186546303625e1532%2112000046424127973%21sea%21CA%212821664736%21ACX&curPageLogUid=u1PkQV2KdZHv&utparam-url=scene%3Asearch%7Cquery_from%3A) |
| 8   | 3   | 3S XT30 to 5V 5A (Buck Converter) | -             | SpeedyFPV    | $5.00            | $15.00             | [Link](https://speedyfpv.com/products/readytosky-5v-ubec-3a-5a-7a-lipo-dc-dc-voltage-converter-step-down-module?variant=44138085810390) |
| 9   | 3   | Custom PCB                        | -             | JLCPCB       | $2.73*           | $8.19*            | -    |

> \* Approximate or discounted pricing
