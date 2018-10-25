#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;

//Funcao que transforma string para numero
int stoi(string l){
	int num = 0;

	for(int i = 0 ; i < l.length(); i++)
		num +=  (l[i]-48) * pow(10, (l.length() - 1 - i));

	return num;
}

//Funcao que calcula o offset
int calc_offset(int n){
	int offset;
	offset = n % 16;
	return offset;
}

//Funcao que calcula o numero de pagina
int calc_pgnumber(int n){
	int pgnumber;
	pgnumber = n/16;
	return pgnumber;
}

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

		//Printa o header da tabela
		cout << "Tabela de entradas:\n"
		     << "ENT = entrada\n"
		     << "LSW =  Word menos significante (pagenumber + offset)\n"
		     << "PGNUM = Page number\n"
		     << "OFF = Off set\n\n" << endl ;

		cout << "ENT\tLSW\tPGNUM\tOFF\n" << endl;

		//Ler a linha do arquivo
		while(getline(entrada, line)){
			//Transforma para numero
			num = stoi(line);

			//Num_efetivo eh LSB do numero de entrada
			num_efetivo = num % 256;

			//Calcula o deslocamento e o numero de pagina
			offset = calc_offset(num_efetivo);
			pgnumber = calc_pgnumber(num_efetivo);

			//printar pagina e deslocamento
			cout << num << "\t"
			     << num_efetivo << "\t"
			     << pgnumber << "\t"
			     << offset << "\t" << endl;
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
