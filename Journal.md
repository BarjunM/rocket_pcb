<img width="1321" height="66" alt="image" src="https://github.com/user-attachments/assets/7f0f11af-4518-42a0-9d62-d79f872627d1" /># Rocket Journal

---
title: "Elytra"
author: "Arjun Gupta (On Slack: Arjun H)"
description: "Elytra is a half-kilogram solid-fuel TVC hobby rocket flight computer"
created_at: "2025-07-25"
---

**Daily Journal, Total time 11 hours**

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
