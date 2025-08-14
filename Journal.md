---
title: "Elytra"
author: "Arjun Gupta (On Slack: Arjun H)"
description: "Elytra is a half-kilogram solid-fuel TVC hobby rocket flight computer"
created_at: "2025-07-25"
---

**Daily Journal, Total time 17 hours**

## July 18 (4 hours): Research begins!

Today was a sunny afternoon so I decided to catch some shade and look at some flight  
computer parts. Since I’ve never made a pcb before, I checked out a couple of videos from BPS  
space and instantly got confused… An overview of researched systems include: some sort of  
accelerometer in order to track active trajectory in 3d space - decided on mpu6050, a barometer  
to track altitude - decided on bmp280, a flight computer - esp32 is great since it has wireless  
capability, and sd storage. Since batteries seem pretty expensive, it looks like I should be able  
to power the whole thing with a 9 volt battery.  

In order to get this thing to just work ASAP, I’ve found a bunch of breakoutboards which I can  
link together though a PCB and which will also be able to mount 3 servo motors to: 2 for the  
TVC gimbal, and one for a parachute system.

![image](/assets/maincomputerdiagram.png)
![image](/assets/servosdiagram.png)
![image](/assets/additionalboarddiagram.png)




## July 23 (5 hours): There's always that one thing that smacks you in the face

After ordering everything and slapping everything together on a breadboard, a couple  
realisations slipped my mind. I just realised that a single 9 volt battery will probability not be able  
to provide enough current to 3 servos under load while also powering the esp32 breakout board  
(~0.5 amp for each motor + 200 mA for the esp32 is a little to close to the ~0.8 mA continuous  
to ~2 A max draw for any general 9v battery).  

I found this 3s lipo from BetaFPV - which also exists on aliexpress  
[https://betafpv.com/collections/batt-3s/products/lava-2s-3s-4s-450mah-75c-battery-2pcs?variant=40376192663686](https://betafpv.com/collections/batt-3s/products/lava-2s-3s-4s-450mah-75c-battery-2pcs?variant=40376192663686)  
which should pair nicely with this 12v to 5v buck converter module with an xt30 connector  
[https://speedyfpv.com/products/readytosky-5v-ubec-3a-5a-7a-lipo-dc-dc-voltage-converter-step-down-module?variant=44138085810390](https://speedyfpv.com/products/readytosky-5v-ubec-3a-5a-7a-lipo-dc-dc-voltage-converter-step-down-module?variant=44138085810390)  
it's only 5$ so def worth it.  


Troubleshooting took quite a bit, I didn’t understand why the schematic didn’t convert to pcb at  
first, then I quickly found out that not all public components actually have component footprints!  
So I had to make a few and resize a couple of the boards before proceeding. Since I am mostly  
using breakoutboards, the footprints are really only a bunch of multi-layer pads.

![image](/assets/livecomputer.png)
![image](/assets/powerdiagram.png)
![image](/assets/unwiredpcb.png)

## July 24 (2 hours): This takes longer than it looks…

Routing is annoying as you want everything to look nice… but it has to actually work. Just spent  
time making it look not trash. Also touched up some of the component footprints and made them  
so they wouldn’t be a huge pain to solder to.

![image](/assets/wiredpcb.png)
![image](/assets/3dpcb.png)

## August 7: PCBs
Got the shipping notice for the boards today. Double‑checked the footprints list & soldering order so I wouldn't waste time once the package landed. Gathered flux, tips, wick, headers, and queued up the continuity checklist. (No real time counted, just logistics.)

## August 8 (2.5 hours): Solder marathon & power bring‑up

Unboxed everything and went straight into assembly. Started with the lowest‑profile components / headers, then sensor breakout boards (mpu6050, bmp280, SD module, ESP32 headers). Lots of careful flux + drag soldering so I wouldn't lift pads on the custom footprints. After each section I buzzed out traces: 3V3, 5V, GND rails, I2C lines, SPI lines to the SD, and servo signal pins. No shorts (yay), only had to reflow one stubborn SDA joint.

Brought up the power system: verified buck output at 5.02 V under a light dummy load and checked that the ESP32 3V3 regulator stayed within spec. All sensors enumerated on the bus first try. Servos started up with a quick test.

![image](/assets/soldering1.jpg)
![image](/assets/soldering2.jpg)
![image](/assets/finished_pcb.jpg)

## August 9–10 (3.5 hours): Sensor validation, PID + TVC sim tuning

Spent time writing bring‑up / test sketches: read mpu6050 accel + gyro, fused with a quick complementary filter; grabbed bmp280 pressure -> altitude deltas; confirmed SD init + logged a short sensor stream. Added error handling to catch any I2C hiccups (none so far). 

[TVC + PID demo video](/assets/FC.MOV)  

Then moved into PID + thrust vector control simulation, fed in some synthetic disturbance profiles and iterated gains until response settled without overshoot while keeping rise time snappy. Also mapped servo pulse limits to physical gimbal angles so the controller output clamps cleanly.

![data](/assets/example_data.png)

Ran a few full-loop software runs (sensor -> filter -> PID -> actuator mapping) and everything behaved; timing headroom on the ESP32 looks fine.

Video demo of all above stuff working [demo video](/assets/Elytra_Basic.mp4)

Link for above if you dont wanna download: https://youtu.be/cWIUwfU-qWE
