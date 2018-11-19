#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

#define TLB_SIZE 16
#define PAGETABLE_SIZE 256
#define MEMORY_SIZE 256

using namespace std;

struct TLB {
	int tlb_page_number;
	int tlb_frame_number;
};

struct PageTable {
	int pt_present_bit;
	int pt_frame_number;
};

int my_stoi(string);				 	 					 //Transforma string para numero
int calc_offset(int);   			 				 	 //Calcula o offset
int calc_pgnumber(int); 			 	 				 //Calcula o numero de pagina
void print_header(void);			 	 				 //Printa o titulo da tabela
void print_line(int,int,int,int,char);	 //Printa as linhas da tabela
void print_error(void);					 				 //Printa mensagem de erro
void print_estatisticas(int,int,int,int);//Printa as estatisticas

void print_tlb(TLB t[]);			 	 				 //Printa a TLB
void print_pagetable(PageTable t[]); 		 //Printa a tabela de paginas
void clean_tlb(TLB t[]);			 	 				 //Limpa a TLB
void clean_pagetable(PageTable t[]); 	 	 //Limpa a tabela de paginas

int check_tlb(TLB t[], int);  					 //Procura por um elemento na TLB
void set_tlb(TLB t[],int,int); 			 		 //Preenche um registro na TLB

int check_pagetable(PageTable t[], int); //Procura por um elemento
void set_pagetable(PageTable t[],int);   //Preenche um registro na pt

int  point_tlb = 0, point_memo = 0;

int main(int argc, char *argv[]){

	string line;
	char memo_buffer[MEMORY_SIZE], caractere;
	int num, num_efetivo, offset, pgnumber, map, tlb_hit, loc;
	int numentradas = 0, faltasdepagina = 0, acertostlb = 0, acertospt = 0;
	fstream entrada, backstore;

	string *memory = new string[MEMORY_SIZE];
	PageTable *pagetable = new PageTable[PAGETABLE_SIZE];
	TLB *tlb = new TLB[TLB_SIZE];

	clean_pagetable(pagetable);
	clean_tlb(tlb);

	//Abrir o arquivo que o usuario pede
	//Caso nao seja pedido nada sera aberto 'enderecos.txt'
	if(argc > 1)
		entrada.open(argv[1]);
	else
		entrada.open("enderecos.txt");

	backstore.open("BACKSTORE.bin");

	//Testar se o arquivo ta aberto
	if (entrada.is_open() && backstore.is_open()){
		cout << "Arquivos abertos com sucesso!\n" << endl;

		print_header();

		//Ler a linha do arquivo de entrada
		while(getline(entrada, line)){
			//Transforma para numero
			num = my_stoi(line);
			//Num_efetivo eh LSW do numero de entrada
			num_efetivo = num & 0x0000FFFF;
			//Calcula o deslocamento e o numero de pagina
			offset = calc_offset(num_efetivo);
			pgnumber = calc_pgnumber(num_efetivo);

			tlb_hit = check_tlb(tlb, pgnumber);

			// Se a pagina nao estiver no TLB
			if (tlb_hit == -1) {
				//Checa se a pagina esta na tabela de paginas
				map = check_pagetable(pagetable, pgnumber);

				//Preenche TLB ou com a posicao da memoria atual (map = -1)
				//Ou com o frame daquela page (map = frame)
				set_tlb(tlb, pgnumber, map);

				//Se a pagina nao estiver mapeada em memoria
				if (map == -1) {
					//Sera necessario alocar um quadro
					backstore.seekg(pgnumber*MEMORY_SIZE);
					backstore.read(memo_buffer, MEMORY_SIZE);

					//Coloca a pagina na memoria
					memory[point_memo & 0xFF] = memo_buffer;

					loc = point_memo & 0xFF;

					//Atualiza a Page Table e pega o caractere
					set_pagetable(pagetable, pgnumber);

					//Atualiza estatisticas
					faltasdepagina++;
				}
				else {
					//Pagina estava na PT
					loc = map;
					acertospt++;
				}
			}
			else {
				// Pagina estava no TLB
				loc = tlb_hit;
				acertostlb++;
			}

			//Pega caractere naquele offset na pagina da memoria
			caractere = memory[loc][offset];
			print_line(num, num_efetivo, pgnumber, offset, caractere);
			numentradas++;
		}

		print_tlb(tlb);
		print_pagetable(pagetable);
		print_estatisticas(numentradas,faltasdepagina,acertostlb,acertospt);

		//Fechar o arquivo
		entrada.close();
		backstore.close();

	}else{
		print_error();
	}

	delete [] memory;
	delete [] tlb;
	delete [] pagetable;

	return 0;
}

