#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;


int my_stoi(string);					//Transforma string para numero
int calc_offset(int);   			//Calcula o offset
int calc_pgnumber(int); 			//Calcula o numero de pagina
void print_header(void);			//Printa o titulo da tabela
void print_line(int,int,int,int);	//Printa as linhas da tabela

struct TLB {
	int pagenumber;
	int framenumber;
};

int page_table[256];

int main(int argc, char *argv[]){

	string line;
	int num, num_efetivo, offset, pgnumber;
	fstream entrada;

	//Abrir o arquivo que o usuario pede
	//Caso nao seja pedido nada sera aberto 'enderecos.txt'
	if(argc > 1)
		entrada.open(argv[1]);
	else
		entrada.open("enderecos.txt");

	//Testar se o arquivo ta aberto
	if (entrada.is_open()){
		cout << "Arquivo aberto com sucesso!\n" << endl;

		print_header();

		//Ler a linha do arquivo
		while(getline(entrada, line)){
			//Transforma para numero
			num = my_stoi(line);
			//Num_efetivo eh LSW do numero de entrada
			num_efetivo = num & 0x0000FFFF;
			//Calcula o deslocamento e o numero de pagina
			offset = calc_offset(num_efetivo);
			pgnumber = calc_pgnumber(num_efetivo);

			print_line(num, num_efetivo, pgnumber, offset);
	}

		//Fechar o arquivo
		entrada.close();
	}
	else{
		cerr << "Erro na abertura do arquivo" << endl;
		cout << "Tente renomear o arquivo para 'enderecos.txt'" << endl;
	}

	return 0;
}

//Funcao que transforma string para numero
int my_stoi(string l){
	int num = 0;
	for(int i = 0 ; i < l.length()-1; i++)
		num +=  (l[i]-0x30) * pow(10, (l.length() - 2 - i));
	return num;
}

//Funcao que calcula o offset
int calc_offset(int n){
	int offset;
	offset = n & 0x00FF;
	return offset;
}

//Funcao que calcula o numero de pagina
int calc_pgnumber(int n){
	int pgnumber;
	pgnumber = (n & 0xFF00)>>8;
	return pgnumber;
}

void print_header(void){
	cout << "Tabela de entradas:\n"
	     << "ENT = entrada\n"
	     << "LSW =  Word menos significante (pagenumber + offset)\n"
	     << "PGNUM = Page number\n"
	     << "OFF = Off set\n\n" << endl ;

	cout << "ENT\tLSW\tPGNUM\tOFF\n" << endl;
}

void print_line(int info1, int info2, int info3, int info4){
	cout << info1 << "\t"
	     << info2 << "\t"
	     << info3 << "\t"
	     << info4 << "\t" << endl;
}