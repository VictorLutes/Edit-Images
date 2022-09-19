#include <iostream>
#include <cstring>
#include <cmath>
#include <vector>

using namespace std;

typedef struct{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
}RGB;

typedef struct{
	vector<double> minHue;
    vector<double> maxHue;
    vector<double> shiftHue;
    int numFaixas;
}HUE;

typedef struct{
	char assinatura[3];
	int sizefile;
	int res;
	int des;
	int tamanhomapa;
	int length;
	int height;
	short planos;
	short bpp;
	int compressao;
	int sizeimage;
	int pmh;
	int pmv;
	int ncu;
	int nci;
}Cabecalho;

#define ERROR -1

int clamp(double v){
    if(v<0) return 0;
    if(v>255) return 255;
    return (int) (v+0.5);
}

int maximize(int a, int b, int c){
    if(a>b){
        if(a>c) return a;
        return c;
    }
    else{
        if(b>c) return b;
        return c;
    }
}

int minimize(int a, int b, int c){
    if(a<b){
        if(a<c) return a;
        return c;
    }
    else{
        if(b<c) return b;
        return c;
    }
}

int applyGrayscale(int red, int green, int blue, int grayscale, int numGrays, int *error){
    int gray=(red+green+blue)/3;
    if(!grayscale){//luma
        gray=(red*0.2126+green*0.7152+blue*0.0722);
    }
    else if(grayscale==1){//desaturation
        gray=(maximize(red, green, blue)+minimize(red, green, blue))/2;
    }
    else if(grayscale==2){//custom
        int conversionFactor=255/(numGrays-1);
        int average=(red+green+blue)/3;
        gray=((int) (average/conversionFactor + 0.5))*conversionFactor;
    }
    else if(grayscale==3){//custom with dithering
        int conversionFactor=255/(numGrays-1);
        int average=(red+green+blue)/3;
        gray=((int) (average/conversionFactor + 0.5))*conversionFactor;
    }
    return gray;
}

double hue2RGB(double p, double q, double t){
    if(t < 0.01) t += 1;
    if(t > 1) t -= 1;
    if(t < 1/6.0) return p + (q - p) * 6 * t;
    if(t < 1/2.0) return q;
    if(t < 2/3.0) return p + (q - p) * (2/3 - t) * 6;
    return p;
}

