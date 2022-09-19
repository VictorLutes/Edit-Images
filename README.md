# Edit-Images
C++ program to edit bitmap images

Compile with g++ -o out colors.cpp\
After executing you will be prompted to input the name of the file you want to edit and the effect you want to apply:

Effects:\
    0: Select Hue - applies grayscale effect to every color outside of the hue interval selected\
    1: Remove Hue - applies grayscale effect to only to colors inside of the hue interval selected\
    2: Shift Hue - shifts the hue of colors inside the range selected by a certain amount\
    3: Shift Hue and Grayscale - shifts the hue of colors inside the range selected by a certain amount and applies grayscale to the rest\
    4: Colorize - Changes the hue of an image to the specified value\
    5: Select RGB range - applies grayscale effect to every color outside of the rgb interval selected\
    6: Select RGB intervals - applies grayscale effect to every color outside of the R, G and B interval selected\

For the effects that apply grayscale there are 4 different types:\
    0: luma - R*0.2126+G*0.7152+B*0.0722\
    1: desaturation - (max(R, G, B)+min(R, G, B))/2\
    2: Custom Shades of Gray - enter the number of shades of gray to use\
    3: Custom Shades of Gray with Dithering - enter the number of shades of gray to use and apply dithering\

<object data="https://github.com/VictorLutes/Edit-Images/blob/main/Examples.pdf" type="application/pdf" width="700px" height="700px">
    <embed src="http://yoursite.com/the.pdf">
        <p>This browser does not support PDFs. Please download the PDF to view it: <a href="http://yoursite.com/the.pdf">Download PDF</a>.</p>
    </embed>
</object>