int my_stoi(string l){
	int num = 0;
	for(int i = 0 ; i < l.length()-1; i++)
		num +=  (l[i]-0x30) * pow(10, (l.length() - 2 - i));
	return num;
}

int calc_offset(int n){
	return 	n & 0xFF;
}

int calc_pgnumber(int n){
	return  n>>8;
}

void print_header(void){
	cout << "Tabela de entradas:\n"
	     << "ENT = entrada\n"
	     << "LSW =  Word menos significante (pagenumber + offset)\n"
	     << "PGNUM = Page number\n"
	     << "OFF = Off set\n"
	     << "CHAR = caractere na posicao\n\n"<< endl ;

	cout << "ENT\tLSW\tPGNUM\tOFF\tCHAR\n" << endl;
}

void print_error(void){
	cerr << "Erro na abertura de arquivo" << endl << endl;
	cout << "Tente renomear o arquivo de entrada para 'enderecos.txt'\n"
		   << "Verifique se existe um 'BACKSTORE.bin' no diretório\n" << endl;
}

void print_line(int info1, int info2, int info3, int info4, char info5){
	cout << info1 << "\t"
	     << info2 << "\t"
	     << info3 << "\t"
	     << info4 << "\t"
	  	 << info5 << "\t" << endl;
}

void print_tlb(TLB t[]){
	cout << "\n\nTLB:" << endl
	  	 << endl << "\tPN" << "\tFN\n" << endl;

	for(int i = 0; i < TLB_SIZE; i++)
		cout << i << "\t"
			   << t[i].tlb_page_number << "\t"
		 	   << t[i].tlb_frame_number << endl;
}

void print_pagetable(PageTable t[]){
	cout << "\n\nTabela de paginas: " << endl
	  	 << endl << "\tFN" << "\tPB\n" << endl;

	for(int i = 0; i < PAGETABLE_SIZE; i++)
		cout << i << "\t"
			   << t[i].pt_frame_number << "\t"
		  	 << t[i].pt_present_bit << endl;
}

void print_estatisticas(int info1, int info2, int info3, int info4){
	cout << "\n\nNumero de entradas: " << info1
	     << "\nNumero de faltas de pagina: " << info2
			 << " (" << info2*100/info1 << "%)"
	     << "\nQuantidade de acertos na TLB: " << info3
			 << " (" << info3*100/info1 << "%)"
	     << "\nQuantidade de acertos na Tabela de Paginas: "  << info4
			 << " (" << info4*100/info1 << "%)" << endl;
}

void clean_tlb(TLB t[]){
	for(int i = 0; i < TLB_SIZE; i++){
		t[i].tlb_page_number = -1;
		t[i].tlb_frame_number = -1;
	}
}

void clean_pagetable(PageTable t[]){
	for(int i = 0; i < PAGETABLE_SIZE; i++){
		t[i].pt_frame_number = -1;
		t[i].pt_present_bit = 0;
	}
}

int check_tlb (TLB t[], int pg) {
	for (int i = 0; i < TLB_SIZE; i++) {
		if (t[i].tlb_page_number == pg)
			return t[i].tlb_frame_number;;
	}
	return -1;
}

void set_tlb (TLB t[], int pg, int map) {
	int position = point_tlb & 0xF;
	t[position].tlb_page_number = pg;

	if(map == -1)
		t[position].tlb_frame_number = point_memo & 0xFF;
	else
		t[position].tlb_frame_number = map;

	point_tlb++;
}

int check_pagetable(PageTable t[], int pg){
	int convert;
	convert = t[pg].pt_frame_number;
	return convert;
}

void set_pagetable(PageTable t[], int pg){
	t[pg].pt_frame_number = point_memo & 0xFF;
	t[pg].pt_present_bit = 1;
	point_memo++;
}
