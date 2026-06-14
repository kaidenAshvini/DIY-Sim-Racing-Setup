# DIY Sim Racing Setup
It is no secret that simulation racing setups are ridiculously expensive (with high end setups reaching thousands of dollars!). I sought to create a DIY Sim Racing Setup, using many materials I and other hobbyists would already have access to, equipped with a force-feedback wheel, a 3-pedal setup, and a 6-speed gearbox. In addition, I aimed for the wheel base to be very compact while offering a driving experience comparable to mid-range setups, for a far cheaper cost.

## Specs
The Wheel Base features a 220W 48V Brushless DC Motor for punchy, high-torque force feedback. It reads position through a 600 P/R Incremental Rotary Encoder, which is responsive and accurate. It is also marginally adjustable, with 3 height settings and 10 degrees of adjustability.

The Pedals feature torsion springs, adjustable end-stops, and rotary encoders.

The Gearbox is a relatively standard 6-speed focused on delivering consistent shifts.

## Design Philosophy:
While I do have many years of experience on an FTC robotics team (Roboavatars #7303), I am fully a software member. This project was my first attempt at CADing and building a physical project from scratch! Keep this in mind as you read through these designs, and please let me know at kaidenashvini@gmail.com of any mistakes I may have made!

Remember, while I have flushed out a completed project that works for me, feel free to adapt to whatever materials you have on hand. This will keep costs very low, as it did in my case, and will make your personal project far more convenient to complete!

1. ### Wheel Base
    - Using 20x20 Extrusions
        - I have access to plenty of these 20x20 McMaster-Carr extrusions through my robotics team. They are cheap, rigid, and very convenient.
    - Front Mount
        - My CAD features a strange design for mounting the force-feedback motor, as it recesses the motor by a few inches. This provides clearance for the inclusion of a shaft coupler (neccesary to extend the motor shaft to mount a 25mm pulley), and it also provides a sturdier rear mounting position.
    - Motor Rear Mount
        - The Motor Rear Mount serves to reinforce the motor mount, as features two 3/8ths inch aluminum plates.
    - Encoder Rear Mount
        - I geared the encoder to directly to the drive shaft, and found a convenient place to mount it. The mount also keeps the extrusions in place.
        - Gearing an encoder, in my opinion, is preferential to belting it. Gearing will have less lag/backlash, and are also easier to 3d print
    - 3D Printed Pulleys
        - Metal pulleys are better in *nearly* every way, **but there are a few reasons I chose to 3D print them**. I chose 3D printed pulleys to keep costs down, and because it makes mounting the wheel way easier. I made mine out of annealed carbon nylon (which many have reported online to be very sturdy!), and the design features embedded nuts and space to attach a flange coupler for compactness.
    - Flipsky Vesc Control
        - This motor controller is common, and it is listed as compatible with the firmware I've decided to go with for this project. They are relatively common, and I had one on hand. In reality, any controller works as long as it's compatible with the firmware listed in ___software___.

2. ### Pedals
    - Using Torsion Springs
        - I have torsion springs beneath the pedal shafts, which aim offer a realistic feel similar to real pedals.
    - Pedal Design/Sizing
        - I based the pedal sizes off of real pedals from my IRL project car. This (hopefully) makes it easy to heel-toe.
    - Reading Pedal Position
        - I chose to use a geared 3D print for low-latency and high-accuracy (by using gear ratios to my advantage)

3. ### Shifter
    - Using A Swivel Adapter
        - A Swivel Adapter, as used on rachets, makes for a great way of replicating some of the smooth feel of shifting gears on an older car.
    - Hall Effect Sensors
        - I can attach hall effect proximity sensors at each hole in the shifter. It's easy, and I can connect them to an ESP32 for the game to read them


## Adaptation:
This is a fully-flushed out project that I've put together. I used the resources that I have available to create the best product I could within a reasonable cost. However, I understand that not everyone will have access to the exact same tools and materials as I did. In this case, **IMPROVISE**. Check out the ___adaptation__ guide for more info on how to make this project yourself.

## Building:
1. ### Wheel Base
    1. Tapping your Extrusions
        - Use a M5 tap, and preferably a drill press, to tap the ends of your extrusions. For the 1/2 ft extrusions, do it for both sides. Otherwise, tap as neccesary
        - Use a cutting fluid or lubricant, I used WD-40
        - **Take your time**, make sure these come out straight
    2. Printing
        - You will want to print each of these parts to the most sturdy setting you can, with the strongest material you have available, especially if you aren't using the reinforcement plates.
    3. Assembly
        - Follow these images:

2. ### Pedals
    1. Side Walls
        - I CNCed my side walls with either aluminum or polycarbonate. You can even print them, although I cannot guarantee their longevity.
    2. Assembly
        - Follow these images

3. ### Gearbox
    1. Assemmbly
        - Follow these images

## Software
Many high-quality DIY sim setups utilize and OpenFFBoard, which comes with all of the force-feedback control that communicates with popular sim games. However, OpenFFBoards are priced at $160, and are produced in Germany. They are fantastic products with great quality, and their software is unmatched in the DIY niche. However, for one reason or another, not everyone can get easy access to one of these boards. Instead, I rely on an ABI encoder-compatible FlipSky VESC controller, which I connect to a cheap power-supply. I also rely on an inexpensive STM32 board compatable with the OpenFFBoard firmware.

This has many implications for developing software, though. The pedals will require seperate microcontrollers. I went with some old ESP32s I had lying around, and also borrowed a RP2040. I wanted to create firmware for multiple MCUs, so that you can use whatever one you want. Either way, both firmwares should be easy to adapt.

### ESP32 (w/out USB HID)

Older ESP32s do not have the correct hardware to support native USB HID, so I designed a way to read serial inputs through a C++ library___ and using vJoy SDK to emulate a HID device. Messy and complicated, yes. But it's important that this project makes sim-racing accessible for anyone.