RGB **PretoBranco(RGB **Paleta, RGB startColor, RGB endColor, HUE faixas, int effect, int greyscale, int numGrays){//retorna uma nova paleta em escalas de cinza
	int grey;
    double red, green, blue, hue, cosA, sinA;
    int matrix[3][3]={{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
	RGB **novaPaleta=(RGB **) malloc(256*sizeof(RGB *));
    
    
    if(effect==5){//select RGB range
        for(int i=0; i<256; i++){
            novaPaleta[i]=(RGB *) malloc(sizeof(RGB));
            grey=applyGrayscale(Paleta[i]->red, Paleta[i]->blue, Paleta[i]->green, greyscale, numGrays, 0);
            if(Paleta[i]->red>=startColor.red && Paleta[i]->red<=endColor.red && Paleta[i]->blue>=startColor.blue && Paleta[i]->blue<=endColor.blue && Paleta[i]->green>=startColor.green && Paleta[i]->green<=endColor.green){
                novaPaleta[i]->red=Paleta[i]->red;
                novaPaleta[i]->blue=Paleta[i]->blue;
                novaPaleta[i]->green=Paleta[i]->green;
            }
            else{
                novaPaleta[i]->red=grey;
                novaPaleta[i]->blue=grey;
                novaPaleta[i]->green=grey;
            }
        }
    }
    else if(effect==6){//select RGB interval
        for(int i=0; i<256; i++){
            novaPaleta[i]=(RGB *) malloc(sizeof(RGB));
            grey=applyGrayscale(Paleta[i]->red, Paleta[i]->blue, Paleta[i]->green, greyscale, numGrays, 0);
            if((Paleta[i]->red>=startColor.red && Paleta[i]->red<=endColor.red) || (Paleta[i]->blue>=startColor.blue && Paleta[i]->blue<=endColor.blue) || (Paleta[i]->green>=startColor.green && Paleta[i]->green<=endColor.green)){
                novaPaleta[i]->red=Paleta[i]->red;
                novaPaleta[i]->blue=Paleta[i]->blue;
                novaPaleta[i]->green=Paleta[i]->green;
            }
            else{
                novaPaleta[i]->red=grey;
                novaPaleta[i]->blue=grey;
                novaPaleta[i]->green=grey;
            }
        }
    }
    else if(effect==0){//Select Hue
        for(int i=0; i<256; i++){
            novaPaleta[i]=(RGB *) malloc(sizeof(RGB));
            grey=applyGrayscale(Paleta[i]->red, Paleta[i]->blue, Paleta[i]->green, greyscale, numGrays,0);
            red=Paleta[i]->red/255.0;
            green=Paleta[i]->green/255.0;
            blue=Paleta[i]->blue/255.0;
            hue=0;
            if(red >= green && green >= blue) hue = 60*((green-blue)/(red-blue));
            else if(green > red && red >= blue) hue = 60*(2-((red-blue)/(green-blue)));
            else if(green >= blue && blue > red) hue = 60*(2+((blue-red)/(green-red)));
            else if(blue > green && green > red) hue = 60*(4-((green-red)/(blue-red)));
            else if(blue > red && red >= green) hue = 60*(4+((red-green)/(blue-green)));
            else if(red >= blue && blue > green) hue = 60*(6-((blue-green)/(red-green)));
        
            novaPaleta[i]->red=grey;
            novaPaleta[i]->blue=grey;
            novaPaleta[i]->green=grey;

            for(int j=0; j<faixas.numFaixas; ++j){
                if(hue>=faixas.minHue[j] && hue<=faixas.maxHue[j]){
                    novaPaleta[i]->red=Paleta[i]->red;
                    novaPaleta[i]->blue=Paleta[i]->blue;
                    novaPaleta[i]->green=Paleta[i]->green;
                }
            }
        }
    }
    else if(effect==1){//Remove Hue
        for(int i=0; i<256; i++){
            novaPaleta[i]=(RGB *) malloc(sizeof(RGB));
            grey=applyGrayscale(Paleta[i]->red, Paleta[i]->blue, Paleta[i]->green, greyscale, numGrays, 0);
            red=Paleta[i]->red/255.0;
            green=Paleta[i]->green/255.0;
            blue=Paleta[i]->blue/255.0;
            hue=0;
            if(red >= green && green >= blue) hue = 60*((green-blue)/(red-blue));
            else if(green > red && red >= blue) hue = 60*(2-((red-blue)/(green-blue)));
            else if(green >= blue && blue > red) hue = 60*(2+((blue-red)/(green-red)));
            else if(blue > green && green > red) hue = 60*(4-((green-red)/(blue-red)));
            else if(blue > red && red >= green) hue = 60*(4+((red-green)/(blue-green)));
            else if(red >= blue && blue > green) hue = 60*(6-((blue-green)/(red-green)));

            novaPaleta[i]->red=Paleta[i]->red;
            novaPaleta[i]->blue=Paleta[i]->blue;
            novaPaleta[i]->green=Paleta[i]->green;

            for(int j=0; j<faixas.numFaixas; ++j){
                if(hue>=faixas.minHue[j] && hue<=faixas.maxHue[j]){
                    novaPaleta[i]->red=grey;
                    novaPaleta[i]->blue=grey;
                    novaPaleta[i]->green=grey;
                }
            }
        }
    }
    else if(effect==2){//Shift Hue
        for(int i=0; i<256; i++){
            novaPaleta[i]=(RGB *) malloc(sizeof(RGB));
            novaPaleta[i]->red=Paleta[i]->red;
            novaPaleta[i]->green=Paleta[i]->green;
            novaPaleta[i]->blue=Paleta[i]->blue;

            red=Paleta[i]->red/255.0;
            green=Paleta[i]->green/255.0;
            blue=Paleta[i]->blue/255.0;
            hue=0;
            if(red >= green && green >= blue) hue = 60*((green-blue)/(red-blue));
            else if(green > red && red >= blue) hue = 60*(2-((red-blue)/(green-blue)));
            else if(green >= blue && blue > red) hue = 60*(2+((blue-red)/(green-red)));
            else if(blue > green && green > red) hue = 60*(4-((green-red)/(blue-red)));
            else if(blue > red && red >= green) hue = 60*(4+((red-green)/(blue-green)));
            else if(red >= blue && blue > green) hue = 60*(6-((blue-green)/(red-green)));
    
            for(int j=0; j<faixas.numFaixas; ++j){
                if(hue>=faixas.minHue[j] && hue<=faixas.maxHue[j]){
                    cosA=cos(M_PI*(faixas.shiftHue[j])/180.0);
                    sinA=sin(M_PI*(faixas.shiftHue[j])/180.0);
                    matrix[0][0]=cosA+(1-cosA)/3.0;
                    matrix[0][1]=(1-cosA)/3.0-sqrt(1.0/3.0)*sinA;
                    matrix[0][2]=(1-cosA)/3.0+sqrt(1.0/3.0)*sinA;
                    matrix[1][0]=(1-cosA)/3.0+sqrt(1.0/3.0)*sinA;
                    matrix[1][1]=cosA+(1-cosA)/3.0;
                    matrix[1][2]=(1-cosA)/3.0-sqrt(1.0/3.0)*sinA;
                    matrix[2][0]=(1-cosA)/3.0-sqrt(1.0/3.0)*sinA;
                    matrix[2][1]=(1-cosA)/3.0+sqrt(1.0/3.0)*sinA;
                    matrix[2][2]=cosA+(1-cosA)/3.0;

                    novaPaleta[i]->red=clamp(Paleta[i]->red*matrix[0][0]+Paleta[i]->green*matrix[0][1]+Paleta[i]->blue*matrix[0][2]);
                    novaPaleta[i]->green=clamp(Paleta[i]->red*matrix[1][0]+Paleta[i]->green*matrix[1][1]+Paleta[i]->blue*matrix[1][2]);
                    novaPaleta[i]->blue=clamp(Paleta[i]->red*matrix[2][0]+Paleta[i]->green*matrix[2][1]+Paleta[i]->blue*matrix[2][2]);
                }
            }
        }
    }
    else if(effect==3){//Shift Hue and Grayscale
        for(int i=0; i<256; i++){
            novaPaleta[i]=(RGB *) malloc(sizeof(RGB));
            grey=applyGrayscale(Paleta[i]->red, Paleta[i]->blue, Paleta[i]->green, greyscale, numGrays, 0);
            novaPaleta[i]->red=grey;
            novaPaleta[i]->blue=grey;
            novaPaleta[i]->green=grey;
            
            red=Paleta[i]->red/255.0;
            green=Paleta[i]->green/255.0;
            blue=Paleta[i]->blue/255.0;
            hue=0;
            if(red >= green && green >= blue) hue = 60*((green-blue)/(red-blue));
            else if(green > red && red >= blue) hue = 60*(2-((red-blue)/(green-blue)));
            else if(green >= blue && blue > red) hue = 60*(2+((blue-red)/(green-red)));
            else if(blue > green && green > red) hue = 60*(4-((green-red)/(blue-red)));
            else if(blue > red && red >= green) hue = 60*(4+((red-green)/(blue-green)));
            else if(red >= blue && blue > green) hue = 60*(6-((blue-green)/(red-green)));
            for(int j=0; j<faixas.numFaixas; ++j){

                if(hue>=faixas.minHue[j] && hue<=faixas.maxHue[j]){
                    cosA=cos(M_PI*(faixas.shiftHue[j])/180.0);
                    sinA=sin(M_PI*(faixas.shiftHue[j])/180.0);
                    matrix[0][0]=cosA+(1-cosA)/3.0;
                    matrix[0][1]=(1-cosA)/3.0-sqrt(1.0/3.0)*sinA;
                    matrix[0][2]=(1-cosA)/3.0+sqrt(1.0/3.0)*sinA;
                    matrix[1][0]=(1-cosA)/3.0+sqrt(1.0/3.0)*sinA;
                    matrix[1][1]=cosA+(1-cosA)/3.0;
                    matrix[1][2]=(1-cosA)/3.0-sqrt(1.0/3.0)*sinA;
                    matrix[2][0]=(1-cosA)/3.0-sqrt(1.0/3.0)*sinA;
                    matrix[2][1]=(1-cosA)/3.0+sqrt(1.0/3.0)*sinA;
                    matrix[2][2]=cosA+(1-cosA)/3.0;

                    novaPaleta[i]->red=clamp(Paleta[i]->red*matrix[0][0]+Paleta[i]->green*matrix[0][1]+Paleta[i]->blue*matrix[0][2]);
                    novaPaleta[i]->green=clamp(Paleta[i]->red*matrix[1][0]+Paleta[i]->green*matrix[1][1]+Paleta[i]->blue*matrix[1][2]);
                    novaPaleta[i]->blue=clamp(Paleta[i]->red*matrix[2][0]+Paleta[i]->green*matrix[2][1]+Paleta[i]->blue*matrix[2][2]);
                }
            }
        }
    }
	else{
        for(int i=0; i<256; i++){
            novaPaleta[i]=(RGB *) malloc(sizeof(RGB));
            double q=0, p=0, lightness=0, saturation=0, maxRGB=maximize(Paleta[i]->red, Paleta[i]->green, Paleta[i]->blue)/255.0, minRGB=minimize(Paleta[i]->red, Paleta[i]->green, Paleta[i]->blue)/255.0;
            hue=faixas.shiftHue[0]/60.0;
            lightness=(maxRGB+minRGB)/2.0;
            if(maxRGB==minRGB){
                saturation=0;
                novaPaleta[i]->red=lightness;
                novaPaleta[i]->green=lightness;
                novaPaleta[i]->blue=lightness;
            }
            else{
                if(lightness>0.5){
                    saturation=(maxRGB-minRGB)/(2.0-maxRGB-minRGB);
                    q = lightness - saturation + lightness * saturation;
                }
                else{
                    saturation=(maxRGB-minRGB)/(maxRGB+minRGB);
                    q = lightness * (1 - saturation);
                }
                p=2*lightness-q;

                if(hue<1){
                    red=p;
                    blue=q;
                    green=hue*(p-q)+blue;
                }
                else if(hue>5){
                    red=p;
                    green=q;
                    blue=green-hue*(p-q);
                }
                else if(hue<3){
                    green=p;
                    if(hue<2){
                        blue=q;
                        red=blue-(hue-2)*(p-q);
                    }
                    else{
                        red=q;
                        blue=(hue-2)*(p-q)+red;
                    }
                }
                else{
                    blue=p;
                    if(hue<4){
                        red=q;
                        green=red-(hue-4)*(p-q);
                    }
                    else{
                        green=q;
                        red=(hue-4)*(p-q)+green;
                    }
                }

                novaPaleta[i]->red=red*255;//(hue2RGB(p, q, hue+0.333333)*255.0);
                novaPaleta[i]->green=green*255;//(hue2RGB(p, q, hue)*255.0);
                novaPaleta[i]->blue=blue*255;//(hue2RGB(p, q, hue-0.333333)*255.0);
            }
        }
    }
	return novaPaleta;
}

void writeNewFile(char *filename, Cabecalho *cabecalho, RGB **Paleta, unsigned char **imagem){//cria um novo aquivo com a imagem invertida o em preto e branco
	int i, j;
	unsigned char zero=0;
	RGB **novaPaleta=NULL;
    
    int color=0;
    char buffer[4];
    char *newname;
    int num;
    cout << "Numero de arquivos: ";
    cin >> num;
    RGB startColor[num], endColor[num];
    HUE faixas[num];
    int effect[num];
    int grayscale[num];
    int numGrays[num];
    for(int i=0; i<num; ++i){
        cout << "Arquivo" << i << ":\n";
        
        cout << "Effect: ";
        cin >> effect[i];
        if(effect[i]!=2 && effect[i]!=4){
            cout << "Grayscale formula: ";
            cin >> grayscale[i];
            if(grayscale[i]==2 || grayscale[i]==3){
                cout << "Number of shades of gray: ";
                cin >> numGrays[i]; 
            }
        }
        if(effect[i]>=5){
            cout << "R1: ";
            scanf("%d", &color);
            startColor[i].red=(unsigned char) color;
            cout << "R2: ";
            scanf("%d", &color);
            endColor[i].red=(unsigned char) color;

            cout << "G1: ";
            scanf("%d", &color);
            startColor[i].green=(unsigned char) color;
            cout << "G2: ";
            scanf("%d", &color);
            endColor[i].green=(unsigned char) color;

            cout << "B1: ";
            scanf("%d", &color);
            startColor[i].blue=(unsigned char) color;
            cout << "B2: ";
            scanf("%d", &color);
            endColor[i].blue=(unsigned char) color;
        }
        else if(effect[i]==4){
            cout << "colorizeHue: ";
            faixas[i].shiftHue.resize(1);
            scanf("%lf", &faixas[i].shiftHue[0]);
        }
        else{
            cout << "numFaixas: ";
            scanf("%d", &faixas[i].numFaixas);
            faixas[i].minHue.resize(faixas[i].numFaixas);
            faixas[i].maxHue.resize(faixas[i].numFaixas);
            faixas[i].shiftHue.resize(faixas[i].numFaixas);
            for(int j=0; j<faixas[i].numFaixas; ++j){
                cout << "minHue" << j << ": ";
                scanf("%lf", &faixas[i].minHue[j]);
                cout << "maxHue" << j << ": ";
                scanf("%lf", &faixas[i].maxHue[j]);
                if(effect[i]==3 || effect[i]==2){
                    cout << "shiftHue" << j << ": ";
                    scanf("%lf", &faixas[i].shiftHue[j]);
                }
            }
        }
    }

    for(int k=0; k<num; ++k){
        newname=NULL;
        newname=(char *) malloc(strlen(filename)+60);
        newname[0]='\0';
        strcat(newname, "./colors/");
        int plus=(int) strlen(newname);
        for(i=0; i<(int) strlen(filename)-4; i++){
            newname[i+plus]=filename[i];
        }
        newname[i+plus]='\0';
        if(effect[k]>=5){
            sprintf(buffer, "%d", startColor[k].red);
            strcat(newname, "R:");
            strcat(newname, buffer);
            
            sprintf(buffer, "%d", endColor[k].red);
            strcat(newname, "-");
            strcat(newname, buffer);

            sprintf(buffer, "%d", startColor[k].green);
            strcat(newname, "G:");
            strcat(newname, buffer);
            
            sprintf(buffer, "%d", endColor[k].green);
            strcat(newname, "-");
            strcat(newname, buffer);

            sprintf(buffer, "%d", startColor[k].blue);
            strcat(newname, "B:");
            strcat(newname, buffer);

            sprintf(buffer, "%d", endColor[k].blue);
            strcat(newname, "-");
            strcat(newname, buffer);
        }
        else if(effect[k]==0){
            strcat(newname, "Select_Hue:");
            sprintf(buffer, "%d", (int) faixas[k].minHue[0]);
            strcat(newname, buffer);
            
            sprintf(buffer, "%d", (int) faixas[k].maxHue[0]);
            strcat(newname, "-");
            strcat(newname, buffer);
        }
        else if(effect[k]==1){
            strcat(newname, "Remove_Hue:");
            sprintf(buffer, "%d", (int) faixas[k].minHue[0]);
            strcat(newname, buffer);
            
            sprintf(buffer, "%d", (int) faixas[k].maxHue[0]);
            strcat(newname, "-");
            strcat(newname, buffer);
        }
        else if(effect[k]==2 || effect[k]==3){
            strcat(newname, "Shift_Hue:");
            sprintf(buffer, "%d", (int) faixas[k].shiftHue[0]);
            strcat(newname, buffer);
            strcat(newname, "Range:");
            sprintf(buffer, "%d", (int) faixas[k].minHue[0]);
            strcat(newname, buffer);
            sprintf(buffer, "%d", (int) faixas[k].maxHue[0]);
            strcat(newname, "-");
            strcat(newname, buffer);
        }
        else if(effect[k]==4){
            strcat(newname, "Colorize:");
            sprintf(buffer, "%d", (int) faixas[k].shiftHue[0]);
            strcat(newname, buffer);
        }
        if(effect[k]!=2 && effect[k]!=4){
            strcat(newname, "Grayscale:");
            sprintf(buffer, "%d", (int) grayscale[k]);
            strcat(newname, buffer);
            if(grayscale[k]==2 || grayscale[k]==3){
                strcat(newname, "Shades:");
                sprintf(buffer, "%d", (int) numGrays[k]);
                strcat(newname, buffer);
            }
        }
        strcat(newname, ".bmp");

        novaPaleta=PretoBranco(Paleta, startColor[k], endColor[k], faixas[k], effect[k], grayscale[k], numGrays[k]);

        FILE *fp=fopen(newname, "wb");
        fwrite(&cabecalho->assinatura, sizeof(char), 2, fp);
        fwrite(&cabecalho->sizefile, sizeof(int), 1, fp);
        fwrite(&cabecalho->res, sizeof(int), 1, fp);
        fwrite(&cabecalho->des, sizeof(int), 1, fp);
        fwrite(&cabecalho->tamanhomapa, sizeof(int), 1, fp);
        fwrite(&cabecalho->length, sizeof(int), 1, fp);
        fwrite(&cabecalho->height, sizeof(int), 1, fp);
        fwrite(&cabecalho->planos, sizeof(short), 1, fp);
        fwrite(&cabecalho->bpp, sizeof(short), 1, fp);
        fwrite(&cabecalho->compressao, sizeof(int), 1, fp);
        fwrite(&cabecalho->sizeimage, sizeof(int), 1, fp);
        fwrite(&cabecalho->pmh, sizeof(int), 1, fp);
        fwrite(&cabecalho->pmv, sizeof(int), 1, fp);
        fwrite(&cabecalho->ncu, sizeof(int), 1, fp);
        fwrite(&cabecalho->nci, sizeof(int), 1, fp);
        
        for(i=0; i<256; i++){
            fwrite(&novaPaleta[i]->blue, sizeof(unsigned char), 1, fp);
            fwrite(&novaPaleta[i]->green, sizeof(unsigned char), 1, fp);
            fwrite(&novaPaleta[i]->red, sizeof(unsigned char), 1, fp);
            fwrite(&zero, sizeof(unsigned char), 1, fp);
        }
        int paddedlength=cabecalho->length+(4-cabecalho->length%4)%4;//largura da imagem incluindo o padding
        for(i=cabecalho->height-1; i>=0; i--){
            for(j=0; j<cabecalho->length; j++){
                fwrite(&imagem[i][j], sizeof(unsigned char), 1, fp);
            }
            for(j=cabecalho->length; j<paddedlength; j++){
                fwrite(&imagem[i][j], sizeof(unsigned char), 1, fp);
            }
        }
        fclose(fp);
        for(i=0; i<256; i++){
            free(novaPaleta[i]);
        }
        free(newname);
        free(novaPaleta);
    }
}

int readBMP(char *filename){//lê o cabecalho do arquivo.bmp, a paleta e os dados da imagem
	FILE *fp=fopen(filename, "rb");
	if(!fp){//se nao foi possivel abrir o arquivo
		printf("Erro no arquivo\n");
		return ERROR;
	}
	int i, j, paddedlength=0;
	Cabecalho *cabecalho=NULL;
	cabecalho=(Cabecalho *) malloc(sizeof(Cabecalho));

	fread(&cabecalho->assinatura, sizeof(char), 2, fp);
	cabecalho->assinatura[2]='\0';
	if(cabecalho->assinatura[0]!='B' || cabecalho->assinatura[1]!='M'){//se o cabecalho nao for BM
		printf("Arquivo nao eh do formato BMP\n");
		free(cabecalho);
		return ERROR;
	}
	fread(&cabecalho->sizefile, sizeof(int), 1, fp);
	fread(&cabecalho->res, sizeof(int), 1, fp);
	fread(&cabecalho->des, sizeof(int), 1, fp);
	fread(&cabecalho->tamanhomapa, sizeof(int), 1, fp);
	fread(&cabecalho->length, sizeof(int), 1, fp);
	fread(&cabecalho->height, sizeof(int), 1, fp);
	fread(&cabecalho->planos, sizeof(short), 1, fp);
	fread(&cabecalho->bpp, sizeof(short), 1, fp);
	fread(&cabecalho->compressao, sizeof(int), 1, fp);
	fread(&cabecalho->sizeimage, sizeof(int), 1, fp);
	fread(&cabecalho->pmh, sizeof(int), 1, fp);
	fread(&cabecalho->pmv, sizeof(int), 1, fp);
	fread(&cabecalho->ncu, sizeof(int), 1, fp);
	fread(&cabecalho->nci, sizeof(int), 1, fp);
	
	RGB **Paleta=NULL;
	Paleta=(RGB **) malloc(256*sizeof(RGB *));
	for(i=0; i<256; i++){
		Paleta[i]=(RGB *) malloc(sizeof(RGB));
		fread(&Paleta[i]->blue, sizeof(unsigned char), 1, fp);
		fread(&Paleta[i]->green, sizeof(unsigned char), 1, fp);
		fread(&Paleta[i]->red, sizeof(unsigned char), 1, fp);
		fseek(fp, 1, SEEK_CUR);
	}
	unsigned char **imagem=NULL;
	imagem=(unsigned char **) malloc(cabecalho->height*sizeof(unsigned char *));
	paddedlength=cabecalho->length+(4-cabecalho->length%4)%4;//largura da imagem incluindo o padding
	for(i=cabecalho->height-1; i>=0; i--){
		imagem[i]=(unsigned char *) malloc(paddedlength*sizeof(unsigned char));
		for(j=0; j<cabecalho->length; j++){
			fread(&imagem[i][j], sizeof(unsigned char), 1, fp);
		}
		for(j=cabecalho->length; j<paddedlength; j++){
			fread(&imagem[i][j], sizeof(unsigned char), 1, fp);
		}
	}
	fclose(fp);

	cout << cabecalho->bpp << "\n";
	writeNewFile(filename, cabecalho, Paleta, imagem);//cria o arquivo da imagem nova e retorna a paleta nova
	for(i=cabecalho->height-1; i>=0; i--){
		free(imagem[i]);
	}
	free(imagem);
	free(cabecalho);
	for(i=0; i<256; i++){
		free(Paleta[i]);
	}
	free(Paleta);
	return 0;
}

int main(int argc, char *argv[]){
	char filename[30];
    cout << "Nome do arquivo: ";
	scanf("%s", filename);
    cout << "Efeitos:\n\t0: Select Hue\n\t1: Remove Hue\n\t2: Shift Hue\n\t3: Shift Hue and Grayscale\n\t4: Colorize\n\t5: Select RGB range\n\t6: Select RGB intervals\n";
    cout << "Grayscale:\n\t0: luma\n\t1: desaturation\n\t2: Custom Shades of Gray\n\t3: Custom Shades of Gray with Dithering\n";    
	if(!strstr(filename, ".bmp")){//se o arquivo nao for .bmp
		printf("Arquivo nao eh do formato BMP\n");
		return 0;
	}
	readBMP(filename);
	return 0;
}
