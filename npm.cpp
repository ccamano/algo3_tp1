

#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <set>
#include <map>

using namespace std;

//Información de la instancia a resolver.
int M; //límite de contagio
int n; //cantidad de locales
vector< pair<int, int>> S; //vector con pares (beneficio,contagio), cada par es un negocio

int sum(vector< pair<int, int>>& V) {

    int suma = 0;

    for(int i=0; i<V.size(); i++)
        suma += V[i].first;

    return suma;

}

int INFTY = 10e6; 
// i: indice actual
// j: indice del último local que se agrego el beneficio
// b: beneficio acumulado hasta el momento
// c: contagio acumulado hasta el momento
// adyacente: bool que dice si hubo un local adyacente agregado en algún momento


int FB(int i, int j, int b,int c, bool adyacente){

    //Caso base
    if (i == n) return ((c <= M and !adyacente) ? b : -INFTY);

    //Chequea si se agrego el local anterior
    bool adyAgregado = (j >=0 && i - j == 1);
    //Recursión
    return max( FB(i+1,i,b+S[i].first,c+S[i].second, adyAgregado or adyacente),
                FB(i+1,j,b,c,adyacente));
}


// i: indice actual
// j: indice del último local que se agrego el beneficio
// b: beneficio acumulado hasta el momento
// c: contagio acumulado hasta el momento
// adyacente: bool que dice si hubo un local adyacente agregado en algún momento

bool poda_factibilidad; //define si la poda por factibilidad esta habilitada
bool poda_optimalidad; //define si la poda por optimalidad está habilitada
int V = -1; //mejor solucion hasta el momento

int BT(int i, int j, int b,int c, bool adyacente, int sumRes){

    //Caso base
    if (i == n){
        if(poda_optimalidad && b > V && c <= M and !adyacente){
            V = b;
        }
        return (c <= M and !adyacente) ? b : -INFTY;
    }
    //Chequea si se agrego el local anterior
    bool nuevo = (j >= 0 && i - j == 1);

    //Poda por factibilidad según el contagio.
    if(poda_factibilidad && c > M) return -INFTY;

    if(poda_optimalidad){
        if(b + sumRes <= V){
            return -INFTY;
        }
    }
    //Recursión
    return max( BT(i+1,i,b+S[i].first,c+S[i].second, adyacente or nuevo, sumRes-S[i].first),
                BT(i+1,j,b,c,adyacente, sumRes-S[i].first));

}

//PROGRAMACIÓN DINÁMICA

vector<vector<int>> MEM;
const int UNDEFINED = -1;
// PD(i, w): minima contaminacion de un subconjunto de locales de {Si, ... , Sn} que sume M−c.

int DP(int i, int j, int c, bool adyacente)
{
	if (c > M) return -INFTY;
	if(adyacente) return -INFTY;
	if (i == n) return 0;

	if (MEM[i][c] == UNDEFINED) {
		bool nuevo = (j >= 0 && i - j == 1);
		MEM[i][c] = max(DP(i+1, j, c, adyacente), //si fuera true entonces caeria en la 2da linea.
                    	S[i].first + DP(i+1, i, c+S[i].second, nuevo || adyacente));
	}

	return MEM[i][c];
}


//main

int main(int argc, char** argv)
{
    // Leemos el parametro que indica el algoritmo a ejecutar.
    map<string, string> algoritmos_implementados = {
            {"FB", "Fuerza Bruta"}, {"BT", "Backtracking con podas"}, {"BT-F", "Backtracking con poda por factibilidad"},
            {"BT-O", "Backtracking con poda por optimalidad"} , {"DP", "Programacion dinámica"}
    };

    // Verificar que el algoritmo pedido exista.
    if (argc < 2 || algoritmos_implementados.find(argv[1]) == algoritmos_implementados.end())
    {
        cerr << "Algoritmo no encontrado: " << argv[1] << endl;
        cerr << "Los algoritmos existentes son: " << endl;
        for (auto& alg_desc: algoritmos_implementados) cerr << "\t- " << alg_desc.first << ": " << alg_desc.second << endl;
        return 0;
    }
    string algoritmo = argv[1];

    // Leemos el input.
    cin >> n >> M;
    S.assign(n,(pair<int,int>(0,0)));
    for (int i = 0; i < n; ++i) {
        int x;
        int y;
        cin >> x >> y;
        pair<int,int> p = make_pair(x,y);
        S[i] = p;
    }

    // Ejecutamos el algoritmo y obtenemos su tiempo de ejecución.
    int optimum;
    optimum = -1;
    auto start = chrono::steady_clock::now();
    if (algoritmo == "FB")
    {
        optimum = FB(0, -1, 0, 0, false);
    }
    else if (algoritmo == "BT")
    {
        K = -1;
        poda_optimalidad = poda_factibilidad = true;
        int sumaMax = sum(S);
        optimum = BT(0, -2, 0, 0, false, sumaMax);
    }
    else if (algoritmo == "BT-F")
    {
        K = -1;
        poda_optimalidad = false;
        poda_factibilidad = true;
        int sumaMax = sum(S);
        optimum = BT(0, -1, 0, 0, false, sumaMax);
    }
    else if (algoritmo == "BT-O")
    {
        K = -1;
        poda_optimalidad = true;
        poda_factibilidad = false;
        int sumaMax = sum(S);
        optimum = BT(0, -2, 0, 0, false, sumaMax);
    }
    else if (algoritmo == "DP")
    {
        // Precomputamos la solucion para los estados.
        MEM = vector<vector<int>>(n+1, vector<int>(M+1, UNDEFINED));
       for (int i = 0; i < n+1; ++i)
            for (int j = 0; j < M+1; ++j){
                DP(i, -2, j, false);
               }

     optimum = DP(0, -1, 0, false);  
    }


    auto end = chrono::steady_clock::now();
    double total_time = chrono::duration<double, milli>(end - start).count();

    // Imprimimos el tiempo de ejecución por stderr.
    clog << total_time << endl;

    // Imprimimos el resultado por stdout.
    cout << (optimum == -INFTY ? -1 : optimum) << endl;
    return 0;
}


