# visDx
Simple Application for Visual stimulu with DxLib [http://dxlib.o.oo7.jp/](http://dxlib.o.oo7.jp/).

# Usage

 - 1. Prepare stimuli as a image file
 - 2. Write condition into .ini file 
 - 3. Execute and select your .ini file
 - 4. start by "T" (Check FPS and sequence by "D)
 
## INI-file SettingScreen

Example .ini file format is below (any comment is not allowed)

~~~~
[Display]
model=P241; not working
sizeX=1920; Display width
sizeY=1080; Display height
rate=60; Reflesh rate
nbit=32; bit
windowmode=1; windowmode (0: full screen) 

[Stim]
shuffle=0; shuffle or not (1 or 0)
interstim=12; inter stimuls interval [frames]
duration=60; stimuls duration
ntrial=5; number of repetition
intertrial=120; inter trial duration
posX=960; stimulus center X
posY=540; stimulus center Y
sequence=0; use sequence file (1 or 0) 
imgroot=images_\\; image file location
imgext=bmp; image file ext
bgcolor=127; background color

[Patch]
show=1; show patch (1 or 0)
posX=0; patch position X
posY=0; patch position X
size=100; patch size
~~~~

# Build
 - Visual studio 2015 community
 
# Dependency
 - boost
 - dxlib



