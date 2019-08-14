#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <windows.h>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
	vector<int> compression_params; //vector que guarda os parametros para salvar a imagem
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY); //formato da imagem (jpg)
	compression_params.push_back(98); //squalidade da imagem (0 a 100)

	long long int tempo1, tempo2, diftempo; //variaveis que vão guardar o tempo para fazer a atualização da imagem de referencia

	VideoCapture cap; //cria a variavel para abrir a camera (web cam)
	cap.open(1); // abre a webcam(0), se tiver outras cameras muda-se o numero(1,2,3...)
	Sleep(1000); // pausa o código durante 1 segundo para a camera estabilizar

	if (!cap.open(1)) { // verifica se a camera abriu, se não envia a mensagem de erro e termina o programa
		cout << "ERRO\n";
		system("PAUSE");
		return 0;
	}

	for (;;) // loop infinito
	{
		tempo1 = GetTickCount(); //pega o tempo de execuçao do programa até aqui

		Mat frame; //cria uma matriz
		cap >> frame; // captura uma matriz(imagem) através da variavel da camera
		if (frame.empty()) break; // se não capturar nenhum frame encerra o programa

		Mat img(650, 600, CV_16UC3, Scalar(0, 50000, 50000)); //cria uma imagem ( 3 channels, 16 bit image depth, 650 high, 600 wide, (0, 50000, 50000) assigned for Blue, Green and Red plane respectively. )

		if (img.empty()) { //se não criar a imagem encerra o programa
			cout << "ERROR : Image cannot be loaded..!!" << endl;
			return -1;
		}
		img = frame; // atribue o que foi capturado pela webcam na imagem criada

		//namedWindow("Reference", CV_WINDOW_AUTOSIZE); // cria uma janela com o nome referencia
		imwrite("Reference.jpg", img, compression_params); //salva a imagem no computador (nome_do_arquivo,imagem,parametros de compressão definidos no comeco do codigo) 
		//imshow("Reference", img); //mostra a imagem(img) na janela que voce abriu

		for (;;) { //loop infinito

			tempo2 = GetTickCount(); //pega o tempo de execuçao do programa até aqui

			Mat frame2;
			cap >> frame2;
			if (frame2.empty()) break; // end of video stream

			Mat img2(650, 600, CV_16UC3, Scalar(0, 50000, 50000)); //create an image ( 3 channels, 16 bit image depth, 650 high, 600 wide, (0, 50000, 50000) assigned for Blue, Green and Red plane respectively. )

			if (img2.empty()) //check whether the image is loaded or not
			{
				cout << "ERROR : Image cannot be loaded..!!" << endl;
				system("PAUSE");
				return -1;
			}
			img2 = frame2;

			//namedWindow("NewImage", CV_WINDOW_AUTOSIZE);
			imwrite("Image.jpg", img2, compression_params);
			//imshow("NewImage", img2);

			Mat ref = imread("Reference.jpg", CV_LOAD_IMAGE_UNCHANGED);
			Mat img = imread("Image.jpg", CV_LOAD_IMAGE_UNCHANGED);

			Mat dif; //cria uma matriz
			absdiff(ref, img, dif);// salva a diferença de ref e img em diffImage

			Mat foregroundMask = Mat::zeros(dif.rows, dif.cols, CV_8UC1); //cria uma matriz e preenche ela com zeros

			float threshold = 1000.0f; // valor de referencia para a variação
			float dist;
			long long int valor_dif = 0; //valor numérico da diferença

			for (int j = 0; j < dif.rows; ++j) //percorre a imagem(matriz) dif 
				for (int i = 0; i < dif.cols; ++i)
				{
					Vec3b pix = dif.at<Vec3b>(j, i); // acessa os valores(valor do pixel) da imagem nos 3 canais

					dist = (pix[0] * pix[0] + pix[1] * pix[1] + pix[2] * pix[2]); // operação entre os valores dos canais 1, 2 (RGB)
					dist = sqrt(dist); // tira a raiz quadrada do valor calculado acima

					if (dist > threshold) // se houvel variação maior que a referencia
					{
						foregroundMask.at<unsigned char>(j, i) = 126; // muda a cor do pixel para branco(255)
						valor_dif++; //incrementa a variavel para pegar o valor numerico da variação
					}
				}

			cout << valor_dif << endl; // mostra o valor da variação

			//namedWindow("MyWindow3", CV_WINDOW_AUTOSIZE);
			imwrite("Dif.jpg", dif, compression_params);
			//imshow("MyWindow3", dif);

			//namedWindow("MyWindow4", CV_WINDOW_AUTOSIZE);
			imwrite("Dif2.jpg", foregroundMask, compression_params);
			//imshow("MyWindow4", foregroundMask);

			diftempo = tempo2 - tempo1;
			if (diftempo >= 10000) { //se a diferenca entre os tempos for maior ou igual a 10seg atualiza a imagem de referencia
				tempo1 = tempo2 = 0;
				cout << "ok\n";
				break;
			}
		}
	}

	return 0;
}