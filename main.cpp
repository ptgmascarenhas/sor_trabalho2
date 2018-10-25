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

int main(){
	string line;
	int num, num_efetivo, offset, pgnumber;
	fstream entrada;
	
	//Abrir o arquivo
	entrada.open("enderecos.txt");

	//Testar se o arquivo ta aberto
	if (entrada.is_open()){
		cout << "Arquivo aberto com sucesso!\n" << endl; 
			 
		//Ler a linha do arquivo
		while(getline(entrada, line)){
			//Transforma para numero
			num = stoi(line);	
			
			//Calcula o deslocamento e o numero de pagina
			num_efetivo = num % 256;
			
			offset = calc_offset(num_efetivo);
			pgnumber = calc_pgnumber(num_efetivo);
			
			//printar pagina e deslocamento
			cout << num << "\t"
				 << num_efetivo << "\t"
				 << offset << "\t"
				 << pgnumber << "\t"<< endl;
		}
		
		//Fechar o arquivo
		entrada.close();
	}
	else
		cerr << "Erro na abertura do arquivo" << endl;
	
	return 0;
}
