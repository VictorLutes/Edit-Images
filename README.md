# Edit-Images
C++ program to edit bitmap images

Compile with g++ -o out colors.cpp

after executing you will be prompted to input the name of the file you want to edit and the effect you want to apply:
0: Select Hue - applies grayscale effect to every color outside of the hue interval selected
1: Remove Hue - applies grayscale effect to only to colors inside of the hue interval selected
2: Shift Hue - shifts the hue of colors inside the range selected by a certain amount
3: Shift Hue and Grayscale - shifts the hue of colors inside the range selected by a certain amount and applies grayscale to the rest
4: Colorize
5: Select RGB range
6: Select RGB intervals
